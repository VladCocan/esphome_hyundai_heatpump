#include "hyundai_heatpump_device.h"

#include <cstring>

#include "hyundai_heatpump.h"
#include "log.h"

#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hyundai_heatpump {

using namespace protocol;

static const char *const DEVICE_TAG = "hyundai_heatpump.device";

namespace {

void log_range_read(uint16_t start_addr, uint16_t end_addr) {
  if (!debug_log_messages) {
    return;
  }
  const char *start_name = register_name(start_addr);
  const char *end_name = register_name(end_addr);
  if (start_name != nullptr && end_name != nullptr) {
    ESP_LOGV(DEVICE_TAG, "Read range %u-%u (%s -> %s)", start_addr, end_addr, start_name, end_name);
    return;
  }
  ESP_LOGV(DEVICE_TAG, "Read range %u-%u", start_addr, end_addr);
}

void log_key_value(uint16_t address, float value, const char *suffix = nullptr) {
  if (!debug_log_messages) {
    return;
  }
  if (debug_log_messages_on_change && !log_should_print(log_dedup_key(address), value, 0.001f)) {
    return;
  }
  const char *name = register_name(address);
  if (name != nullptr && suffix != nullptr) {
    ESP_LOGV(DEVICE_TAG, "%s [%u] = %.2f %s", name, address, value, suffix);
    return;
  }
  if (name != nullptr) {
    ESP_LOGV(DEVICE_TAG, "%s [%u] = %.2f", name, address, value);
    return;
  }
  ESP_LOGV(DEVICE_TAG, "Register %u = %.2f", address, value);
}

void log_key_text(uint16_t address, const std::string &value) {
  if (!debug_log_messages) {
    return;
  }
  if (debug_log_messages_on_change && !log_should_print(log_dedup_key(address), value)) {
    return;
  }
  const char *name = register_name(address);
  if (name != nullptr) {
    ESP_LOGV(DEVICE_TAG, "%s [%u] = %s", name, address, value.c_str());
    return;
  }
  ESP_LOGV(DEVICE_TAG, "Register %u = %s", address, value.c_str());
}

void publish_select_index(select::Select *entity, uint8_t index) {
  if (entity == nullptr || !entity->has_index(index)) {
    return;
  }
  const char *option = entity->option_at(index);
  if (option != nullptr) {
    entity->publish_state(option);
  }
}

class NamedSensor : public sensor::Sensor {
 public:
  void init(const char *name) { this->configure_entity_(name, 0, 0); }
};

class NamedBinarySensor : public binary_sensor::BinarySensor {
 public:
  void init(const char *name) { this->configure_entity_(name, 0, 0); }
};

class NamedTextSensor : public text_sensor::TextSensor {
 public:
  void init(const char *name) { this->configure_entity_(name, 0, 0); }
};

class NamedHyundaiBitSwitch : public HyundaiBitSwitch {
 public:
  void init(const char *name, uint32_t entity_fields = 0) { this->configure_entity_(name, 0, entity_fields); }
};

class NamedHyundaiNumber : public HyundaiNumber {
 public:
  void init(const char *name, uint32_t entity_fields = 0) { this->configure_entity_(name, 0, entity_fields); }
};

class NamedHyundaiSelect : public HyundaiSelect {
 public:
  void init(const char *name, uint32_t entity_fields = 0) { this->configure_entity_(name, 0, entity_fields); }
};

inline bool has_sensor_state(sensor::Sensor *item) {
  return item != nullptr && !std::isnan(item->state);
}

sensor::Sensor *make_sensor(const char *name) {
  auto *entity = new NamedSensor();
  entity->init(name);
  App.register_sensor(entity);
  return entity;
}

binary_sensor::BinarySensor *make_binary_sensor(const char *name) {
  auto *entity = new NamedBinarySensor();
  entity->init(name);
  App.register_binary_sensor(entity);
  return entity;
}

text_sensor::TextSensor *make_text_sensor(const char *name) {
  auto *entity = new NamedTextSensor();
  entity->init(name);
  App.register_text_sensor(entity);
  return entity;
}

}  // namespace

void HyundaiBitSwitch::write_state(bool state) {
  if (this->device_ == nullptr || this->cache_ == nullptr) {
    this->publish_state(state);
    return;
  }
  if (debug_log_messages) {
    ESP_LOGI(DEVICE_TAG, "Switch write register %u bitmask 0x%04X -> %s", this->reg_, this->bitmask_, ONOFF(state));
  }
  this->device_->set_bit_(this->reg_, *this->cache_, this->bitmask_, state);
  this->publish_state(state);
}

void HyundaiNumber::control(float value) {
  if (debug_log_messages) {
    ESP_LOGI(DEVICE_TAG, "Number write register %u -> %.2f", this->reg_, value);
  }
  if (this->write_fn_ != nullptr) {
    this->write_fn_(value);
  } else if (this->device_ != nullptr) {
    this->device_->write_register(this->reg_, static_cast<uint16_t>(std::lround(value)));
  }
  this->publish_state(value);
}

void HyundaiSelect::control(const std::string &value) {
  auto index = this->index_of(value);
  if (!index.has_value()) {
    return;
  }
  if (debug_log_messages) {
    ESP_LOGI(DEVICE_TAG, "Select write register %u -> %s", this->reg_, value.c_str());
  }
  if (this->write_fn_ != nullptr) {
    this->write_fn_(index.value());
  } else if (this->device_ != nullptr) {
    this->device_->write_register(this->reg_, static_cast<uint16_t>(index.value()));
  }
  this->publish_state(value);
}

void HyundaiHeatPumpDevice::setup() {
  auto make_bit_switch = [this](HyundaiBitSwitch *&target, const char *name, uint16_t reg, uint16_t *cache,
                                uint16_t bitmask) {
    auto *entity = new NamedHyundaiBitSwitch();
    entity->init(name, uint32_t(ENTITY_CATEGORY_CONFIG) << ENTITY_FIELD_ENTITY_CATEGORY_SHIFT);
    entity->set_device(this);
    entity->set_register(reg, cache, bitmask);
    entity->set_restore_mode(switch_::SWITCH_RESTORE_DISABLED);
    App.register_switch(entity);
    target = entity;
  };

  auto make_number = [this](HyundaiNumber *&target, const char *name, float min_value, float max_value, float step,
                            const HyundaiNumber::WriteFn &write_fn = nullptr) {
    auto *entity = new NamedHyundaiNumber();
    entity->init(name, uint32_t(ENTITY_CATEGORY_CONFIG) << ENTITY_FIELD_ENTITY_CATEGORY_SHIFT);
    entity->set_device(this);
    entity->traits.set_min_value(min_value);
    entity->traits.set_max_value(max_value);
    entity->traits.set_step(step);
    entity->traits.set_mode(number::NUMBER_MODE_SLIDER);
    entity->set_write_fn(write_fn);
    App.register_number(entity);
    target = entity;
  };

  auto make_select = [this](HyundaiSelect *&target, const char *name, const std::initializer_list<const char *> &options,
                            const HyundaiSelect::WriteFn &write_fn = nullptr) {
    auto *entity = new NamedHyundaiSelect();
    entity->init(name, uint32_t(ENTITY_CATEGORY_CONFIG) << ENTITY_FIELD_ENTITY_CATEGORY_SHIFT);
    entity->set_device(this);
    entity->traits.set_options(options);
    entity->set_write_fn(write_fn);
    App.register_select(entity);
    target = entity;
  };

  this->s_comp_freq_ = make_sensor("Compressor Operating Frequency");
  this->s_fan_speed_ = make_sensor("Fan Speed");
  this->s_water_inlet_temp_ = make_sensor("Water Inlet Temperature");
  this->s_water_outlet_temp_ = make_sensor("Water Outlet Temperature");
  this->s_outdoor_ambient_ = make_sensor("Outdoor Ambient Temperature");
  this->s_room_temp_ta_ = make_sensor("Room Temperature Ta");
  this->s_water_tank_t5_ = make_sensor("Water Tank Temperature T5");
  this->s_current_fault_ = make_sensor("Current Fault");
  this->s_fault_1_ = make_sensor("Fault 1");
  this->s_fault_2_ = make_sensor("Fault 2");
  this->s_fault_3_ = make_sensor("Fault 3");
  this->s_water_flow_ = make_sensor("Water Flow");
  this->s_elec_consumption_ = make_sensor("Electricity Consumption");
  this->s_power_output_ = make_sensor("Power Output");
  this->s_forced_ibh1_ = make_sensor("Forced Hydraulic Module Rear Electric Heater 1");
  this->s_t_sg_max_ = make_sensor("t_SG_MAX");
  this->s_cop_ = make_sensor("Coefficient of Performance");
  this->s_water_temp_delta_ = make_sensor("Water Temperature Delta");
  this->s_t1s_dhw_ = make_sensor("T1S DHW");
  this->s_comp_starts_per_hour_ = make_sensor("Compressor Starts Per Hour");
  this->s_active_state_map_ = make_sensor("Active State Map");

  this->bs_defrosting_ = make_binary_sensor("Status BIT 1 Defrosting");
  this->bs_oil_return_ = make_binary_sensor("Status BIT 1 Oil Return");
  this->bs_pump_o_ = make_binary_sensor("Load Output External Circulation Pump PUMP_O");
  this->bs_sv1_ = make_binary_sensor("Load Output SV 1");
  this->bs_run_ = make_binary_sensor("Load Output RUN");
  this->bs_compressor_running_ = make_binary_sensor("Compressor Running");
  this->bs_hp_running_ = make_binary_sensor("Heat pump running");
  this->bs_tbh_enabled_ = make_binary_sensor("Status TBH Enabled");
  this->bs_ahs_enabled_ = make_binary_sensor("Status AHS Enabled");
  this->bs_ibh_enabled_ = make_binary_sensor("Status IBH Enabled");
  this->bs_dhw_operation_ = make_binary_sensor("Status DHW Operation");
  this->bs_heat_operation_ = make_binary_sensor("Status Heating Operation");
  this->bs_cool_operation_ = make_binary_sensor("Status Cooling Operation");

  make_bit_switch(this->sw_room_temp_ctrl_, "ON/OFF Room Temperature Control", REG_SWITCHES_0, &this->cache_reg_0x00,
                  BIT_ROOM_TEMP_CTRL);
  make_bit_switch(this->sw_flow_ctrl_zone1_, "ON/OFF Water Flow Temperature Control Zone 1", REG_SWITCHES_0,
                  &this->cache_reg_0x00, BIT_FLOW_CTRL_ZONE1);
  make_bit_switch(this->sw_power_dhw_t5s_, "ON/OFF Power DHW T5S", REG_SWITCHES_0, &this->cache_reg_0x00,
                  BIT_POWER_DHW_T5S);
  make_bit_switch(this->sw_flow_ctrl_zone2_, "ON/OFF Water Flow Temperature Control Zone 2", REG_SWITCHES_0,
                  &this->cache_reg_0x00, BIT_FLOW_CTRL_ZONE2);
  make_bit_switch(this->sw_disinfect_, "Function Setting Disinfect", REG_FUNC_SETTINGS_5, &this->cache_reg_0x05,
                  BIT_DISINFECT);
  make_bit_switch(this->sw_silent_mode_, "Function Setting Silent Mode", REG_FUNC_SETTINGS_5, &this->cache_reg_0x05,
                  BIT_SILENT_MODE);
  make_bit_switch(this->sw_eco_mode_, "Function Setting ECO Mode", REG_FUNC_SETTINGS_5, &this->cache_reg_0x05,
                  BIT_ECO_MODE);
  make_bit_switch(this->sw_enswitchpdc_, "EnSwitchPDC", REG_ENSWITCHPDC, &this->cache_reg_0x112, 0x0001);

  make_number(this->n_t1s_zone1_, "Set Water Temperature T1S Zone 1", 5.0f, 60.0f, 1.0f,
              [this](float value) { this->set_low_byte_(REG_T1S_ZONES, this->cache_t1s_zones, static_cast<uint8_t>(value)); });
  make_number(this->n_t1s_zone2_, "Set Water Temperature T1S Zone 2", 5.0f, 60.0f, 1.0f,
              [this](float value) { this->set_high_byte_(REG_T1S_ZONES, this->cache_t1s_zones, static_cast<uint8_t>(value)); });
  make_number(this->n_air_temp_ts_, "Air Temperature Ts", 17.0f, 30.0f, 0.5f,
              [this](float value) { this->write_register(REG_AIR_TEMP_TS, static_cast<uint16_t>(std::lround(value * 2.0f))); });
  make_number(this->n_dhw_t5s_, "Set DHW Tank Temperature T5s", 20.0f, 65.0f, 1.0f,
              [this](float value) { this->write_register(REG_DHW_T5S, static_cast<uint16_t>(std::lround(value))); });
  make_number(this->n_wc_curve_zone1_, "Weather Compensation Curve Zone 1", 1.0f, 9.0f, 1.0f,
              [this](float value) { this->set_low_byte_(REG_WC_CURVE, this->cache_wc_curve, static_cast<uint8_t>(value)); });
  make_number(this->n_wc_curve_zone2_, "Weather Compensation Curve Zone 2", 1.0f, 9.0f, 1.0f,
              [this](float value) { this->set_high_byte_(REG_WC_CURVE, this->cache_wc_curve, static_cast<uint8_t>(value)); });
  make_number(this->n_dt1s5_, "dT1S5", 5.0f, 40.0f, 1.0f,
              [this](float value) { this->write_register(REG_DT1S5, static_cast<uint16_t>(std::lround(value))); });
  make_number(this->n_t_t4_fresh_h_, "t_T4 FRESH_H", 0.5f, 6.0f, 0.5f,
              [this](float value) {
                this->set_low_byte_(REG_T_T4_FRESH_HC, this->cache_reg_0x10E,
                                    static_cast<uint8_t>(std::lround(value * 2.0f)));
              });
  make_number(this->n_t_t4_fresh_c_, "t_T4 FRESH_C", 0.5f, 6.0f, 0.5f,
              [this](float value) {
                this->set_high_byte_(REG_T_T4_FRESH_HC, this->cache_reg_0x10E,
                                     static_cast<uint8_t>(std::lround(value * 2.0f)));
              });
  make_number(this->n_deltatsol_, "DELTATSOL Solar Temp Difference", 5.0f, 20.0f, 1.0f,
              [this](float value) {
                this->set_high_byte_(REG_SOLAR_DELTATSOL, this->cache_reg_0x111, static_cast<uint8_t>(std::lround(value)));
              });

  make_select(this->sel_operational_mode_, "Operational Mode", {"Heat", "Cool", "Auto"},
              [this](size_t index) {
                static const uint16_t values[] = {3, 2, 1};
                this->write_register(REG_OPERATIONAL_MODE, values[index]);
              });
  make_select(this->sel_water_flow_ctrl_, "Water Flow Temperature Control Select", {"Off", "On"},
              [this](size_t index) { this->write_register(REG_WATER_FLOW_CTRL, static_cast<uint16_t>(index)); });
  make_select(this->sel_room_temp_ctrl_, "Room Temperature Control Select", {"Off", "On"},
              [this](size_t index) { this->write_register(REG_ROOM_TEMP_CTRL, static_cast<uint16_t>(index)); });
  make_select(this->sel_power_input_limit_, "Power Input Limitation Type", {"None", "1", "2", "3", "4", "5", "6", "7", "8"},
              [this](size_t index) { this->write_register(REG_POWER_INPUT_LIMIT, static_cast<uint16_t>(index)); });
  make_select(this->sel_zone1_h_emission_, "Zone 1 End Heating Mode Emission Type", {"Underfloor Heating", "Fan Coil Unit", "Radiator"},
              [this](size_t index) { this->set_nibble_(REG_ZONE_EMISSION, this->cache_reg_0x110, index, 0); });
  make_select(this->sel_zone2_h_emission_, "Zone 2 End Heating Mode Emission Type", {"Underfloor Heating", "Fan Coil Unit", "Radiator"},
              [this](size_t index) { this->set_nibble_(REG_ZONE_EMISSION, this->cache_reg_0x110, index, 1); });
  make_select(this->sel_zone1_c_emission_, "Zone 1 End Cooling Mode Emission Type", {"Underfloor Heating", "Fan Coil Unit", "Radiator"},
              [this](size_t index) { this->set_nibble_(REG_ZONE_EMISSION, this->cache_reg_0x110, index, 2); });
  make_select(this->sel_zone2_c_emission_, "Zone 2 End Cooling Mode Emission Type", {"Underfloor Heating", "Fan Coil Unit", "Radiator"},
              [this](size_t index) { this->set_nibble_(REG_ZONE_EMISSION, this->cache_reg_0x110, index, 3); });
  make_select(this->sel_solar_function_, "Solar Function Mode", {"No Function", "Solar + Heat Pump", "Only Solar"},
              [this](size_t index) { this->set_low_byte_(REG_SOLAR_DELTATSOL, this->cache_reg_0x111, index); });

  this->ts_operating_mode_ = make_text_sensor("Operating Mode");
  this->ts_hp_operation_mode_ = make_text_sensor("Heat Pump Operation Mode");
  this->ts_active_state_ = make_text_sensor("Active State");
  this->ts_current_fault_code_ = make_text_sensor("Current Fault Error Code");
  this->ts_current_fault_desc_ = make_text_sensor("Current Fault Error Code Description");
  this->ts_fault_1_code_ = make_text_sensor("Fault 1 Error Code");
  this->ts_fault_2_code_ = make_text_sensor("Fault 2 Error Code");
  this->ts_fault_3_code_ = make_text_sensor("Fault 3 Error Code");
  this->ts_machine_type_ = make_text_sensor("MachineType");
  this->ts_hm_submodel_ = make_text_sensor("Hydraulic Module Sub-Model");
  this->ts_product_code_ = make_text_sensor("Home Appliance Product Code");

  this->parent_->set_interval("compressor-start-reset", 3600000, [this]() {
    this->compressor_start_count_ = 0;
    if (this->s_comp_starts_per_hour_ != nullptr) {
      this->s_comp_starts_per_hour_->publish_state(0);
    }
  });
}

void HyundaiHeatPumpDevice::publish_fast_data(const std::vector<uint8_t> &data, uint16_t start_addr) {
  if (start_addr == REG_SWITCHES_0) {
    log_range_read(REG_SWITCHES_0, REG_ROOM_TEMP_CTRL);

    this->cache_reg_0x00 = reg_u16(data, start_addr, REG_SWITCHES_0);
    this->cache_t1s_zones = reg_u16(data, start_addr, REG_T1S_ZONES);
    this->cache_reg_0x05 = reg_u16(data, start_addr, REG_FUNC_SETTINGS_5);
    this->cache_wc_curve = reg_u16(data, start_addr, REG_WC_CURVE);

    if (this->sw_room_temp_ctrl_ != nullptr) this->sw_room_temp_ctrl_->publish_state((this->cache_reg_0x00 & BIT_ROOM_TEMP_CTRL) != 0);
    if (this->sw_flow_ctrl_zone1_ != nullptr) this->sw_flow_ctrl_zone1_->publish_state((this->cache_reg_0x00 & BIT_FLOW_CTRL_ZONE1) != 0);
    if (this->sw_power_dhw_t5s_ != nullptr) this->sw_power_dhw_t5s_->publish_state((this->cache_reg_0x00 & BIT_POWER_DHW_T5S) != 0);
    if (this->sw_flow_ctrl_zone2_ != nullptr) this->sw_flow_ctrl_zone2_->publish_state((this->cache_reg_0x00 & BIT_FLOW_CTRL_ZONE2) != 0);
    if (this->sw_disinfect_ != nullptr) this->sw_disinfect_->publish_state((this->cache_reg_0x05 & BIT_DISINFECT) != 0);
    if (this->sw_silent_mode_ != nullptr) this->sw_silent_mode_->publish_state((this->cache_reg_0x05 & BIT_SILENT_MODE) != 0);
    if (this->sw_eco_mode_ != nullptr) this->sw_eco_mode_->publish_state((this->cache_reg_0x05 & BIT_ECO_MODE) != 0);

    if (this->sel_operational_mode_ != nullptr) {
      const char *state = operational_mode_select_to_string(reg_u16(data, start_addr, REG_OPERATIONAL_MODE));
      if (state != nullptr) {
        this->sel_operational_mode_->publish_state(state);
      }
    }
    if (this->n_t1s_zone1_ != nullptr) this->n_t1s_zone1_->publish_state(this->cache_t1s_zones & 0x00FF);
    if (this->n_t1s_zone2_ != nullptr) this->n_t1s_zone2_->publish_state((this->cache_t1s_zones >> 8) & 0x00FF);
    if (this->n_air_temp_ts_ != nullptr) this->n_air_temp_ts_->publish_state(reg_u16(data, start_addr, REG_AIR_TEMP_TS) * 0.5f);
    if (this->n_dhw_t5s_ != nullptr) this->n_dhw_t5s_->publish_state(reg_u16(data, start_addr, REG_DHW_T5S));
    if (this->n_wc_curve_zone1_ != nullptr) this->n_wc_curve_zone1_->publish_state(this->cache_wc_curve & 0x00FF);
    if (this->n_wc_curve_zone2_ != nullptr) this->n_wc_curve_zone2_->publish_state((this->cache_wc_curve >> 8) & 0x00FF);
    if (this->s_forced_ibh1_ != nullptr) this->s_forced_ibh1_->publish_state(reg_u16(data, start_addr, REG_FORCED_IBH1));
    if (this->s_t_sg_max_ != nullptr) this->s_t_sg_max_->publish_state(reg_u16(data, start_addr, REG_T_SG_MAX));
    if (this->sel_water_flow_ctrl_ != nullptr) this->sel_water_flow_ctrl_->publish_state(reg_u16(data, start_addr, REG_WATER_FLOW_CTRL) == 1 ? "On" : "Off");
    if (this->sel_room_temp_ctrl_ != nullptr) this->sel_room_temp_ctrl_->publish_state(reg_u16(data, start_addr, REG_ROOM_TEMP_CTRL) == 1 ? "On" : "Off");

    log_key_value(REG_AIR_TEMP_TS, reg_u16(data, start_addr, REG_AIR_TEMP_TS) * 0.5f, "C");
    log_key_value(REG_DHW_T5S, reg_u16(data, start_addr, REG_DHW_T5S), "C");
  }

  if (start_addr == REG_COMP_FREQ) {
    log_range_read(REG_COMP_FREQ, REG_PRODUCT_CODE);

    const uint16_t comp_freq = reg_u16(data, start_addr, REG_COMP_FREQ);
    const uint16_t fan_speed = reg_u16(data, start_addr, REG_FAN_SPEED);
    const uint16_t current_fault = reg_u16(data, start_addr, REG_CURRENT_FAULT);
    const uint16_t fault_1 = reg_u16(data, start_addr, REG_FAULT_1);
    const uint16_t fault_2 = reg_u16(data, start_addr, REG_FAULT_2);
    const uint16_t fault_3 = reg_u16(data, start_addr, REG_FAULT_3);
    const uint16_t status_bits = reg_u16(data, start_addr, REG_STATUS_BITS_1);
    const uint16_t load_output = reg_u16(data, start_addr, REG_LOAD_OUTPUT);
    const uint16_t config_status = reg_u16(data, start_addr, REG_CONFIG_STATUS);

    if (this->s_comp_freq_ != nullptr) this->s_comp_freq_->publish_state(comp_freq);
    if (this->s_fan_speed_ != nullptr) this->s_fan_speed_->publish_state(fan_speed);
    if (this->s_water_inlet_temp_ != nullptr) this->s_water_inlet_temp_->publish_state(reg_s16(data, start_addr, REG_WATER_INLET_TEMP));
    if (this->s_water_outlet_temp_ != nullptr) this->s_water_outlet_temp_->publish_state(reg_s16(data, start_addr, REG_WATER_OUTLET_TEMP));
    if (this->s_outdoor_ambient_ != nullptr) this->s_outdoor_ambient_->publish_state(reg_s16(data, start_addr, REG_OUTDOOR_AMBIENT));
    if (this->s_room_temp_ta_ != nullptr) this->s_room_temp_ta_->publish_state(reg_s16(data, start_addr, REG_ROOM_TEMP_TA));
    if (this->s_water_tank_t5_ != nullptr) this->s_water_tank_t5_->publish_state(reg_s16(data, start_addr, REG_WATER_TANK_T5));
    if (this->s_current_fault_ != nullptr) this->s_current_fault_->publish_state(current_fault);
    if (this->s_fault_1_ != nullptr) this->s_fault_1_->publish_state(fault_1);
    if (this->s_fault_2_ != nullptr) this->s_fault_2_->publish_state(fault_2);
    if (this->s_fault_3_ != nullptr) this->s_fault_3_->publish_state(fault_3);
    if (this->s_water_flow_ != nullptr) this->s_water_flow_->publish_state(reg_u16(data, start_addr, REG_WATER_FLOW) * 0.01f);

    this->electricity_consumption_ = reg_u32(data, start_addr, REG_ELEC_CONSUMPTION) * 0.01f;
    this->power_output_ = reg_u32(data, start_addr, REG_POWER_OUTPUT) * 0.01f;
    if (this->s_elec_consumption_ != nullptr) this->s_elec_consumption_->publish_state(this->electricity_consumption_);
    if (this->s_power_output_ != nullptr) this->s_power_output_->publish_state(this->power_output_);

    if (this->bs_defrosting_ != nullptr) this->bs_defrosting_->publish_state((status_bits & SBIT_DEFROSTING) != 0);
    if (this->bs_oil_return_ != nullptr) this->bs_oil_return_->publish_state((status_bits & SBIT_OIL_RETURN) != 0);
    if (this->bs_pump_o_ != nullptr) this->bs_pump_o_->publish_state((load_output & LBIT_PUMP_O) != 0);
    if (this->bs_sv1_ != nullptr) this->bs_sv1_->publish_state((load_output & LBIT_SV1) != 0);
    if (this->bs_run_ != nullptr) this->bs_run_->publish_state((load_output & LBIT_RUN) != 0);
    if (this->bs_tbh_enabled_ != nullptr) this->bs_tbh_enabled_->publish_state((config_status & CSBIT_TBH_ENABLED) != 0);
    if (this->bs_ahs_enabled_ != nullptr) this->bs_ahs_enabled_->publish_state((config_status & CSBIT_AHS_ENABLED) != 0);
    if (this->bs_ibh_enabled_ != nullptr) this->bs_ibh_enabled_->publish_state((config_status & CSBIT_IBH_ENABLED) != 0);
    if (this->bs_dhw_operation_ != nullptr) this->bs_dhw_operation_->publish_state((config_status & CSBIT_DHW_OPERATION) != 0);
    if (this->bs_heat_operation_ != nullptr) this->bs_heat_operation_->publish_state((config_status & CSBIT_HEAT_OPERATION) != 0);
    if (this->bs_cool_operation_ != nullptr) this->bs_cool_operation_->publish_state((config_status & CSBIT_COOL_OPERATION) != 0);

    if (this->ts_operating_mode_ != nullptr) this->ts_operating_mode_->publish_state(operating_mode_to_string(reg_u16(data, start_addr, REG_OPERATING_MODE)));
    if (this->ts_hp_operation_mode_ != nullptr) this->ts_hp_operation_mode_->publish_state(hp_operation_mode_to_string(reg_u16(data, start_addr, REG_HP_OPERATION_MODE)));
    if (this->ts_current_fault_code_ != nullptr) this->ts_current_fault_code_->publish_state(fault_to_string(current_fault));
    if (this->ts_current_fault_desc_ != nullptr) this->ts_current_fault_desc_->publish_state(fault_to_description(current_fault));
    if (this->ts_fault_1_code_ != nullptr) this->ts_fault_1_code_->publish_state(fault_to_string(fault_1));
    if (this->ts_fault_2_code_ != nullptr) this->ts_fault_2_code_->publish_state(fault_to_string(fault_2));
    if (this->ts_fault_3_code_ != nullptr) this->ts_fault_3_code_->publish_state(fault_to_string(fault_3));
    if (this->ts_machine_type_ != nullptr) this->ts_machine_type_->publish_state(machine_type_to_string(reg_u16(data, start_addr, REG_MACHINE_TYPE)));
    if (this->ts_hm_submodel_ != nullptr) this->ts_hm_submodel_->publish_state(hm_submodel_to_string(reg_u16(data, start_addr, REG_HM_SUBMODEL)));
    if (this->ts_product_code_ != nullptr) this->ts_product_code_->publish_state(product_code_to_string(reg_u16(data, start_addr, REG_PRODUCT_CODE)));

    log_key_value(REG_COMP_FREQ, comp_freq, "Hz");
    log_key_value(REG_FAN_SPEED, fan_speed, "r/min");
    log_key_value(REG_WATER_INLET_TEMP, reg_s16(data, start_addr, REG_WATER_INLET_TEMP), "C");
    log_key_value(REG_WATER_OUTLET_TEMP, reg_s16(data, start_addr, REG_WATER_OUTLET_TEMP), "C");
    log_key_text(REG_OPERATING_MODE, operating_mode_to_string(reg_u16(data, start_addr, REG_OPERATING_MODE)));
    if (current_fault != 0) {
      log_key_text(REG_CURRENT_FAULT, fault_to_string(current_fault));
    }
  }

  this->update_computed_sensors_();
}

void HyundaiHeatPumpDevice::publish_config_data(const std::vector<uint8_t> &data, uint16_t start_addr) {
  if (start_addr == REG_DHW_PUMP_RETURN_TIME) {
    log_range_read(REG_DHW_PUMP_RETURN_TIME, REG_DT1S5);
    if (this->n_dt1s5_ != nullptr) this->n_dt1s5_->publish_state(reg_u16(data, start_addr, REG_DT1S5));
    log_key_value(REG_DT1S5, reg_u16(data, start_addr, REG_DT1S5), "C");
  }

  if (start_addr == REG_POWER_INPUT_LIMIT) {
    log_range_read(REG_POWER_INPUT_LIMIT, REG_TA_ADJUSTMENT);
    this->cache_reg_0x10E = reg_u16(data, start_addr, REG_T_T4_FRESH_HC);
    this->cache_reg_0x110 = reg_u16(data, start_addr, REG_ZONE_EMISSION);
    this->cache_reg_0x111 = reg_u16(data, start_addr, REG_SOLAR_DELTATSOL);
    this->cache_reg_0x112 = reg_u16(data, start_addr, REG_ENSWITCHPDC);

    if (this->sel_power_input_limit_ != nullptr) {
      const uint16_t raw = reg_u16(data, start_addr, REG_POWER_INPUT_LIMIT);
      if (raw <= 8) {
        this->sel_power_input_limit_->publish_state(raw == 0 ? "None" : std::to_string(raw));
      }
    }
    publish_select_index(this->sel_zone1_h_emission_, this->cache_reg_0x110 & 0x000F);
    publish_select_index(this->sel_zone2_h_emission_, (this->cache_reg_0x110 >> 4) & 0x000F);
    publish_select_index(this->sel_zone1_c_emission_, (this->cache_reg_0x110 >> 8) & 0x000F);
    publish_select_index(this->sel_zone2_c_emission_, (this->cache_reg_0x110 >> 12) & 0x000F);
    if (this->sel_solar_function_ != nullptr) {
      publish_select_index(this->sel_solar_function_, this->cache_reg_0x111 & 0x00FF);
    }
    if (this->sw_enswitchpdc_ != nullptr) this->sw_enswitchpdc_->publish_state((this->cache_reg_0x112 & 0x0001) != 0);
    if (this->n_t_t4_fresh_h_ != nullptr) this->n_t_t4_fresh_h_->publish_state((this->cache_reg_0x10E & 0x00FF) * 0.5f);
    if (this->n_t_t4_fresh_c_ != nullptr) this->n_t_t4_fresh_c_->publish_state(((this->cache_reg_0x10E >> 8) & 0x00FF) * 0.5f);
    if (this->n_deltatsol_ != nullptr) this->n_deltatsol_->publish_state((this->cache_reg_0x111 >> 8) & 0x00FF);

    log_key_value(REG_POWER_INPUT_LIMIT, reg_u16(data, start_addr, REG_POWER_INPUT_LIMIT));
    log_key_value(REG_T_T4_FRESH_HC, (this->cache_reg_0x10E & 0x00FF) * 0.5f, "h (heat)");
    log_key_value(REG_SOLAR_DELTATSOL, (this->cache_reg_0x111 >> 8) & 0x00FF, "C");
  }

  this->update_computed_sensors_();
}

void HyundaiHeatPumpDevice::write_register(uint16_t addr, uint16_t val) { this->parent_->write_register(addr, val); }
void HyundaiHeatPumpDevice::set_bit_(uint16_t addr, uint16_t &cache, uint16_t bitmask, bool state) { this->parent_->set_bit_(addr, cache, bitmask, state); }
void HyundaiHeatPumpDevice::set_low_byte_(uint16_t addr, uint16_t &cache, uint8_t val) { this->parent_->set_low_byte_(addr, cache, val); }
void HyundaiHeatPumpDevice::set_high_byte_(uint16_t addr, uint16_t &cache, uint8_t val) { this->parent_->set_high_byte_(addr, cache, val); }
void HyundaiHeatPumpDevice::set_nibble_(uint16_t addr, uint16_t &cache, uint8_t val, uint8_t nibble_pos) { this->parent_->set_nibble_(addr, cache, val, nibble_pos); }

void HyundaiHeatPumpDevice::update_computed_sensors_() {
  const bool compressor_running = has_sensor_state(this->s_comp_freq_) && this->s_comp_freq_->state > 0.0f;
  if (this->bs_compressor_running_ != nullptr) {
    this->bs_compressor_running_->publish_state(compressor_running);
  }
  if (!this->last_compressor_running_ && compressor_running) {
    this->compressor_start_count_++;
  }
  this->last_compressor_running_ = compressor_running;

  const bool pump_out_running = this->bs_pump_o_ != nullptr && this->bs_pump_o_->state;
  const bool fan_running = has_sensor_state(this->s_fan_speed_) && this->s_fan_speed_->state > 0.0f;
  const bool hp_running = compressor_running || fan_running || pump_out_running;
  if (this->bs_hp_running_ != nullptr) {
    this->bs_hp_running_->publish_state(hp_running);
  }
  if (this->s_comp_starts_per_hour_ != nullptr) {
    this->s_comp_starts_per_hour_->publish_state(this->compressor_start_count_);
  }

  if (this->s_water_temp_delta_ != nullptr && has_sensor_state(this->s_water_inlet_temp_) && has_sensor_state(this->s_water_outlet_temp_)) {
    this->s_water_temp_delta_->publish_state(this->s_water_outlet_temp_->state - this->s_water_inlet_temp_->state);
  }
  if (this->s_t1s_dhw_ != nullptr && has_sensor_state(this->s_water_tank_t5_) && this->n_dt1s5_ != nullptr) {
    this->s_t1s_dhw_->publish_state(this->s_water_tank_t5_->state + this->n_dt1s5_->state);
  }
  if (this->s_cop_ != nullptr) {
    if (this->electricity_consumption_ > 0.0f) {
      this->s_cop_->publish_state(this->power_output_ / this->electricity_consumption_);
    } else {
      this->s_cop_->publish_state(NAN);
    }
  }

  const char *active_state = "Inactive";
  if (this->ts_operating_mode_ != nullptr && this->ts_operating_mode_->state != "OFF") {
    if (this->bs_run_ != nullptr && this->bs_run_->state) {
      if (this->bs_defrosting_ != nullptr && this->bs_defrosting_->state) {
        active_state = "Defrosting";
      } else if (this->bs_oil_return_ != nullptr && this->bs_oil_return_->state) {
        active_state = "Oil return";
      } else if (this->bs_sv1_ != nullptr && this->bs_sv1_->state) {
        active_state = "DHW";
      } else if (this->ts_operating_mode_->state == "Heating") {
        active_state = "Heating";
      } else if (this->ts_operating_mode_->state == "Cooling") {
        active_state = "Cooling";
      } else {
        active_state = "Idle";
      }
    } else {
      active_state = "Idle";
    }
  }
  if (this->ts_active_state_ != nullptr) {
    this->ts_active_state_->publish_state(active_state);
  }
  if (this->s_active_state_map_ != nullptr) {
    float state_value = 99.0f;
    if (strcmp(active_state, "Inactive") == 0) state_value = 0;
    else if (strcmp(active_state, "Heating") == 0) state_value = 1;
    else if (strcmp(active_state, "Cooling") == 0) state_value = 2;
    else if (strcmp(active_state, "DHW") == 0) state_value = 3;
    else if (strcmp(active_state, "Defrosting") == 0) state_value = 4;
    else if (strcmp(active_state, "Idle") == 0) state_value = 5;
    else if (strcmp(active_state, "Oil return") == 0) state_value = 6;
    this->s_active_state_map_->publish_state(state_value);
  }
}

}  // namespace hyundai_heatpump
}  // namespace esphome
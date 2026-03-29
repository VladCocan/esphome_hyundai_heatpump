#pragma once

#include <cmath>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/select/select.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "protocol.h"

namespace esphome {
namespace hyundai_heatpump {

class HyundaiHeatPump;
class HyundaiHeatPumpDevice;

class HyundaiBitSwitch : public switch_::Switch {
 public:
  void set_device(HyundaiHeatPumpDevice *device) { device_ = device; }
  void set_register(uint16_t reg, uint16_t *cache, uint16_t bitmask) {
    reg_ = reg;
    cache_ = cache;
    bitmask_ = bitmask;
  }

 protected:
  void write_state(bool state) override;

  HyundaiHeatPumpDevice *device_{nullptr};
  uint16_t reg_{0};
  uint16_t *cache_{nullptr};
  uint16_t bitmask_{0};
};

class HyundaiNumber : public number::Number {
 public:
  using WriteFn = std::function<void(float)>;

  void set_device(HyundaiHeatPumpDevice *device) { device_ = device; }
  void set_register(uint16_t reg) { reg_ = reg; }
  void set_write_fn(WriteFn fn) { write_fn_ = std::move(fn); }

 protected:
  void control(float value) override;

  HyundaiHeatPumpDevice *device_{nullptr};
  uint16_t reg_{0};
  WriteFn write_fn_{nullptr};
};

class HyundaiSelect : public select::Select {
 public:
  using WriteFn = std::function<void(size_t)>;

  void set_device(HyundaiHeatPumpDevice *device) { device_ = device; }
  void set_register(uint16_t reg) { reg_ = reg; }
  void set_write_fn(WriteFn fn) { write_fn_ = std::move(fn); }

 protected:
  void control(const std::string &value) override;

  HyundaiHeatPumpDevice *device_{nullptr};
  uint16_t reg_{0};
  WriteFn write_fn_{nullptr};
};

class HyundaiHeatPumpDevice {
 public:
  explicit HyundaiHeatPumpDevice(HyundaiHeatPump *parent) : parent_(parent) {}

  void setup();
  void publish_fast_data(const std::vector<uint8_t> &data, uint16_t start_addr);
  void publish_config_data(const std::vector<uint8_t> &data, uint16_t start_addr);

  void write_register(uint16_t addr, uint16_t val);
  void set_bit_(uint16_t addr, uint16_t &cache, uint16_t bitmask, bool state);
  void set_low_byte_(uint16_t addr, uint16_t &cache, uint8_t val);
  void set_high_byte_(uint16_t addr, uint16_t &cache, uint8_t val);
  void set_nibble_(uint16_t addr, uint16_t &cache, uint8_t val, uint8_t nibble_pos);

 protected:
  void update_computed_sensors_();

  HyundaiHeatPump *parent_{nullptr};

  uint16_t cache_reg_0x00{0};
  uint16_t cache_reg_0x05{0};
  uint16_t cache_reg_0x110{0};
  uint16_t cache_reg_0x111{0};
  uint16_t cache_reg_0x112{0};
  uint16_t cache_t1s_zones{0};
  uint16_t cache_wc_curve{0};
  uint16_t cache_reg_0x10E{0};

  bool last_compressor_running_{false};
  int compressor_start_count_{0};
  float electricity_consumption_{0.0f};
  float power_output_{0.0f};

  sensor::Sensor *s_comp_freq_{nullptr};
  sensor::Sensor *s_fan_speed_{nullptr};
  sensor::Sensor *s_water_inlet_temp_{nullptr};
  sensor::Sensor *s_water_outlet_temp_{nullptr};
  sensor::Sensor *s_outdoor_ambient_{nullptr};
  sensor::Sensor *s_room_temp_ta_{nullptr};
  sensor::Sensor *s_water_tank_t5_{nullptr};
  sensor::Sensor *s_current_fault_{nullptr};
  sensor::Sensor *s_fault_1_{nullptr};
  sensor::Sensor *s_fault_2_{nullptr};
  sensor::Sensor *s_fault_3_{nullptr};
  sensor::Sensor *s_water_flow_{nullptr};
  sensor::Sensor *s_elec_consumption_{nullptr};
  sensor::Sensor *s_power_output_{nullptr};
  sensor::Sensor *s_forced_ibh1_{nullptr};
  sensor::Sensor *s_t_sg_max_{nullptr};
  sensor::Sensor *s_cop_{nullptr};
  sensor::Sensor *s_water_temp_delta_{nullptr};
  sensor::Sensor *s_t1s_dhw_{nullptr};
  sensor::Sensor *s_comp_starts_per_hour_{nullptr};
  sensor::Sensor *s_active_state_map_{nullptr};

  binary_sensor::BinarySensor *bs_defrosting_{nullptr};
  binary_sensor::BinarySensor *bs_oil_return_{nullptr};
  binary_sensor::BinarySensor *bs_pump_o_{nullptr};
  binary_sensor::BinarySensor *bs_sv1_{nullptr};
  binary_sensor::BinarySensor *bs_run_{nullptr};
  binary_sensor::BinarySensor *bs_compressor_running_{nullptr};
  binary_sensor::BinarySensor *bs_hp_running_{nullptr};
  binary_sensor::BinarySensor *bs_tbh_enabled_{nullptr};
  binary_sensor::BinarySensor *bs_ahs_enabled_{nullptr};
  binary_sensor::BinarySensor *bs_ibh_enabled_{nullptr};
  binary_sensor::BinarySensor *bs_dhw_operation_{nullptr};
  binary_sensor::BinarySensor *bs_heat_operation_{nullptr};
  binary_sensor::BinarySensor *bs_cool_operation_{nullptr};

  HyundaiBitSwitch *sw_room_temp_ctrl_{nullptr};
  HyundaiBitSwitch *sw_flow_ctrl_zone1_{nullptr};
  HyundaiBitSwitch *sw_power_dhw_t5s_{nullptr};
  HyundaiBitSwitch *sw_flow_ctrl_zone2_{nullptr};
  HyundaiBitSwitch *sw_disinfect_{nullptr};
  HyundaiBitSwitch *sw_silent_mode_{nullptr};
  HyundaiBitSwitch *sw_eco_mode_{nullptr};
  HyundaiBitSwitch *sw_enswitchpdc_{nullptr};

  HyundaiNumber *n_t1s_zone1_{nullptr};
  HyundaiNumber *n_t1s_zone2_{nullptr};
  HyundaiNumber *n_air_temp_ts_{nullptr};
  HyundaiNumber *n_dhw_t5s_{nullptr};
  HyundaiNumber *n_wc_curve_zone1_{nullptr};
  HyundaiNumber *n_wc_curve_zone2_{nullptr};
  HyundaiNumber *n_dt1s5_{nullptr};
  HyundaiNumber *n_t_t4_fresh_h_{nullptr};
  HyundaiNumber *n_t_t4_fresh_c_{nullptr};
  HyundaiNumber *n_deltatsol_{nullptr};

  HyundaiSelect *sel_operational_mode_{nullptr};
  HyundaiSelect *sel_water_flow_ctrl_{nullptr};
  HyundaiSelect *sel_room_temp_ctrl_{nullptr};
  HyundaiSelect *sel_power_input_limit_{nullptr};
  HyundaiSelect *sel_zone1_h_emission_{nullptr};
  HyundaiSelect *sel_zone2_h_emission_{nullptr};
  HyundaiSelect *sel_zone1_c_emission_{nullptr};
  HyundaiSelect *sel_zone2_c_emission_{nullptr};
  HyundaiSelect *sel_solar_function_{nullptr};

  text_sensor::TextSensor *ts_operating_mode_{nullptr};
  text_sensor::TextSensor *ts_hp_operation_mode_{nullptr};
  text_sensor::TextSensor *ts_active_state_{nullptr};
  text_sensor::TextSensor *ts_current_fault_code_{nullptr};
  text_sensor::TextSensor *ts_current_fault_desc_{nullptr};
  text_sensor::TextSensor *ts_fault_1_code_{nullptr};
  text_sensor::TextSensor *ts_fault_2_code_{nullptr};
  text_sensor::TextSensor *ts_fault_3_code_{nullptr};
  text_sensor::TextSensor *ts_machine_type_{nullptr};
  text_sensor::TextSensor *ts_hm_submodel_{nullptr};
  text_sensor::TextSensor *ts_product_code_{nullptr};
};

}  // namespace hyundai_heatpump
}  // namespace esphome
#include "hyundai_heatpump.h"

#include "esphome/core/log.h"

#include "hyundai_heatpump_device.h"
#include "protocol.h"

#include "protocol.cpp"
#include "hyundai_heatpump_device.cpp"

namespace esphome {
namespace hyundai_heatpump {

using namespace protocol;

static const char *const TAG = "hyundai_heatpump";

namespace {

void log_register_summary(const char *tag, uint16_t address) {
  const char *name = register_name(address);
  const char *remark = register_remark(address);
  if (name != nullptr && remark != nullptr) {
    ESP_LOGCONFIG(tag, "    %u: %s (%s)", address, name, remark);
    return;
  }
  if (name != nullptr) {
    ESP_LOGCONFIG(tag, "    %u: %s", address, name);
    return;
  }
  ESP_LOGCONFIG(tag, "    %u", address);
}

}  // namespace

HyundaiHeatPump::HyundaiHeatPump() {
  this->set_command_throttle(100);
  this->set_allow_duplicate_commands(false);
}

void HyundaiHeatPump::setup() {
  if (this->device_ == nullptr) {
    this->device_ = new HyundaiHeatPumpDevice(this);
  }
  this->device_->setup();
}

void HyundaiHeatPump::update() {
  this->schedule_fast_read_();
  if ((this->slow_update_counter_++ % 6U) == 0U) {
    this->schedule_config_read_();
  }
}

void HyundaiHeatPump::dump_config() {
  ESP_LOGCONFIG(TAG, "Hyundai Heat Pump Modbus Component");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "  Command throttle: %u ms", this->command_throttle_);
  ESP_LOGCONFIG(TAG, "  Fast polling ranges:");
  log_register_summary(TAG, REG_SWITCHES_0);
  log_register_summary(TAG, REG_ROOM_TEMP_CTRL);
  log_register_summary(TAG, REG_COMP_FREQ);
  log_register_summary(TAG, REG_PRODUCT_CODE);
  ESP_LOGCONFIG(TAG, "  Slow polling ranges:");
  log_register_summary(TAG, REG_DHW_PUMP_RETURN_TIME);
  log_register_summary(TAG, REG_DT1S5);
  log_register_summary(TAG, REG_POWER_INPUT_LIMIT);
  log_register_summary(TAG, REG_TA_ADJUSTMENT);
}

void HyundaiHeatPump::write_register(uint16_t addr, uint16_t val) {
  const char *name = register_name(addr);
  const char *remark = register_remark(addr);
  if (name != nullptr && remark != nullptr) {
    ESP_LOGD(TAG, "Writing register %u (%s) = 0x%04X (%u) [%s]", addr, name, val, val, remark);
  } else if (name != nullptr) {
    ESP_LOGD(TAG, "Writing register %u (%s) = 0x%04X (%u)", addr, name, val, val);
  } else {
    ESP_LOGD(TAG, "Writing register %u = 0x%04X (%u)", addr, val, val);
  }
  auto command = modbus_controller::ModbusCommandItem::create_write_single_command(this, addr, val);
  this->queue_command(command);
}

void HyundaiHeatPump::set_bit_(uint16_t addr, uint16_t &cache, uint16_t bitmask, bool state) {
  if (state) {
    cache |= bitmask;
  } else {
    cache &= ~bitmask;
  }
  this->write_register(addr, cache);
}

void HyundaiHeatPump::set_low_byte_(uint16_t addr, uint16_t &cache, uint8_t val) {
  cache &= 0xFF00;
  cache |= val;
  this->write_register(addr, cache);
}

void HyundaiHeatPump::set_high_byte_(uint16_t addr, uint16_t &cache, uint8_t val) {
  cache &= 0x00FF;
  cache |= static_cast<uint16_t>(val) << 8;
  this->write_register(addr, cache);
}

void HyundaiHeatPump::set_nibble_(uint16_t addr, uint16_t &cache, uint8_t val, uint8_t nibble_pos) {
  const uint8_t shift = nibble_pos * 4;
  cache &= ~(static_cast<uint16_t>(0x000F) << shift);
  cache |= (static_cast<uint16_t>(val & 0x0F) << shift);
  this->write_register(addr, cache);
}

void HyundaiHeatPump::schedule_fast_read_() {
  this->queue_command(modbus_controller::ModbusCommandItem::create_read_command(
      this, modbus::ModbusRegisterType::HOLDING, REG_SWITCHES_0, REG_ROOM_TEMP_CTRL - REG_SWITCHES_0 + 1,
      [this](modbus::ModbusRegisterType, uint16_t start_address, const std::vector<uint8_t> &data) {
        if (this->device_ != nullptr) {
          this->device_->publish_fast_data(data, start_address);
        }
      }));

  this->queue_command(modbus_controller::ModbusCommandItem::create_read_command(
      this, modbus::ModbusRegisterType::HOLDING, REG_COMP_FREQ, REG_PRODUCT_CODE - REG_COMP_FREQ + 1,
      [this](modbus::ModbusRegisterType, uint16_t start_address, const std::vector<uint8_t> &data) {
        if (this->device_ != nullptr) {
          this->device_->publish_fast_data(data, start_address);
        }
      }));
}

void HyundaiHeatPump::schedule_config_read_() {
  this->queue_command(modbus_controller::ModbusCommandItem::create_read_command(
      this, modbus::ModbusRegisterType::HOLDING, REG_DHW_PUMP_RETURN_TIME, REG_DT1S5 - REG_DHW_PUMP_RETURN_TIME + 1,
      [this](modbus::ModbusRegisterType, uint16_t start_address, const std::vector<uint8_t> &data) {
        if (this->device_ != nullptr) {
          this->device_->publish_config_data(data, start_address);
        }
      }));

  this->queue_command(modbus_controller::ModbusCommandItem::create_read_command(
      this, modbus::ModbusRegisterType::HOLDING, REG_POWER_INPUT_LIMIT, REG_TA_ADJUSTMENT - REG_POWER_INPUT_LIMIT + 1,
      [this](modbus::ModbusRegisterType, uint16_t start_address, const std::vector<uint8_t> &data) {
        if (this->device_ != nullptr) {
          this->device_->publish_config_data(data, start_address);
        }
      }));
}

}  // namespace hyundai_heatpump
}  // namespace esphome
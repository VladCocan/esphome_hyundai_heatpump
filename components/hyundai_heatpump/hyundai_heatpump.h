#pragma once

#include <cstdint>

#include "esphome/components/modbus_controller/modbus_controller.h"
#include "esphome/core/component.h"

namespace esphome {
namespace hyundai_heatpump {

class HyundaiHeatPumpDevice;

class HyundaiHeatPump : public modbus_controller::ModbusController {
 public:
  HyundaiHeatPump();

  void setup() override;
  void update() override;
  void dump_config() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

  void write_register(uint16_t addr, uint16_t val);

 protected:
  friend class HyundaiHeatPumpDevice;

  void set_bit_(uint16_t addr, uint16_t &cache, uint16_t bitmask, bool state);
  void set_low_byte_(uint16_t addr, uint16_t &cache, uint8_t val);
  void set_high_byte_(uint16_t addr, uint16_t &cache, uint8_t val);
  void set_nibble_(uint16_t addr, uint16_t &cache, uint8_t val, uint8_t nibble_pos);

  void schedule_fast_read_();
  void schedule_config_read_();

  HyundaiHeatPumpDevice *device_{nullptr};
  uint32_t slow_update_counter_{0};
};

}  // namespace hyundai_heatpump
}  // namespace esphome
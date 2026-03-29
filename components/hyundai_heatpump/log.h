#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "esphome/core/log.h"

namespace esphome {
namespace hyundai_heatpump {

extern bool debug_log_messages;
extern bool debug_log_raw_bytes;
extern bool debug_log_messages_on_change;

uint32_t log_dedup_key(uint16_t id);
uint32_t log_dedup_key(uint16_t id, const std::string &text);
bool log_should_print(uint32_t key, double value, double epsilon = 0.0f, uint32_t min_interval_ms = 0);
bool log_should_print(uint32_t key, const std::string &value, uint32_t min_interval_ms = 0);
void log_raw_bytes(const char *tag, const char *prefix, const std::vector<uint8_t> &data);

}  // namespace hyundai_heatpump
}  // namespace esphome
#include "log.h"

#include <cmath>
#include <map>

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace hyundai_heatpump {

bool debug_log_messages = false;
bool debug_log_raw_bytes = false;
bool debug_log_messages_on_change = false;

namespace {

struct DedupEntry {
  bool has_numeric{false};
  bool has_text{false};
  double numeric_value{0.0f};
  std::string text_value;
  uint32_t last_log_ms{0};
};

static std::map<uint32_t, DedupEntry> s_dedup;
static constexpr size_t MAX_DEDUP_ENTRIES = 256;

uint32_t fnv1a_32(const char *data, size_t len) {
  uint32_t hash = 2166136261UL;
  for (size_t i = 0; i < len; i++) {
    hash ^= static_cast<uint8_t>(data[i]);
    hash *= 16777619UL;
  }
  return hash;
}

uint32_t fnv1a_32(const std::string &value) { return fnv1a_32(value.data(), value.size()); }

void evict_if_needed() {
  if (s_dedup.size() <= MAX_DEDUP_ENTRIES) {
    return;
  }
  s_dedup.erase(s_dedup.begin());
}

}  // namespace

uint32_t log_dedup_key(uint16_t id) { return 0xA5A50000UL ^ static_cast<uint32_t>(id); }

uint32_t log_dedup_key(uint16_t id, const std::string &text) {
  return log_dedup_key(id) ^ fnv1a_32(text);
}

bool log_should_print(uint32_t key, double value, double epsilon, uint32_t min_interval_ms) {
  auto &entry = s_dedup[key];
  const uint32_t now = millis();

  if (!entry.has_numeric) {
    entry.has_numeric = true;
    entry.numeric_value = value;
    entry.last_log_ms = now;
    evict_if_needed();
    return true;
  }

  const bool changed = std::fabs(entry.numeric_value - value) > epsilon;
  const bool interval_elapsed = min_interval_ms > 0 && (now - entry.last_log_ms) >= min_interval_ms;
  if (!changed && !interval_elapsed) {
    return false;
  }

  entry.numeric_value = value;
  entry.last_log_ms = now;
  evict_if_needed();
  return true;
}

bool log_should_print(uint32_t key, const std::string &value, uint32_t min_interval_ms) {
  auto &entry = s_dedup[key];
  const uint32_t now = millis();

  if (!entry.has_text) {
    entry.has_text = true;
    entry.text_value = value;
    entry.last_log_ms = now;
    evict_if_needed();
    return true;
  }

  const bool changed = entry.text_value != value;
  const bool interval_elapsed = min_interval_ms > 0 && (now - entry.last_log_ms) >= min_interval_ms;
  if (!changed && !interval_elapsed) {
    return false;
  }

  entry.text_value = value;
  entry.last_log_ms = now;
  evict_if_needed();
  return true;
}

void log_raw_bytes(const char *tag, const char *prefix, const std::vector<uint8_t> &data) {
  if (!debug_log_raw_bytes) {
    return;
  }

  const size_t buffer_size = format_hex_pretty_size(data.size());
  std::string buffer(buffer_size == 0 ? 1 : buffer_size, '\0');
  format_hex_pretty_to(buffer.data(), buffer.size(), data.data(), data.size(), ' ');
  ESP_LOGD(tag, "%s%s", prefix, buffer.c_str());
}

}  // namespace hyundai_heatpump
}  // namespace esphome
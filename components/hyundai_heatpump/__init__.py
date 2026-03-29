from esphome import core
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import modbus, modbus_controller, uart
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
    CONF_UPDATE_INTERVAL,
)
from esphome.core import CORE
from esphome.core.entity_helpers import (
    register_device_class,
    register_icon,
    register_unit_of_measurement,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = [
    "modbus",
    "sensor",
    "binary_sensor",
    "switch",
    "number",
    "select",
    "text_sensor",
]

CONF_INTERNAL_MODBUS_ID = "internal_modbus_id"
CONF_MODBUS_ID = modbus.CONF_MODBUS_ID
CONF_DEBUG_LOG_MESSAGES = "debug_log_messages"
CONF_DEBUG_LOG_MESSAGES_RAW = "debug_log_messages_raw"
CONF_DEBUG_LOG_MESSAGES_ON_CHANGE = "debug_log_messages_on_change"

ENTITY_PLATFORM_COUNTS = {
    "sensor": 21,
    "binary_sensor": 13,
    "switch": 8,
    "number": 10,
    "select": 9,
    "text_sensor": 11,
}

ENTITY_METADATA_DEFINES = {
    "HYUNDAI_DC_TEMPERATURE": register_device_class("temperature"),
    "HYUNDAI_DC_TEMPERATURE_DELTA": register_device_class("temperature_delta"),
    "HYUNDAI_DC_POWER": register_device_class("power"),
    "HYUNDAI_DC_FREQUENCY": register_device_class("frequency"),
    "HYUNDAI_DC_DURATION": register_device_class("duration"),
    "HYUNDAI_UOM_C": register_unit_of_measurement("°C"),
    "HYUNDAI_UOM_HZ": register_unit_of_measurement("Hz"),
    "HYUNDAI_UOM_RPM": register_unit_of_measurement("r/min"),
    "HYUNDAI_UOM_KW": register_unit_of_measurement("kW"),
    "HYUNDAI_UOM_H": register_unit_of_measurement("h"),
    "HYUNDAI_UOM_PER_H": register_unit_of_measurement("/h"),
    "HYUNDAI_ICON_HVAC": register_icon("mdi:hvac"),
    "HYUNDAI_ICON_SOLAR": register_icon("mdi:solar-power"),
    "HYUNDAI_ICON_GAUGE": register_icon("mdi:gauge"),
    "HYUNDAI_ICON_RADIATOR": register_icon("mdi:radiator"),
    "HYUNDAI_ICON_THERMOMETER": register_icon("mdi:thermometer"),
    "HYUNDAI_ICON_THERMOMETER_LINES": register_icon("mdi:thermometer-lines"),
    "HYUNDAI_ICON_WATER_THERMOMETER": register_icon("mdi:water-thermometer"),
    "HYUNDAI_ICON_TUNE": register_icon("mdi:tune-variant"),
    "HYUNDAI_ICON_TIMER": register_icon("mdi:timer-outline"),
    "HYUNDAI_ICON_PUMP": register_icon("mdi:pump"),
    "HYUNDAI_ICON_VALVE": register_icon("mdi:valve"),
    "HYUNDAI_ICON_SNOWFLAKE": register_icon("mdi:snowflake"),
    "HYUNDAI_ICON_LEAF": register_icon("mdi:leaf"),
    "HYUNDAI_ICON_VOLUME_OFF": register_icon("mdi:volume-off"),
    "HYUNDAI_ICON_POWER_PLUG": register_icon("mdi:power-plug"),
    "HYUNDAI_ICON_SHIELD_CHECK": register_icon("mdi:shield-check"),
}

hyundai_heatpump_ns = cg.esphome_ns.namespace("hyundai_heatpump")
HyundaiHeatPump = hyundai_heatpump_ns.class_(
    "HyundaiHeatPump",
    modbus_controller.ModbusController,
)

def _inject_modbus_reference(config):
    declared_internal_id = config[CONF_INTERNAL_MODBUS_ID]
    if declared_internal_id.id is None:
        declared_internal_id.id = f"{config[CONF_ID].id}_modbus"
        declared_internal_id.is_manual = True

    internal_id = declared_internal_id.copy()
    internal_id.is_declaration = False
    config[CONF_MODBUS_ID] = internal_id
    return config


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HyundaiHeatPump),
            cv.GenerateID(CONF_INTERNAL_MODBUS_ID): cv.declare_id(modbus.Modbus),
            cv.Optional(CONF_UPDATE_INTERVAL, default="10s"): cv.update_interval,
            cv.Optional(CONF_DEBUG_LOG_MESSAGES, default=False): cv.boolean,
            cv.Optional(CONF_DEBUG_LOG_MESSAGES_RAW, default=False): cv.boolean,
            cv.Optional(CONF_DEBUG_LOG_MESSAGES_ON_CHANGE, default=False): cv.boolean,
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.Schema({cv.Optional(CONF_ADDRESS, default=0x01): cv.hex_uint8_t})),
    _inject_modbus_reference,
)


async def to_code(config):
    parent = cg.new_Pvariable(config[CONF_INTERNAL_MODBUS_ID])
    await cg.register_component(parent, {})
    await uart.register_uart_device(parent, config)
    cg.add(parent.set_role(modbus.MODBUS_ROLES["client"]))
    cg.add(parent.set_send_wait_time(cv.positive_time_period_milliseconds("250ms")))
    cg.add(parent.set_turnaround_time(cv.positive_time_period_milliseconds("100ms")))
    cg.add(parent.set_disable_crc(False))

    var = cg.new_Pvariable(config[CONF_ID])
    for platform_name, count in ENTITY_PLATFORM_COUNTS.items():
        CORE.platform_counts[platform_name] += count
    cg.add_define("USE_ENTITY_DEVICE_CLASS")
    cg.add_define("USE_ENTITY_ICON")
    cg.add_define("USE_ENTITY_UNIT_OF_MEASUREMENT")
    for key, value in ENTITY_METADATA_DEFINES.items():
        cg.add_define(key, value)
    await cg.register_component(var, config)
    cg.add(var.set_debug_log_messages(config[CONF_DEBUG_LOG_MESSAGES]))
    cg.add(var.set_debug_log_messages_raw(config[CONF_DEBUG_LOG_MESSAGES_RAW]))
    cg.add(var.set_debug_log_messages_on_change(config[CONF_DEBUG_LOG_MESSAGES_ON_CHANGE]))
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(parent.register_device(var))

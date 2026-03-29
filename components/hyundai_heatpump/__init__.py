from esphome import core
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import modbus, modbus_controller, uart
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
    CONF_UPDATE_INTERVAL,
)
from esphome.core import CORE, Define

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

ENTITY_COUNT_DEFINES = {
    "ESPHOME_ENTITY_SENSOR_COUNT": 23,
    "ESPHOME_ENTITY_BINARY_SENSOR_COUNT": 13,
    "ESPHOME_ENTITY_SWITCH_COUNT": 8,
    "ESPHOME_ENTITY_NUMBER_COUNT": 10,
    "ESPHOME_ENTITY_SELECT_COUNT": 9,
    "ESPHOME_ENTITY_TEXT_SENSOR_COUNT": 11,
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
    for key, value in ENTITY_COUNT_DEFINES.items():
        CORE.defines = {define for define in CORE.defines if define.name != key}
        CORE.defines.add(Define(key, value))
    cg.add_define("USE_SENSOR")
    cg.add_define("USE_BINARY_SENSOR")
    cg.add_define("USE_SWITCH")
    cg.add_define("USE_NUMBER")
    cg.add_define("USE_SELECT")
    cg.add_define("USE_TEXT_SENSOR")
    await cg.register_component(var, config)
    cg.add(var.set_debug_log_messages(config[CONF_DEBUG_LOG_MESSAGES]))
    cg.add(var.set_debug_log_messages_raw(config[CONF_DEBUG_LOG_MESSAGES_RAW]))
    cg.add(var.set_debug_log_messages_on_change(config[CONF_DEBUG_LOG_MESSAGES_ON_CHANGE]))
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(parent.register_device(var))

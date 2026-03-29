import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import modbus, modbus_controller
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
    CONF_UPDATE_INTERVAL,
)

DEPENDENCIES = ["modbus"]
AUTO_LOAD = [
    "sensor",
    "binary_sensor",
    "switch",
    "number",
    "select",
    "text_sensor",
    "modbus_controller",
]

CONF_MODBUS_ID = modbus.CONF_MODBUS_ID

hyundai_heatpump_ns = cg.esphome_ns.namespace("hyundai_heatpump")
HyundaiHeatPump = hyundai_heatpump_ns.class_(
    "HyundaiHeatPump",
    modbus_controller.ModbusController,
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HyundaiHeatPump),
            cv.Optional(CONF_UPDATE_INTERVAL, default="10s"): cv.update_interval,
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(modbus.modbus_device_schema(0x01))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add_define("USE_SENSOR")
    cg.add_define("USE_BINARY_SENSOR")
    cg.add_define("USE_SWITCH")
    cg.add_define("USE_NUMBER")
    cg.add_define("USE_SELECT")
    cg.add_define("USE_TEXT_SENSOR")
    await cg.register_component(var, config)
    await modbus.register_modbus_device(var, config)

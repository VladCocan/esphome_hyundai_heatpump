#include "protocol.h"

namespace esphome {
namespace hyundai_heatpump {
namespace protocol {

uint16_t reg_u16(const std::vector<uint8_t> &data, uint16_t start_addr, uint16_t reg) {
  const size_t offset = static_cast<size_t>(reg - start_addr) * 2;
  if (offset + 1 >= data.size()) {
    return 0;
  }
  return modbus_controller::get_data<uint16_t>(data, offset);
}

int16_t reg_s16(const std::vector<uint8_t> &data, uint16_t start_addr, uint16_t reg) {
  const size_t offset = static_cast<size_t>(reg - start_addr) * 2;
  if (offset + 1 >= data.size()) {
    return 0;
  }
  return modbus_controller::get_data<int16_t>(data, offset);
}

uint32_t reg_u32(const std::vector<uint8_t> &data, uint16_t start_addr, uint16_t reg) {
  const size_t offset = static_cast<size_t>(reg - start_addr) * 2;
  if (offset + 3 >= data.size()) {
    return 0;
  }
  return modbus_controller::get_data<uint32_t>(data, offset);
}

const char *register_name(uint16_t address) {
  switch (address) {
    case REG_SWITCHES_0: return "Power on/off";
    case REG_OPERATIONAL_MODE: return "Mode setting";
    case REG_T1S_ZONES: return "Set water temperature T1S / T1S2";
    case REG_AIR_TEMP_TS: return "Set air temperature TS";
    case REG_DHW_T5S: return "T5S";
    case REG_FUNC_SETTINGS_5: return "Function setting";
    case REG_WC_CURVE: return "Curve selection";
    case REG_FORCED_DHW: return "Forced water heating";
    case REG_FORCED_TBH: return "Forced TBH";
    case REG_FORCED_IBH1: return "Forced IBH1";
    case REG_T_SG_MAX: return "t_SG_MAX";
    case REG_COMP_FREQ: return "Operating frequency";
    case REG_OPERATING_MODE: return "Operating mode";
    case REG_FAN_SPEED: return "Fan speed";
    case REG_PMV_OPENNESS: return "PMV openness";
    case REG_WATER_INLET_TEMP: return "Inlet water temperature";
    case REG_WATER_OUTLET_TEMP: return "Outlet water temperature";
    case REG_COND_TEMP_T3: return "T3 Temperature";
    case REG_OUTDOOR_AMBIENT: return "T4 Temperature";
    case REG_DISCHARGE_TEMP: return "Discharge temperature";
    case REG_RETURN_AIR_TEMP: return "Return air temperature";
    case REG_TOTAL_WATER_T1: return "T1";
    case REG_SYSTEM_WATER_T1B: return "T1B";
    case REG_REFRIG_LIQUID_T2: return "T2";
    case REG_REFRIG_GAS_T2B: return "T2B";
    case REG_ROOM_TEMP_TA: return "Ta";
    case REG_WATER_TANK_T5: return "T5";
    case REG_HIGH_PRESSURE: return "Pressure 1";
    case REG_LOW_PRESSURE: return "Pressure 2";
    case REG_OUTDOOR_CURRENT: return "ODU current";
    case REG_OUTDOOR_VOLTAGE: return "ODU voltage";
    case REG_TBT1: return "Tbt1";
    case REG_TBT2: return "Tbt2";
    case REG_COMP_OP_TIME: return "Compressor operation time";
    case REG_UNIT_CAPACITY: return "Unit capacity";
    case REG_CURRENT_FAULT: return "Current fault";
    case REG_FAULT_1: return "Fault 1";
    case REG_FAULT_2: return "Fault 2";
    case REG_FAULT_3: return "Fault 3";
    case REG_STATUS_BITS_1: return "Status bit 1";
    case REG_LOAD_OUTPUT: return "Load output";
    case REG_SW_VERSION: return "Software version";
    case REG_WC_VERSION: return "Wired controller version No.";
    case REG_COMP_TARGET_FREQ: return "Unit target frequency";
    case REG_DC_BUS_CURRENT: return "DC bus current";
    case REG_DC_BUS_VOLTAGE: return "DC bus voltage";
    case REG_TF_MODULE_TEMP: return "TF module temperature";
    case REG_CURVE_T1S_CALC_1: return "Climate curve T1S calculated value 1";
    case REG_CURVE_T1S_CALC_2: return "Climate curve T1S calculated value 2";
    case REG_WATER_FLOW: return "Water flow";
    case REG_LIMIT_CURRENT: return "ODU current limit";
    case REG_ABILITY_HM: return "Capacity of hydraulic module";
    case REG_TSOLAR: return "Tsolar";
    case REG_ELEC_CONSUMPTION: return "Electricity consumption";
    case REG_POWER_OUTPUT: return "Power output";
    case REG_RT_HEAT_CAP: return "Real-time heating capacity";
    case REG_RT_RENEW_HEAT_CAP: return "Real-time renewable heating capacity";
    case REG_RT_HEAT_PWR: return "Real-time heating power consumption";
    case REG_RT_HEAT_COP: return "Real-time heating COP";
    case REG_MACHINE_TYPE: return "MachineType";
    case REG_T1S_COOL_UL_12: return "Upper limit of T1S for cooling";
    case REG_T1S_COOL_LL_12: return "Lower limit of T1S for cooling";
    case REG_T1S_HEAT_UL_12: return "Upper limit of T1S for heating";
    case REG_T1S_HEAT_LL_12: return "Lower limit of T1S for heating";
    case REG_TS_UPPER_LIMIT: return "Upper limit for TS setting";
    case REG_TS_LOWER_LIMIT: return "Lower limit for TS setting";
    case REG_WH_UPPER_LIMIT: return "Upper limit for water heating";
    case REG_WH_LOWER_LIMIT: return "Lower limit for water heating";
    case REG_DHW_PUMP_RETURN_TIME: return "Pump running time";
    case REG_DT1S5: return "dT1S5";
    case REG_T4_DHW_MAX: return "T4DHWmax";
    case REG_T4_DHW_MIN: return "T4DHWmin";
    case REG_T_TBH_DELAY: return "t_TBH_delay";
    case REG_DT5_TBH_OFF: return "dT5S_TBH_off";
    case REG_T4_TBH_ON: return "T4_TBH_on";
    case REG_T5S_DI: return "T5s_DI";
    case REG_T_DI_MAX: return "t_DI_max";
    case REG_T_DI_HIGHTEMP: return "t_DI_hightemp";
    case REG_DT1SC: return "dT1SC";
    case REG_DTSC: return "dTSC";
    case REG_T4CMAX: return "T4cmax";
    case REG_T4CMIN: return "T4cmin";
    case REG_DT1SH: return "dT1SH";
    case REG_DTSH: return "dTSH";
    case REG_T4HMAX: return "T4hmax";
    case REG_T4HMIN: return "T4hmin";
    case REG_T4_IBH_ON: return "T4_IBH_on";
    case REG_DT1_IBH_ON: return "dT1_IBH_on";
    case REG_T_IBH_DELAY: return "t_IBH_delay";
    case REG_T4_AHS_ON: return "T4_AHS_on";
    case REG_DT1_AHS_ON: return "dT1_AHS_on";
    case REG_T_AHS_DELAY: return "t_AHS_delay";
    case REG_T_DHWHP_MAX: return "t_DHWHP_max";
    case REG_T_DHWHP_RESTRICT: return "t_DHWHP_restrict";
    case REG_T4AUTOCMIN: return "T4autocmin";
    case REG_T4AUTOHMAX: return "T4autohmax";
    case REG_T1S_H_A_H: return "T1S_H.A_H";
    case REG_T5S_H_A_DHW: return "T5S_H.A_DHW";
    case REG_T_DRYUP: return "t_DRYUP";
    case REG_T_HIGHPEAK: return "t_HIGHPEAK";
    case REG_T_DRYD: return "t_DRYDOWN";
    case REG_T_DRYPEAK: return "t_DRYPEAK";
    case REG_T_FIRSTFH: return "t_ARSTH";
    case REG_T1S_FIRSTFH: return "T1S (initial floor heating)";
    case REG_T1SETC1: return "T1SetC1";
    case REG_T1SETC2: return "T1SetC2";
    case REG_T4C1: return "T4C1";
    case REG_T4C2: return "T4C2";
    case REG_T1SETH1: return "T1SetH1";
    case REG_T1SETH2: return "T1SetH2";
    case REG_T4H1: return "T4H1";
    case REG_T4H2: return "T4H2";
    case REG_POWER_INPUT_LIMIT: return "Power input limitation";
    case REG_T_T4_FRESH_HC: return "t_T4_FRESH_H / t_T4_FRESH_C";
    case REG_T_DELAY_PUMP: return "T_PUMPI_DELAY";
    case REG_ZONE_EMISSION: return "EMISSION TYPE";
    case REG_SOLAR_DELTATSOL: return "Solar function / DELTATSOL";
    case REG_ENSWITCHPDC: return "Ahs_PDC / EnSwitchPDC";
    case REG_GAS_PRICE: return "GAS-COST";
    case REG_ELEC_PRICE: return "ELE-COST";
    case REG_SETHEATER: return "SETHEATER";
    case REG_SIGHEATER: return "SIGHEATER";
    case REG_T2_ANTI_SVRUN: return "t2_Anti_SVRun";
    case REG_Z2_T1SETC1: return "Zone 2 T1SetC1";
    case REG_Z2_T1SETC2: return "Zone 2 T1SetC2";
    case REG_Z2_T4C1: return "Zone 2 T4C1";
    case REG_Z2_T4C2: return "Zone 2 T4C2";
    case REG_Z2_T1SETH1: return "Zone 2 T1SetH1";
    case REG_Z2_T1SETH2: return "Zone 2 T1SetH2";
    case REG_Z2_T4H1: return "Zone 2 T4H1";
    case REG_Z2_T4H2: return "Zone 2 T4H2";
    case REG_TA_ADJUSTMENT: return "Ta adjustment temperature";
    default: return nullptr;
  }
}

const char *register_remark(uint16_t address) {
  switch (address) {
    case REG_OPERATIONAL_MODE: return "1: Auto; 2: Cooling; 3: Heating; others invalid.";
    case REG_T1S_ZONES: return "Low 8 bits Zone 1, high 8 bits Zone 2.";
    case REG_AIR_TEMP_TS: return "Protocol value = actual value * 2.";
    case REG_DHW_T5S: return "Water tank temperature range is 20C to 70C.";
    case REG_FORCED_DHW: return "0 invalid, 1 forced on, 2 forced off.";
    case REG_T_SG_MAX: return "Maximum operation time at high electricity price for smart grid: 0-24h.";
    case REG_COMP_FREQ: return "Compressor operating frequency, Hz.";
    case REG_OPERATING_MODE: return "ODU actual operating mode: 2 cooling, 3 heating, 0 off.";
    case REG_FAN_SPEED: return "Fan speed, r/min.";
    case REG_PMV_OPENNESS: return "Openness of ODU EXV, P.";
    case REG_UNIT_CAPACITY: return "0702 for register 200 is reserved. When it is 071x, data 4-30 means 4-30 kW.";
    case REG_CURRENT_FAULT:
    case REG_FAULT_1:
    case REG_FAULT_2:
    case REG_FAULT_3: return "Check the code table for detailed fault codes.";
    case REG_STATUS_BITS_1: return "Status bits: defrosting, anti-freezing, oil return, remote on/off, thermostat and SG flags.";
    case REG_LOAD_OUTPUT: return "Load output bits: IBH1/2, TBH, pumps, valves, alarm, RUN, aux heat, DEFROST.";
    case REG_DC_BUS_CURRENT: return "Actual value * 10, unit A.";
    case REG_DC_BUS_VOLTAGE: return "Actual value / 10, unit V.";
    case REG_WATER_FLOW: return "Actual value * 100, m3/h.";
    case REG_RT_HEAT_CAP:
    case REG_RT_RENEW_HEAT_CAP:
    case REG_RT_HEAT_PWR:
    case REG_RT_HEAT_COP:
    case REG_RT_RENEW_COOL_CAP:
    case REG_RT_COOL_CAP:
    case REG_RT_COOL_PWR:
    case REG_RT_COOL_EER:
    case REG_RT_DHW_CAP:
    case REG_RT_RENEW_DHW_CAP:
    case REG_RT_DHW_PWR:
    case REG_RT_DHW_COP: return "Actual value * 100.";
    case REG_MACHINE_TYPE: return "PDF documents 06 = A-R290.";
    case REG_DHW_PUMP_RETURN_TIME: return "Default 5 min, range 5-120 min, step 1 min.";
    case REG_POWER_INPUT_LIMIT: return "1-8 = type 1-8, default 1.";
    case REG_T_T4_FRESH_HC: return "Sent value = actual value * 2; low byte heating, high byte cooling.";
    case REG_ZONE_EMISSION: return "Bits 12-15 zone 2 cooling, 8-11 zone 1 cooling, 4-7 zone 2 heating, 0-3 zone 1 heating.";
    case REG_SOLAR_DELTATSOL: return "Low byte solar function, high byte DELTATSOL.";
    case REG_ENSWITCHPDC: return "Bit0 EnSwitchPDC, 1 enable / 0 disable.";
    case REG_GAS_PRICE:
    case REG_ELEC_PRICE: return "Sending value = actual value * 100.";
    case REG_SETHEATER: return "High byte max, low byte min, actual value in degC.";
    case REG_SIGHEATER: return "High byte max, low byte min, actual value in V.";
    case REG_T2_ANTI_SVRUN: return "Valve anti-lock running time, range 0-120 s, default 30 s.";
    default: return nullptr;
  }
}

bool is_read_only_register(uint16_t address) {
  if (address <= 17) {
    return false;
  }
  if (address >= 100 && address <= 199) {
    return true;
  }
  if (address >= 200 && address <= 208) {
    return true;
  }
  return false;
}

bool is_writable_register(uint16_t address) {
  if (address <= 17) {
    return true;
  }
  return address >= 209 && address <= 288;
}

const char *fault_to_string(uint16_t code) {
  switch (code) {
    case 0: return "OK";
    case 1: return "E0";
    case 2: return "E1";
    case 3: return "E2";
    case 4: return "E3";
    case 5: return "E4";
    case 6: return "E5";
    case 7: return "E6";
    case 8: return "E7";
    case 9: return "E8";
    case 10: return "E9";
    case 11: return "EA";
    case 12: return "Eb";
    case 13: return "Ec";
    case 14: return "Ed";
    case 15: return "EE";
    case 20: return "P0";
    case 21: return "P1";
    case 23: return "P3";
    case 24: return "P4";
    case 25: return "P5";
    case 26: return "P6";
    case 31: return "Pb";
    case 33: return "Pd";
    case 38: return "PP";
    case 39: return "H0";
    case 40: return "H1";
    case 41: return "H2";
    case 42: return "H3";
    case 43: return "H4";
    case 44: return "H5";
    case 45: return "H6";
    case 46: return "H7";
    case 47: return "H8";
    case 48: return "H9";
    case 49: return "HA";
    case 50: return "Hb";
    case 52: return "Hd";
    case 53: return "HE";
    case 54: return "HF";
    case 55: return "HH";
    case 57: return "HP";
    case 65: return "C7";
    case 112: return "bH";
    case 116: return "F1";
    case 134: return "L0";
    case 135: return "L1";
    case 136: return "L2";
    case 138: return "L4";
    case 139: return "L5";
    case 141: return "L7";
    case 142: return "L8";
    case 143: return "L9";
    default: return "Unknown";
  }
}

const char *fault_to_description(uint16_t code) {
  switch (code) {
    case 0: return "OK";
    case 1: return "Water flow fault(E8 displayed 3 times)";
    case 2: return "Phase loss or neutral wire and live wire are connected reversely";
    case 3: return "Communication fault between controller and hydraulic module";
    case 4: return "Final outlet water temp. sensor(T1) fault";
    case 5: return "Water tank temp. sensor(T5) fault";
    case 6: return "Condenser outlet refrigerant temperature sensor(T3) fault";
    case 7: return "Ambient temperature sensor(T4) fault";
    case 8: return "Buffer tank up temp. sensor(Tbt1) fault";
    case 9: return "Water flow failure";
    case 10: return "Suction temp. sensor (Th) fault";
    case 11: return "Discharge temp. sensor (Tp) fault";
    case 12: return "Solar temp. sensor(Tsolar) fault";
    case 13: return "Buffer tank low temp. sensor(Tbt2) fault";
    case 14: return "Inlet water temp. sensor(Tw_in) malfunction";
    case 15: return "Hydraulic module EEprom failure";
    case 20: return "Low pressure switch protection";
    case 21: return "High pressure switch protection";
    case 23: return "Compressor overcurrent protection";
    case 24: return "High discharge temperature protection";
    case 25: return "|Tw_out - Tw_in| value too big protection";
    case 26: return "Inverter module protection";
    case 31: return "Anti-freeze mode";
    case 33: return "High temperature protection of refrigerant outlet temp. of condenser";
    case 38: return "Tw_out - Tw_in unusual protection";
    case 39: return "Communication fault between PCB B and main control board";
    case 40: return "Communication fault between inverter module and PCB B";
    case 41: return "Refrigerant liquid temp. sensor(T2) fault";
    case 42: return "Refrigerant gas temp. sensor(T2B) fault";
    case 43: return "Three times P6(L0/L1) protection";
    case 44: return "Room temp. sensor (Ta) fault";
    case 45: return "DC fan motor fault";
    case 46: return "Voltage protection";
    case 47: return "Pressure sensor fault";
    case 48: return "Outlet water for zone 2 temp. sensor(Tw2) fault";
    case 49: return "Outlet water temp. sensor(Tw_out) fault";
    case 50: return "3 times PP protection and Tw_out < 7C";
    case 52: return "Communication fault between hydraulic modules in parallel";
    case 53: return "Communication error between main board and thermostat transfer board";
    case 54: return "Inverter module board EE PROM fault";
    case 55: return "H6 display 10 times in 2 hours";
    case 57: return "Low pressure protection (Pe<0.6) occurred 3 times in 1 hour";
    case 65: return "Transducer module temperature too high protection";
    case 112: return "PED PCB fault";
    case 116: return "Low DC generatrix voltage protection";
    case 134: return "Module protection";
    case 135: return "DC generatrix low voltage protection";
    case 136: return "DC generatrix high voltage protection";
    case 138: return "MCE fault";
    case 139: return "Zero speed protection";
    case 141: return "Phase sequence fault";
    case 142: return "Speed difference > 15Hz between front and back clock";
    case 143: return "Speed difference > 15Hz between real and set speed";
    default: return "Unknown";
  }
}

const char *operating_mode_to_string(uint16_t value) {
  switch (value) {
    case 0: return "OFF";
    case 2: return "Cooling";
    case 3: return "Heating";
    case 5: return "DHW Heating";
    default: return "Unknown";
  }
}

const char *hp_operation_mode_to_string(uint16_t value) {
  switch (value) {
    case 0: return "Off";
    case 2: return "Cooling";
    case 3: return "Heating";
    case 5: return "DHW";
    default: return "Invalid";
  }
}

const char *machine_type_to_string(uint16_t value) {
  switch (value) {
    case 6: return "A-R290";
    default: return "Unknown";
  }
}

const char *hm_submodel_to_string(uint16_t value) {
  switch (value) {
    case 0: return "R32-P";
    case 1: return "Aqua";
    case 2: return "C-R32-P";
    case 3: return "R290-A";
    case 4: return "R290-N";
    case 5: return "C-R290-A";
    case 6: return "C-R290-N";
    case 7: return "R32-A";
    case 8: return "C-R32-A";
    case 9: return "R290-M";
    case 10: return "R32-H";
    default: return "Unknown";
  }
}

const char *operational_mode_select_to_string(uint16_t value) {
  switch (value) {
    case 3: return "Heat";
    case 2: return "Cool";
    case 1: return "Auto";
    default: return nullptr;
  }
}

std::string product_code_to_string(uint16_t value) {
  const uint8_t code = (value & 0x00F0) >> 4;
  return code == 4 ? "4" : std::to_string(code);
}

}  // namespace protocol
}  // namespace hyundai_heatpump
}  // namespace esphome
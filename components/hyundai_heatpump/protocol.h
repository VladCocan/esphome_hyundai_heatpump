#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "modbus_controller.h"

namespace esphome {
namespace hyundai_heatpump {
namespace protocol {

// Annex 3: wired controller register map. The PDF uses decimal register
// addresses; the values below are kept as the exact numeric addresses used on
// the Modbus bus. PLC numbers shown in comments come directly from the table.

// Writable wired-controller registers (pages 88-89 of the PDF).
constexpr uint16_t REG_SWITCHES_0 = 0;          // PLC 40001, Power on/off bits.
constexpr uint16_t REG_OPERATIONAL_MODE = 1;    // PLC 40002, Mode setting: 1=Auto, 2=Cooling, 3=Heating.
constexpr uint16_t REG_T1S_ZONES = 2;           // PLC 40003, low byte Zone 1 T1S, high byte Zone 2 T1S2.
constexpr uint16_t REG_AIR_TEMP_TS = 3;         // PLC 40004, TS room setpoint, protocol value = actual * 2.
constexpr uint16_t REG_DHW_T5S = 4;             // PLC 40005, DHW setpoint T5S.
constexpr uint16_t REG_FUNC_SETTINGS_5 = 5;     // PLC 40006, function setting bits.
constexpr uint16_t REG_WC_CURVE = 6;            // PLC 40007, climate curve selection, Zone 1/Zone 2 packed.
constexpr uint16_t REG_FORCED_DHW = 7;          // PLC 40008, forced water heating: 0 invalid, 1 on, 2 off.
constexpr uint16_t REG_FORCED_TBH = 8;          // PLC 40009, forced tank electric heater TBH.
constexpr uint16_t REG_FORCED_IBH1 = 9;         // PLC 40010, forced rear electric heater IBH1.
constexpr uint16_t REG_T_SG_MAX = 10;           // PLC 40011, max operation time at high electricity price.
constexpr uint16_t REG_WATER_FLOW_CTRL = 16;    // PLC 40017, water flow temperature control switch.
constexpr uint16_t REG_ROOM_TEMP_CTRL = 17;     // PLC 40018, room temperature control switch.

// Whole-unit operating registers, read-only (page 89-91).
constexpr uint16_t REG_COMP_FREQ = 100;         // PLC 40101, operating frequency, Hz.
constexpr uint16_t REG_OPERATING_MODE = 101;    // PLC 40102, ODU actual operating mode.
constexpr uint16_t REG_FAN_SPEED = 102;         // PLC 40103, fan speed, r/min.
constexpr uint16_t REG_PMV_OPENNESS = 103;      // PLC 40104, ODU EXV openness.
constexpr uint16_t REG_WATER_INLET_TEMP = 104;  // PLC 40105, TW_in, degC.
constexpr uint16_t REG_WATER_OUTLET_TEMP = 105; // PLC 40106, TW_out, degC.
constexpr uint16_t REG_COND_TEMP_T3 = 106;      // PLC 40107, condenser temperature T3.
constexpr uint16_t REG_OUTDOOR_AMBIENT = 107;   // PLC 40108, outdoor ambient temperature T4.
constexpr uint16_t REG_DISCHARGE_TEMP = 108;    // PLC 40109, compressor discharge temperature Tp.
constexpr uint16_t REG_RETURN_AIR_TEMP = 109;   // PLC 40110, compressor return air temperature.
constexpr uint16_t REG_TOTAL_WATER_T1 = 110;    // PLC 40111, total outlet water temperature T1.
constexpr uint16_t REG_SYSTEM_WATER_T1B = 111;  // PLC 40112, system outlet water temperature behind AHS.
constexpr uint16_t REG_REFRIG_LIQUID_T2 = 112;  // PLC 40113, refrigerant liquid side temperature T2.
constexpr uint16_t REG_REFRIG_GAS_T2B = 113;    // PLC 40114, refrigerant gas side temperature T2B.
constexpr uint16_t REG_ROOM_TEMP_TA = 114;      // PLC 40115, room temperature Ta.
constexpr uint16_t REG_WATER_TANK_T5 = 115;     // PLC 40116, tank temperature T5.
constexpr uint16_t REG_HIGH_PRESSURE = 116;     // PLC 40117, ODU high pressure, kPa.
constexpr uint16_t REG_LOW_PRESSURE = 117;      // PLC 40118, ODU low pressure, kPa.
constexpr uint16_t REG_OUTDOOR_CURRENT = 118;   // PLC 40119, ODU operating current, A.
constexpr uint16_t REG_OUTDOOR_VOLTAGE = 119;   // PLC 40120, ODU voltage, V.
constexpr uint16_t REG_TBT1 = 120;              // PLC 40121, Tbt1.
constexpr uint16_t REG_TBT2 = 121;              // PLC 40122, Tbt2.
constexpr uint16_t REG_COMP_OP_TIME = 122;      // PLC 40123, compressor operation time, hour.
constexpr uint16_t REG_UNIT_CAPACITY = 123;     // PLC 40124, unit capacity, special 0702/071x remark in PDF.
constexpr uint16_t REG_CURRENT_FAULT = 124;     // PLC 40125, current fault code.
constexpr uint16_t REG_FAULT_1 = 125;           // PLC 40126, fault history 1.
constexpr uint16_t REG_FAULT_2 = 126;           // PLC 40127, fault history 2.
constexpr uint16_t REG_FAULT_3 = 127;           // PLC 40128, fault history 3.
constexpr uint16_t REG_STATUS_BITS_1 = 128;     // PLC 40129, status bitfield.
constexpr uint16_t REG_LOAD_OUTPUT = 129;       // PLC 40130, load output bitfield.
constexpr uint16_t REG_SW_VERSION = 130;        // PLC 40131, hydraulic module software version.
constexpr uint16_t REG_WC_VERSION = 131;        // PLC 40132, wired controller version number.
constexpr uint16_t REG_COMP_TARGET_FREQ = 132;  // PLC 40133, unit target frequency.
constexpr uint16_t REG_DC_BUS_CURRENT = 133;    // PLC 40134, actual value * 10, A.
constexpr uint16_t REG_DC_BUS_VOLTAGE = 134;    // PLC 40135, actual value / 10, V.
constexpr uint16_t REG_TF_MODULE_TEMP = 135;    // PLC 40136, feedback on ODU TF module temperature.
constexpr uint16_t REG_CURVE_T1S_CALC_1 = 136;  // PLC 40137, climate curve calculated T1S Zone 1.
constexpr uint16_t REG_CURVE_T1S_CALC_2 = 137;  // PLC 40138, climate curve calculated T1S Zone 2.
constexpr uint16_t REG_WATER_FLOW = 138;        // PLC 40139, actual value * 100, m3/h.
constexpr uint16_t REG_LIMIT_CURRENT = 139;     // PLC 40140, ODU current limit scheme value.
constexpr uint16_t REG_ABILITY_HM = 140;        // PLC 40141, hydraulic module capacity, actual value * 100.
constexpr uint16_t REG_TSOLAR = 141;            // PLC 40142, Tsolar.
constexpr uint16_t REG_ELEC_CONSUMPTION = 143;  // PLC 40144-40145, high/low words electricity consumption.
constexpr uint16_t REG_POWER_OUTPUT = 145;      // PLC 40146-40147, high/low words power output.
constexpr uint16_t REG_RT_HEAT_CAP = 148;       // PLC 40149, real-time heating capacity, actual * 100.
constexpr uint16_t REG_RT_RENEW_HEAT_CAP = 149; // PLC 40150, real-time renewable heating capacity.
constexpr uint16_t REG_RT_HEAT_PWR = 150;       // PLC 40151, real-time heating power consumption.
constexpr uint16_t REG_RT_HEAT_COP = 151;       // PLC 40152, real-time heating COP, actual * 100.
constexpr uint16_t REG_TOTAL_HEAT_SYS = 152;    // PLC 40153-40154, cumulative system heating energy.
constexpr uint16_t REG_TOTAL_RENEW_HEAT_SYS = 154; // PLC 40155-40156, cumulative renewable system heating energy.
constexpr uint16_t REG_TOTAL_HEAT_PWR_SYS = 156;   // PLC 40157-40158, cumulative system power consumption.
constexpr uint16_t REG_TOTAL_HEAT_MAST = 158;      // PLC 40159-40160, cumulative heating energy.
constexpr uint16_t REG_TOTAL_RENEW_HEAT_MAST = 160; // PLC 40161-40162, cumulative renewable heating energy.
constexpr uint16_t REG_TOTAL_HEAT_PWR_MAST = 162;   // PLC 40163-40164, cumulative power consumption for heating.
constexpr uint16_t REG_TOTAL_COP_HEAT_MAST = 164;   // PLC 40165, cumulative heating efficiency ratio.
constexpr uint16_t REG_TOTAL_COOL_MAST = 165;       // PLC 40166-40167, cumulative cooling energy.
constexpr uint16_t REG_TOTAL_RENEW_COOL_MAST = 167; // PLC 40168-40169, cumulative renewable cooling energy.
constexpr uint16_t REG_TOTAL_COOL_PWR_MAST = 169;   // PLC 40170-40171, cumulative power consumption for cooling.
constexpr uint16_t REG_TOTAL_COP_COOL_MAST = 171;   // PLC 40172, cumulative cooling efficiency ratio.
constexpr uint16_t REG_TOTAL_DHW_MAST = 172;        // PLC 40173-40174, cumulative DHW heating energy.
constexpr uint16_t REG_TOTAL_RENEW_DHW_MAST = 174;  // PLC 40175-40176, cumulative DHW renewable energy.
constexpr uint16_t REG_TOTAL_DHW_PWR_MAST = 176;    // PLC 40177-40178, cumulative power consumption for DHW.
constexpr uint16_t REG_TOTAL_COP_DHW_MAST = 178;    // PLC 40179, cumulative DHW heating COP.
constexpr uint16_t REG_RT_RENEW_COOL_CAP = 179;     // PLC 40180, real-time renewable cooling capacity.
constexpr uint16_t REG_RT_COOL_CAP = 180;           // PLC 40181, real-time cooling capacity.
constexpr uint16_t REG_RT_COOL_PWR = 181;           // PLC 40182, real-time cooling power consumption.
constexpr uint16_t REG_RT_COOL_EER = 182;           // PLC 40183, real-time cooling EER.
constexpr uint16_t REG_RT_DHW_CAP = 183;            // PLC 40184, real-time DHW heating capacity.
constexpr uint16_t REG_RT_RENEW_DHW_CAP = 184;      // PLC 40185, real-time renewable DHW heating capacity.
constexpr uint16_t REG_RT_DHW_PWR = 185;            // PLC 40186, real-time DHW heating power consumption.
constexpr uint16_t REG_RT_DHW_COP = 186;            // PLC 40187, real-time DHW heating COP.
constexpr uint16_t REG_MACHINE_TYPE = 187;          // PLC 40188, MachineType. PDF documents 06 = A-R290.
constexpr uint16_t REG_HM_SUBMODEL = 190;           // Retained from working config; not clearly labeled in the extracted table.
constexpr uint16_t REG_TL_OUTDOOR_PIPE = 191;       // Retained from working config; slave-unit style field.
constexpr uint16_t REG_PUMP_INTERNAL_PWM = 192;     // Retained from working config.
constexpr uint16_t REG_T9I_PHE_INLET = 193;         // Retained from working config.
constexpr uint16_t REG_T9O_PHE_OUTLET = 194;        // Retained from working config.
constexpr uint16_t REG_EXV2_OPENNESS = 195;         // Retained from working config.
constexpr uint16_t REG_EXV3_OPENNESS = 196;         // Retained from working config.
constexpr uint16_t REG_FAN2_SPEED = 197;            // Retained from working config.
constexpr uint16_t REG_CONFIG_STATUS = 198;         // Retained from working config.
constexpr uint16_t REG_HP_OPERATION_MODE = 199;     // Retained from working config.
constexpr uint16_t REG_PRODUCT_CODE = 200;          // Read-only in PDF. 0702 reserved; 071x => 4-30 kW remark near unit capacity/product family.
constexpr uint16_t REG_T1S_COOL_UL_12 = 201;        // PLC 40202, upper limit of T1S for cooling.
constexpr uint16_t REG_T1S_COOL_LL_12 = 202;        // PLC 40203, lower limit of T1S for cooling.
constexpr uint16_t REG_T1S_HEAT_UL_12 = 203;        // PLC 40204, upper limit of T1S for heating.
constexpr uint16_t REG_T1S_HEAT_LL_12 = 204;        // PLC 40205, lower limit of T1S for heating.
constexpr uint16_t REG_TS_UPPER_LIMIT = 205;        // PLC 40206, upper limit for TS setting.
constexpr uint16_t REG_TS_LOWER_LIMIT = 206;        // PLC 40207, lower limit for TS setting.
constexpr uint16_t REG_WH_UPPER_LIMIT = 207;        // PLC 40208, upper limit for water heating.
constexpr uint16_t REG_WH_LOWER_LIMIT = 208;        // PLC 40209, lower limit for water heating.

// Parameter-setting registers, 209+ are writable according to the PDF.
constexpr uint16_t REG_DHW_PUMP_RETURN_TIME = 209;  // PLC 40210, DHW pump return duration.
constexpr uint16_t REG_DT5_ON = 212;                // PLC 40213, dT5_On.
constexpr uint16_t REG_DT1S5 = 213;                 // PLC 40214, dT1S5.
constexpr uint16_t REG_T4_DHW_MAX = 215;            // PLC 40216, T4DHWmax.
constexpr uint16_t REG_T4_DHW_MIN = 216;            // PLC 40217, T4DHWmin.
constexpr uint16_t REG_T_TBH_DELAY = 217;           // PLC 40218, t_TBH_delay.
constexpr uint16_t REG_DT5_TBH_OFF = 218;           // PLC 40219, dT5S_TBH_off.
constexpr uint16_t REG_T4_TBH_ON = 219;             // PLC 40220, T4_TBH_on.
constexpr uint16_t REG_T5S_DI = 220;                // PLC 40221, T5s_DI.
constexpr uint16_t REG_T_DI_MAX = 221;              // PLC 40222, t_DI_max.
constexpr uint16_t REG_T_DI_HIGHTEMP = 222;         // PLC 40223, t_DI_hightemp.
constexpr uint16_t REG_DT1SC = 224;                 // PLC 40225, dT1SC.
constexpr uint16_t REG_DTSC = 225;                  // PLC 40226, dTSC.
constexpr uint16_t REG_T4CMAX = 226;                // PLC 40227, T4cmax.
constexpr uint16_t REG_T4CMIN = 227;                // PLC 40228, T4cmin.
constexpr uint16_t REG_T_INTERVAL_H = 229;          // PLC 40230, omitted name in excerpt, retained from working config.
constexpr uint16_t REG_DT1SH = 230;                 // PLC 40231, dT1SH.
constexpr uint16_t REG_DTSH = 231;                  // PLC 40232, dTSH.
constexpr uint16_t REG_T4HMAX = 232;                // PLC 40233, T4hmax.
constexpr uint16_t REG_T4HMIN = 233;                // PLC 40234, T4hmin.
constexpr uint16_t REG_T4_IBH_ON = 234;             // PLC 40235, T4_IBH_on.
constexpr uint16_t REG_DT1_IBH_ON = 235;            // PLC 40236, dT1_IBH_on.
constexpr uint16_t REG_T_IBH_DELAY = 237;           // PLC 40238, t_IBH_delay.
constexpr uint16_t REG_T4_AHS_ON = 238;             // PLC 40239, T4_AHS_on.
constexpr uint16_t REG_DT1_AHS_ON = 240;            // PLC 40241, dT1_AHS_on.
constexpr uint16_t REG_T_AHS_DELAY = 241;           // PLC 40242, t_AHS_delay.
constexpr uint16_t REG_T_DHWHP_MAX = 242;           // PLC 40243, t_DHWHP_max.
constexpr uint16_t REG_T_DHWHP_RESTRICT = 243;      // PLC 40244, t_DHWHP_restrict.
constexpr uint16_t REG_T4AUTOCMIN = 244;            // PLC 40245, T4autocmin.
constexpr uint16_t REG_T4AUTOHMAX = 245;            // PLC 40246, T4autohmax.
constexpr uint16_t REG_T1S_H_A_H = 246;             // PLC 40247, T1S_H.A_H.
constexpr uint16_t REG_T5S_H_A_DHW = 250;           // PLC 40251, T5S_H.A_DHW.
constexpr uint16_t REG_T_DRYUP = 251;               // PLC 40252, t_DRYUP.
constexpr uint16_t REG_T_HIGHPEAK = 252;            // PLC 40253, t_HIGHPEAK.
constexpr uint16_t REG_T_DRYD = 255;                // PLC 40256, t_DRYDOWN.
constexpr uint16_t REG_T_DRYPEAK = 256;             // PLC 40257, t_DRYPEAK.
constexpr uint16_t REG_T_FIRSTFH = 257;             // PLC 40258, t_ARSTH / first floor heating duration.
constexpr uint16_t REG_T1S_FIRSTFH = 258;           // PLC 40259, initial floor heating T1S.
constexpr uint16_t REG_T1SETC1 = 259;               // PLC 40260, custom cooling curve T1SetC1.
constexpr uint16_t REG_T1SETC2 = 260;               // PLC 40261, custom cooling curve T1SetC2.
constexpr uint16_t REG_T4C1 = 261;                  // PLC 40262, custom cooling curve T4C1.
constexpr uint16_t REG_T4C2 = 262;                  // PLC 40263, custom cooling curve T4C2.
constexpr uint16_t REG_T1SETH1 = 263;               // PLC 40264, custom heating curve T1SetH1.
constexpr uint16_t REG_T1SETH2 = 264;               // PLC 40265, custom heating curve T1SetH2.
constexpr uint16_t REG_T4H1 = 265;                  // PLC 40266, custom heating curve T4H1.
constexpr uint16_t REG_T4H2 = 266;                  // PLC 40267, custom heating curve T4H2.
constexpr uint16_t REG_POWER_INPUT_LIMIT = 267;     // PLC 40268, power input limitation type.
constexpr uint16_t REG_T_T4_FRESH_HC = 268;         // PLC 40269, low byte Heating / high byte Cooling, sent value = actual * 2.
constexpr uint16_t REG_T_DELAY_PUMP = 269;          // PLC 40270, T_PUMPI_DELAY.
constexpr uint16_t REG_ZONE_EMISSION = 270;         // PLC 40271, emission types packed as 4 nibbles.
constexpr uint16_t REG_SOLAR_DELTATSOL = 273;       // PLC 40274, low byte solar function, high byte DELTATSOL.
constexpr uint16_t REG_ENSWITCHPDC = 274;           // PLC 40275, bit0 EnSwitchPDC.
constexpr uint16_t REG_GAS_PRICE = 275;             // PLC 40276, gas price, actual * 100.
constexpr uint16_t REG_ELEC_PRICE = 276;            // PLC 40277, electricity price, actual * 100.
constexpr uint16_t REG_SETHEATER = 277;             // PLC 40278, high byte max / low byte min temperature.
constexpr uint16_t REG_SIGHEATER = 278;             // PLC 40279, high byte max / low byte min voltage.
constexpr uint16_t REG_T2_ANTI_SVRUN = 279;         // PLC 40280, valve anti-lock running time.
constexpr uint16_t REG_Z2_T1SETC1 = 280;            // PLC 40281, Zone 2 custom cooling curve T1SetC1.
constexpr uint16_t REG_Z2_T1SETC2 = 281;            // PLC 40282, Zone 2 custom cooling curve T1SetC2.
constexpr uint16_t REG_Z2_T4C1 = 282;               // PLC 40283, Zone 2 custom cooling curve T4C1.
constexpr uint16_t REG_Z2_T4C2 = 283;               // PLC 40284, Zone 2 custom cooling curve T4C2.
constexpr uint16_t REG_Z2_T1SETH1 = 284;            // PLC 40285, Zone 2 custom heating curve T1SetH1.
constexpr uint16_t REG_Z2_T1SETH2 = 285;            // PLC 40286, Zone 2 custom heating curve T1SetH2.
constexpr uint16_t REG_Z2_T4H1 = 286;               // PLC 40287, Zone 2 custom heating curve T4H1.
constexpr uint16_t REG_Z2_T4H2 = 287;               // PLC 40288, Zone 2 custom heating curve T4H2.
constexpr uint16_t REG_TA_ADJUSTMENT = 288;         // PLC 40289, Ta adjustment temperature.

static const uint16_t BIT_ROOM_TEMP_CTRL = 0x0001;
static const uint16_t BIT_FLOW_CTRL_ZONE1 = 0x0002;
static const uint16_t BIT_POWER_DHW_T5S = 0x0004;
static const uint16_t BIT_FLOW_CTRL_ZONE2 = 0x0008;

static const uint16_t BIT_DISINFECT = 0x0010;
static const uint16_t BIT_SILENT_MODE = 0x0040;
static const uint16_t BIT_SILENT_MODE_LVL = 0x0080;
static const uint16_t BIT_HOLIDAY_HOME = 0x0100;
static const uint16_t BIT_ECO_MODE = 0x0400;
static const uint16_t BIT_DHW_PUMP_RECYCLE = 0x0800;
static const uint16_t BIT_WC_ZONE1 = 0x1000;
static const uint16_t BIT_WC_ZONE2 = 0x2000;

static const uint16_t SBIT_DEFROSTING = 0x0002;
static const uint16_t SBIT_ANTI_FREEZING = 0x0004;
static const uint16_t SBIT_OIL_RETURN = 0x0008;
static const uint16_t SBIT_REMOTE_ONOFF = 0x0010;
static const uint16_t SBIT_TEST_MODE = 0x0020;
static const uint16_t SBIT_HEAT_BY_ROOM_TS = 0x0040;
static const uint16_t SBIT_COOL_BY_ROOM_TS = 0x0080;
static const uint16_t SBIT_SOLAR_INPUT = 0x0100;
static const uint16_t SBIT_ANTI_FREEZE_TANK = 0x0200;
static const uint16_t SBIT_SG = 0x0400;
static const uint16_t SBIT_EUV = 0x0800;
static const uint16_t SBIT_REQ_SERIAL = 0x2000;
static const uint16_t SBIT_REQ_SW_VER = 0x4000;
static const uint16_t SBIT_REQ_OP_PARAM = 0x8000;

static const uint16_t LBIT_IBH1 = 0x0001;
static const uint16_t LBIT_IBH2 = 0x0002;
static const uint16_t LBIT_TBH = 0x0004;
static const uint16_t LBIT_PUMP_I = 0x0008;
static const uint16_t LBIT_SV1 = 0x0010;
static const uint16_t LBIT_SV2 = 0x0020;
static const uint16_t LBIT_PUMP_O = 0x0040;
static const uint16_t LBIT_PUMP_D = 0x0080;
static const uint16_t LBIT_PUMP_C = 0x0100;
static const uint16_t LBIT_SV3 = 0x0200;
static const uint16_t LBIT_HEAT4 = 0x0400;
static const uint16_t LBIT_PUMP_S = 0x0800;
static const uint16_t LBIT_ALARM = 0x1000;
static const uint16_t LBIT_RUN = 0x2000;
static const uint16_t LBIT_AUX_HEAT = 0x4000;
static const uint16_t LBIT_DEFROST = 0x8000;

static const uint16_t CSBIT_TBH_ENABLED = 0x8000;
static const uint16_t CSBIT_AHS_ENABLED = 0x4000;
static const uint16_t CSBIT_T1B_ENABLED = 0x1000;
static const uint16_t CSBIT_AHS_MODE = 0x0800;
static const uint16_t CSBIT_IBH_ENABLED = 0x0400;
static const uint16_t CSBIT_T1_ENABLED = 0x0200;
static const uint16_t CSBIT_ENERGY_METER = 0x0100;
static const uint16_t CSBIT_DHW_OPERATION = 0x0020;
static const uint16_t CSBIT_HEAT_OPERATION = 0x0010;
static const uint16_t CSBIT_COOL_OPERATION = 0x0008;

uint16_t reg_u16(const std::vector<uint8_t> &data, uint16_t start_addr, uint16_t reg);
int16_t reg_s16(const std::vector<uint8_t> &data, uint16_t start_addr, uint16_t reg);
uint32_t reg_u32(const std::vector<uint8_t> &data, uint16_t start_addr, uint16_t reg);

const char *register_name(uint16_t address);
const char *register_remark(uint16_t address);
bool is_read_only_register(uint16_t address);
bool is_writable_register(uint16_t address);

const char *fault_to_string(uint16_t code);
const char *fault_to_description(uint16_t code);
const char *operating_mode_to_string(uint16_t value);
const char *hp_operation_mode_to_string(uint16_t value);
const char *machine_type_to_string(uint16_t value);
const char *hm_submodel_to_string(uint16_t value);
const char *operational_mode_select_to_string(uint16_t value);
std::string product_code_to_string(uint16_t value);

}  // namespace protocol
}  // namespace hyundai_heatpump
}  // namespace esphome
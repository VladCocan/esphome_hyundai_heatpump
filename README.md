# ESPHome Hyundai R290

ESPHome external component for Hyundai R290 heat pumps, with a minimal example node configuration and a maintained Modbus register mapping.

The component is designed to keep the ESPHome YAML clean: UART is configured in YAML, while Modbus transport and controller wiring are created internally by the custom component.

## Highlights

- Clean ESPHome integration through a local external component
- Internal Modbus setup with no top-level `modbus:` or `modbus_controller:` block required
- Dynamic creation of sensors, binary sensors, switches, numbers, selects, and text sensors
- Derived entities for runtime state such as COP, active state, and compressor starts per hour
- Optional parsed and raw debug logging for protocol troubleshooting
- Home Assistant metadata for device classes, units, icons, and diagnostic/config categorization
- Verified against ESPHome `2026.3.1`

## Repository Layout

- `components/hyundai_heatpump/`: external component implementation
- `hyundai-r290.yaml`: example ESPHome node configuration
- `secrets.example.yaml`: placeholder secrets for local setup and CI
- `registers.txt`: protocol and register notes collected during reverse engineering
- `CONTRIBUTING.md`: contribution workflow

## Requirements

- Python 3
- ESPHome `2026.3.1`
- An ESP32 board supported by ESPHome
- Physical UART connection to the heat pump controller
- A verified hardware interface for the controller bus and voltage levels used by your installation

## Installation

1. Clone the repository.
2. Create and activate a Python virtual environment.
3. Install ESPHome `2026.3.1`.
4. Copy `secrets.example.yaml` to `secrets.yaml` if you do not already have a local secrets file.
5. Update Wi-Fi credentials, API keys, OTA password, IP settings, and hardware pins for your installation.
6. Treat `hyundai-r290.yaml` as a starting point only; it contains example values that must be reviewed before flashing.

```bash
python -m venv .venv
source .venv/bin/activate
pip install esphome==2026.3.1
test -f secrets.yaml || cp secrets.example.yaml secrets.yaml
```

## Example Configuration

The repository includes a working example in `hyundai-r290.yaml`. The essential component configuration is intentionally small:

```yaml
external_components:
  - source:
      type: local
      path: components
    components: [hyundai_heatpump]

uart:
  id: uart_bus
  tx_pin: GPIO17
  rx_pin: GPIO16
  baud_rate: 9600
  parity: NONE
  stop_bits: 1

hyundai_heatpump:
  id: heatpump
  uart_id: uart_bus
  address: 1
  update_interval: 10s
```

## Configuration Notes

- `uart_id` is required.
- `address` defaults to `0x01`.
- `update_interval` defaults to `10s`.
- The component internally creates and manages the Modbus client.
- You should not add separate `modbus:` or `modbus_controller:` blocks for this component.

### Debug Options

The component exposes three optional debug flags:

- `debug_log_messages`: logs parsed register values and higher-level state changes
- `debug_log_messages_raw`: logs raw Modbus RX/TX frames
- `debug_log_messages_on_change`: suppresses repeated parsed logs unless values change

Example:

```yaml
hyundai_heatpump:
  id: heatpump
  uart_id: uart_bus
  address: 1
  debug_log_messages: true
  debug_log_messages_raw: true
  debug_log_messages_on_change: true
```

## What The Component Exposes

Depending on the firmware and the mapped registers, the component publishes:

- Temperatures for inlet, outlet, tank, ambient, and room readings
- Operating and fault information
- Power, compressor, and flow-related readings
- Binary runtime and mode flags
- Writable configuration controls through switches, numbers, and selects
- Text diagnostics such as fault descriptions, machine type, and product code

Several entities are also derived in software, including:

- Coefficient of performance
- Heat pump active state
- Water temperature delta
- Compressor starts per hour

## Validation And Build

Recommended local validation flow:

```bash
source .venv/bin/activate
test -f secrets.yaml || cp secrets.example.yaml secrets.yaml
esphome config hyundai-r290.yaml
esphome compile hyundai-r290.yaml
```

The sample configuration in this repository is validated with `esphome config`, and the project has been compiled successfully against ESPHome `2026.3.1`.

## Home Assistant Integration Notes

Entities created by the component are annotated with metadata where semantics are clear:

- device classes
- units of measurement
- icons
- diagnostic and config categories

This keeps the Home Assistant UI cleaner and closer to what you would expect from native ESPHome entities.

## Development Notes

- The component vendors the necessary `modbus_controller` C++ implementation needed for the current ESPHome build flow.
- The internal Modbus parent is created automatically from the configured UART device.
- The example YAML is intentionally minimal and should be adapted before flashing real hardware.

## Safety And Support

- This is an unofficial integration and is not affiliated with Hyundai.
- Confirm electrical compatibility and controller pinout before connecting UART hardware to the heat pump.
- Use caution when enabling writable controls on production heating systems.
- Review register mappings and defaults before changing any heat-pump settings.

## Contributing

See `CONTRIBUTING.md` for contribution expectations and workflow.
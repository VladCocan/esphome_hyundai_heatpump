# ESPHome Hyundai R290

Custom ESPHome component and example configuration for integrating a Hyundai R290 heat pump over Modbus.

This repository contains:

- A local external component in `components/hyundai_heatpump`
- A working ESPHome example in `hyundai-r290.yaml`
- Register notes used while mapping the controller protocol

## Features

- Reads operating, fault, and energy registers from the heat pump
- Exposes writable controls for selected configuration registers
- Publishes higher-level derived entities such as COP, active state, and compressor starts per hour
- Uses a local external component instead of a large inline YAML-only implementation

## Repository Layout

- `components/hyundai_heatpump`: ESPHome external component
- `hyundai-r290.yaml`: example node configuration
- `secrets.example.yaml`: example secrets file for local validation and CI
- `registers.txt`: scratch file for additional register notes

## Quick Start

1. Clone the repository.
2. Create a virtual environment and install ESPHome.
3. Create `secrets.yaml` from `secrets.example.yaml` if it does not already exist, then replace the placeholder values.
4. Adjust pins, network settings, and entity choices in `hyundai-r290.yaml` for your setup.
5. Validate the configuration.

```bash
python -m venv .venv
source .venv/bin/activate
pip install esphome==2026.3.1
test -f secrets.yaml || cp secrets.example.yaml secrets.yaml
esphome config hyundai-r290.yaml
```

## Notes

- This project is not an official Hyundai repository.
- The sample YAML includes network settings and hardware pins that should be reviewed before flashing.
- `secrets.yaml` is ignored by Git and should never be committed.

## GitHub Automation

GitHub Actions validates the sample ESPHome configuration on push and pull request using `secrets.example.yaml`.

## Contributing

See `CONTRIBUTING.md` for the expected workflow before opening a pull request.
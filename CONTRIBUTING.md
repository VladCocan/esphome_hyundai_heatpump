# Contributing

## Development Setup

```bash
python -m venv .venv
source .venv/bin/activate
pip install esphome==2026.3.1
test -f secrets.yaml || cp secrets.example.yaml secrets.yaml
esphome config hyundai-r290.yaml
```

## Before Opening a Pull Request

- Keep changes focused on one problem or feature.
- Do not commit personal Wi-Fi credentials, API keys, or OTA passwords.
- Run `esphome config hyundai-r290.yaml` successfully.
- Update `README.md` if behavior, setup, or supported entities changed.

## Pull Request Expectations

- Describe the heat pump behavior or register area being changed.
- Mention how the change was validated.
- Include logs, screenshots, or register references when they clarify the change.

## Scope

This repository contains both a reusable ESPHome component and a sample node definition. If your change only affects one of those, keep the other untouched unless the update is required for consistency.
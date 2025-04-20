# Gbox420 - ESPHome Edition

Welcome to the **ESPHome version** of the Gbox420 project.  
This is a full rewrite of the original C implementation, designed to run on a Raspberry Pi Pico W (or ESP8266/ESP32) using the [ESPHome](https://esphome.io) platform.
[Project Info & Setup Guide](https://sites.google.com/site/growboxguy/esphome)

Modules:
Gate opener(new) - Open/Close gate or garage door with a virtual button for Home Assistant
Hempy module - Wight based watering system for Hempy buckets (normal buckets without drainage also supported) with interactive Home Assistant Dashboard

Remaining:
Main - For measuring and controlling AC power (Lights,fans,AC-DC adapters)
Aeroponics - Control aeroponics tote (Solenoids, Pressure pump, pressure sensor)
Reservoir - Monitor water reservoir (Weight, PH, TDS, Auto dosing nutrients)
Paradox serial relay(new) - Create a serial bridge between Home Assistant and a Paradox alarm system using [PAI](https://github.com/ParadoxAlarmInterface) (Supported [Systems]: https://github.com/ParadoxAlarmInterface/pai/wiki/Compatibility ) TODO: Move the links to SerialRelay.yaml


## Modules

- **Gate Opener (new):** Open/close a gate or garage door via a virtual Home Assistant button
- **Hempy Module:** Weight-based watering system for Hempy buckets (supports standard non-draining buckets too) with full HA dashboard integration

### 🧪 In Progress / Planned:
- **Main:** Monitor and control AC power (lights, fans, DC adapters)
- **Aeroponics:** Manage an aeroponics tote (solenoids, pump, pressure sensor, temp sensor)
- **Reservoir:** Monitor reservoir weight, pH, TDS, and auto-dosing nutrients
- **Paradox Serial Relay (new):** Serial bridge for Paradox alarm systems using [PAI](https://github.com/ParadoxAlarmInterface/pai)  TODO: Move relevant links to `SerialRelay.yaml`

##  Folder Structure

- `gate.yaml` – ESPHome config for Gate Opener
- `gateDashboard.yaml` – Home Assistant dashboard for Gate Opener
- `hempy.yaml` – ESPHome config for Hempy Module (uses external component from `components/hempy`) or [GitHub](https://github.com/growboxguy/Gbox420/tree/master/ESPHome/components/hempy)
- `components/hempy/__init__.py` – Maps ESPHome YAML config to C++ logic
- `components/hempy/hempy.cpp` – Watering logic (C++)
- `components/hempy/hempy.h` – Header file
- `hempyAutomation.yaml` – Home Assistant automation for mobile phone +  for HA system notifications
- `hempyDashboard.yaml` – Home Assistant dashboard for Hempy Module
- `secrets_gate.yaml`, `secrets_hempy.yaml` – Device-specific sections for your `secrets.yaml`

---

## Getting Started

0. Verify you have Python, Python Package Manager and ESPHome installed
 (Guide: [ESPHome Setup](https://sites.google.com/site/growboxguy/esphome/esphome_hempy#h.h3ovvputqo8q))
   python --version
   pip --version
   esphome --version
1. Copy this folder into your ESPHome config directory
2. Customize the YAML files for your setup
3. Append secrets_XXXX.yaml file content to your own secrets.yaml
4. Build and flash using ESPHome:
   esphome run gate.yaml
   The above command runs all the below commands:
    esphome compile gate.yaml
    esphome upload gate.yaml
    esphome logs gate.yaml       
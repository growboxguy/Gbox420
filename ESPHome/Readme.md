# Gbox420 - ESPHome Edition

Welcome to the **ESPHome version** of the Gbox420 project.  
This is a full rewrite of the original C implementation, designed to run on a Raspberry Pi Pico W (or ESP8266/ESP32) using the [ESPHome](https://esphome.io) platform.<br/>[Project Info & Setup Guide](https://sites.google.com/site/growboxguy/esphome)

## Modules

- **[Gate Opener](https://sites.google.com/site/growboxguy/esphome/esphome_gate)** - Open/close a gate or garage door via a virtual button in Home Assistant 
- **[Hempy Module](https://sites.google.com/site/growboxguy/esphome/esphome_hempy)** - Weight-based watering system for Hempy buckets (supports standard non-draining buckets too) with full HA dashboard integration

### In Progress / Planned:
- **Main:** Monitor and control AC power (lights, fans, DC adapters)
- **Aeroponics:** Manage an aeroponics tote (solenoids, pump, pressure sensor, temp sensor)
- **Reservoir:** Monitor reservoir weight, pH, TDS, and auto-dosing nutrients
- **Paradox Serial Relay (new):** Serial bridge for Paradox alarm systems using [PAI](https://github.com/ParadoxAlarmInterface/pai)
## Folder Structure

### Gate Module
- `gate.yaml` – ESPHome config for Gate Opener  
- `gateDashboard.yaml` – Home Assistant dashboard for Gate Opener  
- `secrets_gate.yaml` – Gate-specific section for your `secrets.yaml`  

### Serial relay - Paradox Alarm Interface
- `serialRelay.yaml` – ESPHome config for Serial relay between Paradox alarm panel and Home Assistant - Paradox Alarm Interface  
- `serialRelayDashboard.yaml` – Home Assistant dashboard for the Paradox alarm 
- `secrets_serialRelay.yaml` – Gate-specific section for your `secrets.yaml`  

### Hempy Module
- `hempy.yaml` – ESPHome config for Hempy Module (uses external component from `components/hempy` or [GitHub](https://github.com/growboxguy/Gbox420/tree/master/ESPHome/components/hempy))  
- `components/hempy/__init__.py` – Maps ESPHome YAML config to C++ logic  
- `components/hempy/hempy.cpp` – Watering logic (C++)  
- `components/hempy/hempy.h` – Header file  
- `hempyAutomation.yaml` – Home Assistant automation for mobile phone + HA system notifications  
- `hempyDashboard.yaml` – Home Assistant dashboard for Hempy Module  
- `secrets_hempy.yaml` – Hempy-specific section for your `secrets.yaml`  

---

## Getting Started 

**Guide:** [ESPHome Setup](https://sites.google.com/site/growboxguy/esphome/esphome_hempy#h.h3ovvputqo8q)

0. Verify you have Python, Python Package Manager and ESPHome installed<br/>
    **python --versio**<br/>
    **pip --version**<br/>
    **esphome --version**<br/>
1. Copy this folder into your ESPHome config directory
2. Customize the YAML files for your setup
3. Append **secrets_XXXX.yaml** file content to your own **secrets.yaml**
4. Build and flash using the following Command Prompt commands<br/>
   **esphome run gate.yaml**<br/><br/>
   The above command runs all the below commands:<br/>
    **esphome compile gate.yaml**<br/>
    **esphome upload gate.yaml**<br/>
    **esphome logs gate.yaml**       
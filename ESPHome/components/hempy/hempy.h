#pragma once

#include "esphome.h" // Include ESPHome macros and functionality
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/hx711/hx711.h"

namespace esphome
{
  namespace hempy
  {

    enum class HempyStates
    {
      DISABLED,  //Watering logic is disabled by the user. Can be re-enabled by starting or stopping the watering pump.
      DRY,  // The plant needs manual watering, the pump possibly failed. Stop the manual watering when the system beeps, that means Max weight is reached
      IDLE, // Hempy bucket weight within limits (between Start watering - Max weight)
      WATERING,  // Water pump currently running, triggered when Start watering weight is reached, or if weight drops below Wet weight - Evaporation target 
      DRAINING  // Wait for the Hempy bucket to drain, measures the bucket's weight loss. The sum of the total weight loss during DRAINING cycles is compared against DrainTarget
    };

    class HempyBucket : public PollingComponent
    {
    public:
      HempyBucket(std::string name, text_sensor::TextSensor *state_sensor, hx711::HX711Sensor *weight_sensor, number::Number *start_watering_weight, number::Number *watering_increments, number::Number *max_watering_weight, number::Number *pump_timeout, number::Number *drain_wait_time, number::Number *drain_target_weight, number::Number *evaporation_target_weight, sensor::Sensor *dry_weight, sensor::Sensor *wet_weight, switch_::Switch *waterPump, uint32_t update_interval) : PollingComponent(update_interval), DefaultUpdateInterval(update_interval), Name(name), StateSensor(state_sensor), WeightSensor(weight_sensor), StartWateringWeight(start_watering_weight), WateringIncrement(watering_increments), MaxWateringWeight(max_watering_weight), MaxWateringTime(pump_timeout), DrainWaitTime(drain_wait_time), DrainTargetWeight(drain_target_weight), EvaporationTargetWeight(evaporation_target_weight), DryWeight(dry_weight), WetWeight(wet_weight), WaterPump(waterPump) {}
      void setup() override;
      void update() override;
      void refresh();                       // Update the weight sensor before calling update()
      void update_interval(uint32_t miliseconds); // How often to call update(). Changes the Polling interval of the component
      void update_state(HempyStates NewState, bool Force = false);
      const char *to_text_state(HempyStates state);
      bool is_watering_active();
      void toggle_watering_logic(int8_t RequestedState = -1);   // Enables or disables weight based watering (Useful when working with the plant). SuspendForMinutes: Automatically re-enable watering after (X minutes)
      void start_watering();                                    // Start watering (re-enables watering logic)
      void stop_watering();                                     // Stops watering
      void toggle_watering();                                   // Triggers watering (re-enables watering logic), or stops watering if it is in progress
      void disable_watering();                                  // Disable watering logic
      void update_next_watering_weight(float weight);           // Force update the next watering weight (Called when Start Water Weight is changed on the dashboard)
      void update_evaportation_target(float EvaporationTarget); // Recalculates watering weight if WetWeight is known
      HempyStates State{HempyStates::IDLE};                     // Stores the current state of the hempy bucket

    private:
      std::string Name;                        // Name of the object
      text_sensor::TextSensor *StateSensor;    // Register a sensor to publish current state: IDLE/WATERING/DRAINING/DISABLED
      hx711::HX711Sensor *WeightSensor;        // Weight sensor object
      number::Number *StartWateringWeight;     // When the bucket weight drops below this -> Start the watering process
      number::Number *WateringIncrement;       // How much water to pump in one cycle, then wait for DrainWaitTime seconds before either starting a new pump cycle (DrainTargetWeight not reached) or considering the watering done (DrainTargetWeight reached)
      number::Number *MaxWateringWeight;       // Safety limit: Disable watering when bucket weight goes above this -> Consider the drain hose clogged and disable the watering logic
      number::Number *MaxWateringTime;         // Safety limit: Maximum total time the pump can run during watering. If the drain target is not hit before the timeout -> Consider the pump broken and disable the watering logic
      number::Number *DrainWaitTime;           // How long to wait between watering cycles for the water to drain in to the waste reservoir
      number::Number *DrainTargetWeight;       // Target weight reduction before watering is considered complete (drain-to-waste system)
      number::Number *EvaporationTargetWeight; // How much weight should the bucket loose before starting another watering. When a watering is complete the wet weight - Evaporation target will give the next start watering weight. Calculated after every watering. After boot, before the first watering "start_watering_weight" is used.
      switch_::Switch *WaterPump;              // Reference to the relay controlling the water pump
      sensor::Sensor *DryWeight;               // Start watering when bucket weight drops below (Initially equals to StartWateringWeight, then calculated after each watering using EvaporationTargetWeight)
      sensor::Sensor *WetWeight;               // Weight measured after watering
      uint32_t StateTimer = 0;                 // Track how much time is spent in one State
      uint32_t PumpOnTimer = 0;                // Track how long watering pump is on continuously (one water-drain cycle)
      uint32_t WateringTimer = 0;              // Track how long watering pump is on in total (all water-drain cycles)
      float StateWeight = 0;                   // Used to store the weight of the bucket when entering a new state
      float DrainProgress = 0;                 // Tracks how much water have drained away during the WATERING-DRAINING cycles
      bool UpdateInProgress = false;           // True while the state of the hempy bucket is updating (update_state running)
      uint32_t DefaultUpdateInterval = 1000;   // Stores the update_interval set in the YAML file in miliseconds
    };

  } // namespace hempy
} // namespace esphome
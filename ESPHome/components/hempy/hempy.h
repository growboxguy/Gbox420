#pragma once

#include "esphome.h" // Include ESPHome macros and functionality
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"

namespace esphome
{
  namespace hempy
  {

    enum class HempyStates
    {
      DISABLED,
      IDLE,
      WATERING,
      DRAINING
    };

    class HempyBucket : public PollingComponent
    {
    public:
      static constexpr int UpdateIntervalMS = 1000; // Polling interval in milliseconds
      HempyBucket(text_sensor::TextSensor *state_sensor, sensor::Sensor *weight_sensor, number::Number *start_watering_weight, number::Number *watering_increments, number::Number *max_watering_weight, number::Number *max_watering_time, number::Number *drain_wait_time, number::Number *drain_target_weight, number::Number *evaporation_target_weight, switch_::Switch *waterPump) : PollingComponent(UpdateIntervalMS), StateSensor(state_sensor), WeightSensor(weight_sensor), StartWateringWeight(start_watering_weight), WateringIncrements(watering_increments), MaxWateringWeight(max_watering_weight), MaxWateringTime(max_watering_time), DrainWaitTime(drain_wait_time), DrainTargetWeight(drain_target_weight), EvaporationTargetWeight(evaporation_target_weight), WaterPump(waterPump) {}
      void setup() override;
      void update() override;
      void update_state(HempyStates NewState, bool Force = false);
      const char *to_text_state(HempyStates state);      
      bool is_watering_active();
      void toggle_watering_logic(int8_t  RequestedState = -1);  //Enables or disables weight based watering (Useful when working with the plant). SuspendForMinutes: Automatically re-enable watering after (X minutes)
      void toggle_watering(); //Triggers watering immediately (automatically re-enables watering logic), or stops watering if it is in progress
      HempyStates State{HempyStates::IDLE};    // Stores the current state of the hempy bucket
      
    private:
      text_sensor::TextSensor *StateSensor;    // Register a sensor to publish current state: IDLE/WATERING/DRAINING/DISABLED
      sensor::Sensor *WeightSensor;            // Weight sensor object
      number::Number *StartWateringWeight;     // When the bucket weight drops below this -> Start the watering process
      number::Number *WateringIncrements;      // How much water to pump in one cycle, then wait for DrainWaitTime seconds before either starting a new pump cycle (DrainTargetWeight not reached) or considering the watering done (DrainTargetWeight reached)
      number::Number *MaxWateringWeight;       // Safety limit: Disable watering when bucket weight goes above this -> Consider the drain hose clogged and disable the watering logic
      number::Number *MaxWateringTime;         // Safety limit: Maximum total time the pump can run during watering. If the drain target is not hit before the timeout -> Consider the pump broken and disable the watering logic
      number::Number *DrainWaitTime;           // How long to wait between watering cycles for the water to drain in to the waste reservoir
      number::Number *DrainTargetWeight;       // How much water should drain before considering the watering done (drain to waste)
      number::Number *EvaporationTargetWeight; // How much weight should the bucket loose before starting another watering. When a watering is complete the wet weight - Evaporation target will give the next start watering weight. Calculated after every watering. After boot, before the first watering "start_watering_weight" is used.
      switch_::Switch *WaterPump;              // Reference to the relay controlling the water pump
      uint32_t StateTimer = 0;                 // Track how much time is spent in one State
      uint32_t PumpOnTimer = 0;                // Track how long watering pump is on continuously (one water-drain cycle)
      uint32_t WateringTimer = 0;              // Track how long watering pump is on in total (all water-drain cycles)
      float DryWeight = 0;                     // Stores the next watering weight (Calculated after watering using EvaporationTargetWeight)
      float BucketStateWeight = 0;             // Used to store the weight of the bucket when entering a new state
      bool UpdateInProgress = false;           // True while the state of the hempy bucket is updating (update_state running)
    };

  } // namespace hempy
} // namespace esphome
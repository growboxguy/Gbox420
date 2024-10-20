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
      HempyBucket(sensor::Sensor *weight_sensor, number::Number *start_watering_weight, number::Number *watering_increments, number::Number *max_watering_weight, number::Number *max_watering_time, number::Number *drain_wait_time, number::Number *drain_target_weight, switch_::Switch *waterPump) : PollingComponent(UpdateIntervalMS), WeightSensor(weight_sensor), StartWateringWeight(start_watering_weight), WateringIncrements(watering_increments), MaxWateringWeight(max_watering_weight), MaxWateringTime(max_watering_time), DrainWaitTime(drain_wait_time), DrainTargetWeight(drain_target_weight), WaterPump(waterPump) {}
      void setup() override;
      void update() override;
      void update_state(HempyStates NewState);
      const char *to_text_state(HempyStates state);
      
    private:
      sensor::Sensor *WeightSensor;        // Weight sensor object
      number::Number *StartWateringWeight; // When the bucket weight drops below this -> Start the watering process
      number::Number *WateringIncrements; // How much water to pump in one cycle, then wait for DrainWaitTime seconds before either starting a new pump cycle (DrainTargetWeight not reached) or considering the watering done (DrainTargetWeight reached)
      number::Number *MaxWateringWeight;   // Safety limit: Disable watering when bucket weight goes above this -> Consider the drain hose clogged and disable the watering logic
      number::Number *MaxWateringTime;     // Safety limit: Maximum total time the pump can run during watering. If the drain target is not hit before the timeout -> Consider the pump broken and disable the watering logic
      number::Number *DrainWaitTime;       // How long to wait between watering cycles for the water to drain in to the waste reservoir
      number::Number *DrainTargetWeight;    // How much water should drain before considering the watering done (drain to waste)
      switch_::Switch *WaterPump;           // Reference to the relay controlling the water pump
      HempyStates State{HempyStates::IDLE}; // Stores the current state of the hempy bucket
      uint32_t StateTimer = 0;              // Track how much time is spent in one State
      uint32_t PumpOnTimer = 0;             // Track how long watering pump is on continuously (one water-drain cycle)
      uint32_t WateringTimer = 0;           // Track how long watering pump is on in total (all water-drain cycles)
      float BucketStartWeight = 0;          // Store the start weight of the bucket before starting the water-drain cycles
    };

  } // namespace hempy
} // namespace esphome
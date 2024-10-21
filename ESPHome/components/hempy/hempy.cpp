#include "hempy.h"

namespace esphome
{
  namespace hempy
  {
    void HempyBucket::setup()
    {
      ESP_LOGI("hempy", "Hempy initialized");
    }

    void HempyBucket::update()
    {
      update_state(State);
      StateSensor->publish_state(to_text_state(State)); // Publish the current state to Home Assistant
      ESP_LOGI("hempy", "State: %s, Weight: %.2fkg (StartWatering: %.1f, Increment: %.1f, Max: %.1f), DrainTarget:%.1fkg (%.0fsec), EvaporationTarget:%.1fkg, DryWeight:%.1fkg",
               to_text_state(State), WeightSensor->state, StartWateringWeight->state, WateringIncrements->state, MaxWateringWeight->state, DrainTargetWeight->state, DrainWaitTime->state, EvaporationTargetWeight->state, DryWeight);
    }

    void HempyBucket::update_state(HempyStates NewState, bool Force)
    {
      if (UpdateInProgress && !Force)
      {
        return; // Exit if an update is already in progress
      }
      UpdateInProgress = true;
      bool BlockOverWritingState = false; // Used when a state transitions to a new state
      uint32_t CurrentTime = millis();
      if (State != NewState)
      {
        StateTimer = CurrentTime; // Start measuring the time spent in the new State
        ESP_LOGI("hempy", "State: %s -> %s", to_text_state(State), to_text_state(NewState));
      }

      switch (NewState)
      {
      case HempyStates::DISABLED:
        if (WaterPump->state)
          WaterPump->turn_off();
        break;
      case HempyStates::IDLE:
        if (WaterPump->state)
          WaterPump->turn_off();
        if ((StartWateringWeight->state > 0 && WeightSensor->state <= StartWateringWeight->state) || (EvaporationTargetWeight->state > 0 && DryWeight > 0 && WeightSensor->state <= DryWeight))
        {
          update_state(HempyStates::WATERING, true);
          BlockOverWritingState = true;
        }
        break;
      case HempyStates::WATERING:
        if (State != NewState)
        {
          BucketStateWeight = WeightSensor->state;                          // Store the bucket weight before starting the pump
          if (State == HempyStates::IDLE || State == HempyStates::DISABLED) // First time entering the WATERING-DRAINING cycles
          {
            PumpOnTimer = CurrentTime; /// Start measuring the pump ON time for this cycle
            WateringTimer = 0;         /// Reset the counter that tracks the total pump ON time during the watering process (multiple WATERING-DRAINING cycles)
          }
          if (State == HempyStates::DRAINING) /// The WATERING-DRAINING cycles are already in progress
          {
            PumpOnTimer = CurrentTime; /// Start measuring the pump ON time for this cycle
          }
          WaterPump->turn_on();
        }
        if (WeightSensor->state >= MaxWateringWeight->state || (WeightSensor->state >= BucketStateWeight + WateringIncrements->state)) // Max weight reached OR WateringIncrements's worth of water was added, wait for it to drain
        {
          WateringTimer += CurrentTime - PumpOnTimer;
          update_state(HempyStates::DRAINING, true);
          BlockOverWritingState = true;
        }
        else if ((CurrentTime - PumpOnTimer) > MaxWateringTime->state * 1000) // Disable watering in case MaxWateringTime is reached: Consider the pump broken
        {
          ESP_LOGW("hempy", "Timeout, pump DISABLED");
          update_state(HempyStates::DISABLED, true);
          BlockOverWritingState = true;
        }
        break;
      case HempyStates::DRAINING:
        if (WaterPump->state)
          WaterPump->turn_off();
        if (State != NewState)
        {
          BucketStateWeight = WeightSensor->state;
          //ESP_LOGW("hempy", "Stored drain start weight: %.2f", BucketStateWeight);
        }
        if (CurrentTime - StateTimer >= (DrainWaitTime->state * 1000)) // Waiting for the water to drain
        {
          //ESP_LOGW("hempy", "if %.2f <=  %.2f - %.2f ", WeightSensor->state, BucketStateWeight, DrainTargetWeight->state);
          if (WeightSensor->state <= (BucketStateWeight - DrainTargetWeight->state)) // Check if enough water was drained into the waste reservoir
          {
            DryWeight = WeightSensor->state - EvaporationTargetWeight->state; // Calculate next watering weight
            update_state(HempyStates::IDLE, true);
          }
          else
          {
            update_state(HempyStates::WATERING, true); /// Continue watering
          }
          BlockOverWritingState = true;
        }
        break;
      }

      if (State != NewState && !BlockOverWritingState)
      {
        State = NewState;
      }
      UpdateInProgress = false;
    }

    const char *HempyBucket::to_text_state(HempyStates state)
    {
      switch (state)
      {
      case HempyStates::DISABLED:
        return "DISABLED";
      case HempyStates::IDLE:
        return "IDLE";
      case HempyStates::WATERING:
        return "WATERING";
      case HempyStates::DRAINING:
        return "DRAINING";
      default:
        return "?";
      }
    }

  } // namespace hempy
} // namespace esphome
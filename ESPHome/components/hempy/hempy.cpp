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
      ESP_LOGI("hempy", "State: %s, Weight: %.2f kg, (start: %.1f, increment: %.1f, max: %.1f), DrainTarget:%.1f (%d sec), EvaporationTarget:%.1f",
               to_text_state(State), WeightSensor->state, StartWateringWeight->state, WateringIncrements->state, MaxWateringWeight->state, DrainTargetWeight->state, DrainWaitTime->state, EvaporationTargetWeight->state); // Log the weight in kg (or the unit configured)
    }

    void HempyBucket::update_state(HempyStates NewState)
    {
      bool BlockOverWritingState = false; // Used when a state transitions to a new state
      uint32_t CurrentTime = millis();
      if (State != NewState)
      {
        StateTimer = CurrentTime; ///< Start measuring the time spent in the new State
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
        if (StartWateringWeight->state <= 0 || WeightSensor->state <= StartWateringWeight->state)
        {
          // if (BucketWeightSensor->getWeight() >= DryWeight - DrainTargetWeight->state) ///< Filters out waterings triggered by a disconnected weight sensor
          {
            // if (BucketWasteReservoir->setReserved())
            {
              update_state(HempyStates::WATERING);
              BlockOverWritingState = true;
            }
          }
        }
        break;
      case HempyStates::WATERING:
        if (State != NewState)
        {
          BucketStartWeight = WeightSensor->state;                          // Store the bucket weight before starting the pump
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
        if (WeightSensor->state >= MaxWateringWeight->state) // TODO: add new logic instead of: && BucketWeightSensor->getWeight(false) - BucketStartWeight + BucketWasteReservoir->getWeightIncrease() >= DrainTargetWeight->state) //Wet weight reached AND Target overflow's worth of water was added, wait for it to drain
        {
          WateringTimer += CurrentTime - PumpOnTimer;
          update_state(HempyStates::DRAINING);
          BlockOverWritingState = true;
        }
        if (WeightSensor->state >= MaxWateringWeight->state) // TODO: add new logic instead of: && BucketWeightSensor->getWeight(false) - BucketStartWeight + BucketWasteReservoir->getWeightIncrease() >= *DrainTarget) //Wet weight reached AND Target overflow's worth of water was added, wait for it to drain
        {
          WateringTimer += CurrentTime - PumpOnTimer;
          update_state(HempyStates::DRAINING);
          BlockOverWritingState = true;
        }
        /* TODO: Add timeout for water pump
        if (WateringTimer > ((uint32_t)BucketPump->getTimeOut() * 1000) || BucketPump->getState() == WaterPumpStates::DISABLED || BucketWasteReservoir->getState() == WasteReservoirStates::FULL) ///< Disable watering if: Timeout before the waste target was reached, pump failed or the waste reservoir got full
        {
          BucketWasteReservoir->clearReservation();
          update_state(HempyStates::DISABLED);
          BlockOverWritingState = true;
        }
        */
        break;
      case HempyStates::DRAINING:
        if (WaterPump->state)
          WaterPump->turn_off();
        State = HempyStates::DRAINING;                                // Store the new state immediately - Only important when DrainWaitTime is set to 0
        if (CurrentTime - StateTimer > (DrainWaitTime->state * 1000)) ///< Waiting for the water to drain
        {
          /*
          if (BucketWasteReservoir->checkTarget(DrainTargetWeight->state)) //Check if target overflow weight is reached
          {
            WetWeight = BucketWeightSensor->getWeight(); //Measure wet weight
            DryWeight = WetWeight - *EvaporationTargetWeight;  //Calculate next watering weight
            BucketWasteReservoir->clearReservation();    ///< Free up the waste reservoir
            update_state(HempyStates::IDLE);
          }
          else
          {
            update_state(HempyStates::WATERING); /// Continue watering
          }
          */
          update_state(HempyStates::IDLE); // TODO: Create new logic for measuring the weight of water that drained away
          BlockOverWritingState = true;
        }
        break;
      }

      if (State != NewState && !BlockOverWritingState)
      {
        State = NewState;
      }
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
#include "hempy.h"

namespace esphome
{
  namespace hempy
  {
    void HempyBucket::setup()
    {
      update_interval(DefaultUpdateInterval);               // Sync the HempyBucket and WeightSensor's update interval
      DryWeight->publish_state(StartWateringWeight->state); // At startup use StartWateringWeight, DryWeight will be calculated after each watering
      WetWeight->publish_state(MaxWateringWeight->state);   // Wet weight at startup is unknown and set to MaxWateringWeight, WetWeight will be calculated after each watering
      ESP_LOGI("hempy", "%s ready", Name.c_str());
    }

    void HempyBucket::update()
    {
      update_state(State);
      StateSensor->publish_state(to_text_state(State)); // Publish the current state to Home Assistant
      ESP_LOGI("hempy", "%s State: %s, Weight: %.2fkg (Increment: %.1f, Max: %.1f), Drain:%.1fkg (%.0fsec), Evaporation:%.1fkg (Dry: %.2fkg, Wet: %.2fkg)",
               Name.c_str(), to_text_state(State), WeightSensor->state, WateringIncrement->state, MaxWateringWeight->state, DrainTargetWeight->state, DrainWaitTime->state, EvaporationTargetWeight->state, DryWeight->state, WetWeight->state);
    }

    void HempyBucket::refresh()
    {
      WeightSensor->update(); // Force sensor update
      update();
    }

    void HempyBucket::update_interval(uint32_t miliseconds) // Update the Polling frequency -> how often should update() run
    {
      this->set_update_interval(miliseconds);
      this->stop_poller();
      this->start_poller();
      WeightSensor->set_update_interval(miliseconds);
      WeightSensor->stop_poller();
      WeightSensor->start_poller();
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
        ESP_LOGI("hempy", "%s State: %s -> %s", Name.c_str(), to_text_state(State), to_text_state(NewState));
      }

      switch (NewState)
      {
      case HempyStates::DISABLED:
        if (WaterPump->state)
          WaterPump->turn_off();
        if (State != NewState)                    // When the state just changed
          update_interval(DefaultUpdateInterval); // Restore the original update_interval from YAML
        break;
      case HempyStates::DRY:
        if (WaterPump->state)
          WaterPump->turn_off(); // Pump is considered failed. With automatic watering it should never switch to DRY mode. Triggers a notification.
        if (State != NewState)   // When the state just changed
          update_interval(1000);
        if (WeightSensor->state >= MaxWateringWeight->state || WeightSensor->state >= WetWeight->state) // Check if manual watering reached the Max Weight
          update_state(HempyStates::IDLE);
          BlockOverWritingState = true;
        break;
      case HempyStates::IDLE:
        if (WaterPump->state)
          WaterPump->turn_off();
        if (State != NewState)                    // When the state just changed
          update_interval(DefaultUpdateInterval); // Restore the original update_interval from YAML
        if ((StartWateringWeight->state > 0 && WeightSensor->state <= StartWateringWeight->state) || (EvaporationTargetWeight->state > 0 && DryWeight->state > 0 && WeightSensor->state <= DryWeight->state))
        {
          update_state(HempyStates::WATERING, true);
          BlockOverWritingState = true;
        }
        break;
      case HempyStates::WATERING:
        if (State != NewState)
        {
          update_interval(1000);              // 1sec - Speed up calling update()
          StateWeight = WeightSensor->state;  // Store the bucket weight before starting the pump
          if (State != HempyStates::DRAINING) // First time entering the WATERING-DRAINING cycles
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
        if (WeightSensor->state >= MaxWateringWeight->state || (WeightSensor->state >= StateWeight + WateringIncrement->state && WeightSensor->state >= WetWeight->state)) // Max weight reached OR (WateringIncrement's worth of water was added AND WetWeight is reached), wait for it to drain
        {
          WateringTimer += CurrentTime - PumpOnTimer;
          update_state(HempyStates::DRAINING, true);
          BlockOverWritingState = true;
        }
        else if ((CurrentTime - PumpOnTimer) > MaxWateringTime->state * 1000) // Disable watering in case MaxWateringTime is reached: Consider the pump broken
        {
          ESP_LOGW("Hempy", "%s Timeout, pump DISABLED", Name.c_str());
          update_state(HempyStates::DRY, true);
          BlockOverWritingState = true;
        }
        break;
      case HempyStates::DRAINING:
        if (WaterPump->state) // If the pump is on -> turn it off
          WaterPump->turn_off();
        if (State != NewState)
        {
          StateWeight = WeightSensor->state;
          // ESP_LOGW("hempy", "Stored drain start weight: %.2f", StateWeight);
        }
        if (CurrentTime - StateTimer >= (DrainWaitTime->state * 1000)) // Waiting for the water to drain
        {
          DrainProgress += StateWeight - WeightSensor->state; // Calculate how much water has drained
          // ESP_LOGW("hempy", "if %.2f <=  %.2f - %.2f ", WeightSensor->state, StateWeight, DrainTargetWeight->state);
          if (DrainProgress >= DrainTargetWeight->state || WeightSensor->state >= MaxWateringWeight->state) // Check if enough water was drained into the waste reservoir, OR MaxWateringWeight is reached
          {
            WetWeight->publish_state(WeightSensor->state);                                    // Store the wet weight
            float CalculatedDryWeight = WeightSensor->state - EvaporationTargetWeight->state; // Calculate next watering weight
            if (CalculatedDryWeight >= StartWateringWeight->state)
              DryWeight->publish_state(CalculatedDryWeight);
            else
              DryWeight->publish_state(StartWateringWeight->state);
            DrainProgress = 0; // Reset the drain tracker
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
      case HempyStates::DRY:
        return "DRY";
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

    bool HempyBucket::is_watering_active()
    {
      return State != HempyStates::DISABLED && State != HempyStates::DRY;
    }

    void HempyBucket::toggle_watering_logic(int8_t RequestedState)
    {
      if ((RequestedState == -1 && State == HempyStates::DISABLED) || RequestedState)
      {
        update_state(HempyStates::IDLE, true);
        ESP_LOGW("hempy", "%s Watering logic enabled", Name.c_str());
      }
      else
      {
        update_state(HempyStates::DISABLED, true);
        ESP_LOGW("hempy", "%s Watering logic disabled", Name.c_str());
      }
    }

    void HempyBucket::start_watering()
    {
      if (State != HempyStates::WATERING && State != HempyStates::DRAINING) // If watering is not in progress: start watering
        update_state(HempyStates::WATERING, true);
    }

    void HempyBucket::stop_watering()
    {
      if (State != HempyStates::IDLE)
        update_state(HempyStates::IDLE, true);
    }

    void HempyBucket::disable_watering()
    {
      if (State != HempyStates::DISABLED)
        update_state(HempyStates::DISABLED, true);
    }

    void HempyBucket::toggle_watering()
    {
      if (State != HempyStates::WATERING && State != HempyStates::DRAINING) // If watering is not in progress: start watering
        update_state(HempyStates::WATERING, true);
      else
        update_state(HempyStates::IDLE, true); // If watering is in progress: stop it (second click stops watering)
    }

    void HempyBucket::update_next_watering_weight(float NewWateringWeight) // Force update the next watering weight (Called when Start Water Weight is changed on the dashboard)
    {
      if (DryWeight->state != NewWateringWeight)
      {
        DryWeight->publish_state(NewWateringWeight);
        ESP_LOGI("hempy", "%s Next watering weight: %.2f", Name.c_str(), NewWateringWeight);
      }
    }

    void HempyBucket::update_evaportation_target(float EvaporationTarget) // Force update the next watering weight (Called when Start Water Weight is changed on the dashboard)
    {
      if (WetWeight->state > 0)
      {
        float NewDryWeight = WetWeight->state - EvaporationTarget; // Calculate new dry weight
        if (StartWateringWeight->state < NewDryWeight)
          DryWeight->publish_state(NewDryWeight); // use the new calculated watering weight
        else
          DryWeight->publish_state(StartWateringWeight->state); // use the Start Weight from the UI

        ESP_LOGI("hempy", "%s Next watering weight: %.2f", Name.c_str(), DryWeight->state);
      }
    }

  } // namespace hempy
} // namespace esphome
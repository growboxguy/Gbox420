#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/HempyBucket.h"

///UNDER DEVELOPMENT - Hempy bucket - website component

class HempyBucket_Web : public HempyBucket, public Common_Web
{
public:
  HempyBucket_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WaterPump *BucketPump );
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void reportToJSON();
  
private:
  
protected:
  Module_Web *Parent; 
};

virtual void refresh(bool Interrupt);  //Aeroponics class cannot be instantiated, 
virtual void report(bool Interrupt);

void aeroSprayNow(bool DueToHighPressure);
void aeroSprayOff();
void aeroPumpOn(bool CalledFromWebsite);
void aeroPumpOff(bool CalledFromWebsite);
void aeroPumpDisable();
void aeroMix();
void setAeroSprayOnOff(bool State);

void setAeroInterval(int interval);
void setAeroDuration(int duration);
void setAeroPressureLow(float PressureLow);
void setAeroPressureHigh(float PressureHigh);
const __FlashStringHelper * pumpStateToText();
//Quiet time section: Blocks running the pump in a pre-defined time range
uint32_t AeroLastRefill= 0;

void setAeroPumpTimeout(int Timeout);
void setAeroPrimingTime(int Timing);

void setQuietRefillOnOff(bool State);
void setAeroPressureTankOnOff(bool State);  //TODO: REMOVE THIS!
void setAeroBlowOnOff(bool State);

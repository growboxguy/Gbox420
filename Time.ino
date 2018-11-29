void updateTime() {  //fills the CurrentTime global variable
  if(UpdateNtpTime)setNtpTime();
  Time Now = Clock.time();  // Get the current time and date from the chip.
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",Now.yr, Now.mon, Now.date,Now.hr, Now.min, Now.sec);  //Format and store time
}

void setNtpTime(){
   time_t now = EspCmd.GetTime() - UNIX_OFFSET + (UTCOffsetHour * 3600) + (UTCOffsetMinute * 60); //https://forum.arduino.cc/index.php?topic=567637.0
   struct tm ts = *localtime(&now);
   char FormattedTime[20];
   strftime(FormattedTime, sizeof(FormattedTime), "%Y/%m/%d-%H:%M:%S", &ts);
   setTime(FormattedTime);
   UpdateNtpTime = false;
}

void setTime(char* dateToSet) {  //sets the real time clock
  addToLog(dateToSet);
  int year    = cropFromString( dateToSet, 0, 4);
  int month   = cropFromString( dateToSet, 5, 2);
  int day     = cropFromString ( dateToSet, 8, 2);
  int hour    = cropFromString ( dateToSet, 11, 2);
  int minute  = cropFromString ( dateToSet, 14, 2);
  int sec  = cropFromString ( dateToSet, 17, 2);  
  Clock.writeProtect(false);  //Disable write protection
  Time timeToSet(year,month,day,hour,minute,sec,Time::kSunday);
  Clock.time(timeToSet);
  Clock.halt(false);  //Enable setting the time
  Clock.writeProtect(true); //Re-enable write protection
}

int cropFromString(char* string,int start, int width){
  int value=0;
  for( int n=0; n < width; n++ )
    value = value * 10 + string[start+n] - '0';
  return value;  
}

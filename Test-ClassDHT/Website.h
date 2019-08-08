#ifndef Website_H
#define Website_H

#include "Common.h"

class Website : public Common
{
  protected:
    static void resetWebServer(void);   // Callback made from esp-link to notify that it has just come out of a reset
    static void loadCallback(char * url); //called when website is loaded
    static void refreshCallback(char * url);  //called when website is refreshed
    static void buttonPressCallback(char *button); //Called when any button on the website is pressed
    static void setFieldCallback(char * field); //Called when any field on the website is updated

  public:
    Website(HardwareSerial *SerialPort);
    void refresh();

  private:  

};

#endif

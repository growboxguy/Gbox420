#pragma once //header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

//These are functions extending the functions of *_Web components, each component must include this file

//forward declaration
extern ELClientWebServer WebServer;

bool isThisMyComponent(char const *lookupName); //When it return true the component belongs to this object
char *getWebsiteComponentName(const __FlashStringHelper *Name);


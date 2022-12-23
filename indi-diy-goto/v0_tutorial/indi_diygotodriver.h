#pragma once

#include "libindi/defaultdevice.h"
#include "indipropertyswitch.h"
#include "indipropertytext.h"


class DIYGoToDriver : public INDI::DefaultDevice
{

public:
    // Class constructor
    DIYGoToDriver();
    // Class destructor
    virtual ~DIYGoToDriver() = default;
    // Pure virtual method: you must override it in your class
    virtual const char *getDefaultName() override;
    // Adding driver properties
    virtual bool initProperties() override;
    // Saving selected property values from previous session
    virtual bool saveConfigItems(FILE *fp) override;

    // Function that gets called when a user clicks on a switch
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[],
                             int n) override;
    // Function that gets called when a user enters text
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[],
                           int n) override;
    // Function that defines what happens when properties are updated
    virtual bool updateProperties() override;


private:
    // Example property: switch
    INDI::PropertySwitch SayHelloSP {1}; // A switch propety with 1 element.
    enum
    {
       HELLO_COMMAND
    };

    // Example property: switch with multiple values
    // Use the inherent autoincrementing of an enum to generate our indexes.
    // This makes keeping track of multiple values on a property MUCH easier
    // than remembering indexes throughout your code.
    // The last value _N is used as the total count.
    enum
    {
      SAY_HI_DEFAULT,
      SAY_HI_CUSTOM,
      SAY_HI_N,
    };
    INDI::PropertySwitch SayHiSP {SAY_HI_N};

    // Example property: text
    INDI::PropertyText WhatToSayTP {1};

    // Example property: number
    INDI::PropertyNumber SayCountNP {1};

};


#pragma once

#include "libindi/defaultdevice.h"
#include "indipropertyswitch.h"

class DIYGoToDriver : public INDI::DefaultDevice
{
public:
    DIYGoToDriver();
    virtual ~DIYGoToDriver() = default;

    // You must override this method in your class.
    virtual const char *getDefaultName() override;


// Adding driver properties
public:
    virtual bool initProperties() override;


// Example property: a simple switch
private:
    INDI::PropertySwitch SayHelloSP {1}; // A switch propety with 1 element.
    enum
    {
       HELLO_COMMAND
    };

// Function that gets called when a user clicks on a switch
public:
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[],
                             int n) override;

};



#include "config.h"
#include "indi_diygotodriver.h"

// We declare an auto pointer to DIYGoToDriver.
static std::unique_ptr<DIYGoToDriver> diygotodriver(new DIYGoToDriver());

// Class constructor
DIYGoToDriver::DIYGoToDriver()
{
    setVersion(CDRIVER_VERSION_MAJOR, CDRIVER_VERSION_MINOR);
}

const char *DIYGoToDriver::getDefaultName()
{
    return "DIY Go-To Driver";
}


// Initialize driver properties
bool DIYGoToDriver::initProperties()
{
    // initialize the parent's properties first
    INDI::DefaultDevice::initProperties();

    // Switch property
    SayHelloSP[HELLO_COMMAND].fill(
       "HELLO_COMMAND",// The name of the VALUE
       "Say Hello",    // The label of the VALUE
       ISS_OFF         // The switch state
       );
    SayHelloSP.fill(
        getDeviceName(),  // The name of the device
        "SAY_HELLO",      // The name of the PROPERTY
        "Example switch", // The label of the PROPERTY
        MAIN_CONTROL_TAB, // What tab should we be on?
        IP_RW,            // Let's make it read/write.
        ISR_ATMOST1,      // At most 1 can be on
        60,               // With a timeout of 60 seconds
        IPS_IDLE          // and an initial state of idle.
        );
    // now we register the property with the DefaultDevice
    // without this, the property won't show up on the control panel
    // NOTE: you don't have to call defineProperty here. You can call it at
    // any time. Maybe you don't want it to show until you are connected, or
    // until the user does something else? Maybe you want to connect, query your
    // device, then call this. It's up to you.
    defineProperty(&SayHelloSP);


    // Switch property with multiple values
    SayHiSP[SAY_HI_DEFAULT].fill(
        "SAY_HI_DEFAULT",    // The name of the VALUE
        "Say Hi",            // The label of the VALUE
        ISS_OFF                 // The switch state
        );
    SayHiSP[SAY_HI_CUSTOM].fill(
        "SAY_HI_CUSTOM",     // The name of the VALUE
        "Say Custom",           // The label of the VALUE
        ISS_OFF                 // The switch state
        );
    SayHiSP.fill(
        getDeviceName(),  // The name of the device
        "SAY_HI",      // The name of the PROPERTY
        "Example mult switch", // The label of the PROPERTY
        MAIN_CONTROL_TAB, // What tab should we be on?
        IP_RW,            // Let's make it read/write.
        ISR_ATMOST1,      // At most 1 can be on
        60,               // With a timeout of 60 seconds
        IPS_IDLE          // and an initial state of idle.
        );        
    defineProperty(&SayHiSP);


//    // Text property
//    WhatToSayTP[0].fill("WHAT_TO_SAY", "What to say?", "Hello, world!");
//    WhatToSayTP.fill(
//          getDeviceName(), 
//          "WHAT_TO_SAY", 
//          "Example text", 
//          MAIN_CONTROL_TAB, 
//          IP_RW, 
//          60, 
//          IPS_IDLE);
//    defineProperty(&WhatToSayTP);

    // Text property: load value from last session, if available
    // We start by assigning the default value to a string
    char configSay[256]={"Hello, world!"};
    // Next we load the value, if any, from the config file. If the operation fails, we still have our default value.
    // If the operation succeeds, we get the config value.
    IUGetConfigText(getDeviceName(), "WHAT_TO_SAY", "WHAT_TO_SAY", configSay, 256);
    // Next we intilize the property like before. This time we set the initial text to configSay.
    WhatToSayTP[0].fill("WHAT_TO_SAY", "What to say?", configSay);
    WhatToSayTP.fill(
          getDeviceName(), 
          "WHAT_TO_SAY", 
          "Example text",
          MAIN_CONTROL_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&WhatToSayTP);



    // Number property
    // Counter of how many times the user clicks the button
    SayCountNP[0].fill(
                 "SAY_COUNT",       // name of the VALUE
                 "Count",           // label of the VALUE
                 "%0.f",            // format specifier to show the value to the user; this should be a format specifier for a double
                 0,                 // minimum value; used by the client to render the UI
                 0,                 // maximum value; used by the client to render the UI
                 0,                 // step value; used by the client to render the UI
                 0);                // current value
    SayCountNP.fill(
                getDeviceName(),    // device name
                "SAY_COUNT",        // PROPERTY name
                "Say Count",        // PROPERTY label
                MAIN_CONTROL_TAB,   // What tab should we be on?
                IP_RO,              // Make this read-only
                0,                  // With no timeout
                IPS_IDLE);          // and an initial state of idle
    defineProperty(&SayCountNP);


    return true;
}


// Save selected property values from last session
bool DIYGoToDriver::saveConfigItems(FILE *fp)
{
    INDI::DefaultDevice::saveConfigItems(fp);
    IUSaveConfigText(fp, &WhatToSayTP);

    return true;
}

// Function that gets called when the user clicks on a switch
bool DIYGoToDriver::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[],
                                 int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (SayHelloSP.isNameMatch(name))
        {
            // Log a message. This will show up in the control panel.
            LOG_INFO("Hello, world!");
            // Turn the switch back off
            SayHelloSP[HELLO_COMMAND].setState(ISS_OFF);
            // Set the property state back to idle
            SayHelloSP.setState(IPS_IDLE);
            // And actually inform INDI of those two operations
            SayHelloSP.apply();

            // Increment our "Say Count" counter.
            // Here we update the value on the property.
            SayCountNP[0].setValue(SayCountNP[0].getValue() + 1);
            // And then send a message to the clients to let them know it is updated.
            SayCountNP.apply();
            // failed attempt at printing the counter value
            //LOG_INFO(SayCountNP[0].getValue());
            //LOG_INFO("Counter = %f", SayCountNP[0].getValue());
            //LOG_INFO(to_string(SayCountNP[0].getValue()));

            // We're done!
            return true;
        }


        if (SayHiSP.isNameMatch(name))
        {
            // Accept what we received.
            SayHiSP.update(states, names, n);

            // Find out what switch was clicked.
            int index = SayHiSP.findOnSwitchIndex();
            switch (index)
            {
            case SAY_HI_DEFAULT: // see how much better this is than direct indexes? USE AN ENUM!
                LOG_INFO("Hi, world!");
                break;
            case SAY_HI_CUSTOM:
                LOG_INFO(WhatToSayTP[0].getText());
                break;
            }
            // Turn all switches back off.
            SayHiSP.reset();
            // Set the property state back to idle
            SayHiSP.setState(IPS_IDLE);
            // And actually inform INDI of those two operations
            SayHiSP.apply();
            return true;
        }

    }
    // Nobody has claimed this, so let the parent handle it
    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}


// Function that gets called when new text is entered
bool DIYGoToDriver::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    // Make sure it is for us.
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (WhatToSayTP.isNameMatch(name))
        {
            // Update the property to what the client sent
            // All elements in the property will now by synced with the client.
            WhatToSayTP.update(texts, names, n);
            // Set state to Idle
            WhatToSayTP.setState(IPS_IDLE);
            // Send back to client.
            WhatToSayTP.apply();
            //log the content of the property to the client
            LOG_INFO(WhatToSayTP[0].getText());
            return true;
        }
    }
    // Nobody has claimed this, so let the parent handle it
    return INDI::DefaultDevice::ISNewText(dev, name, texts, names, n);
}


// This function updates the client about properties
bool DIYGoToDriver::updateProperties()
{
    INDI::DefaultDevice::updateProperties();

    if (isConnected())
    {
        defineProperty(&SayCountNP);
    }
    else
    {
        deleteProperty(SayCountNP);
    }

    return true;
}

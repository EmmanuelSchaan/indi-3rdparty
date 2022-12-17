#include "config.h"
#include "indi_diygotodriver.h"

// We declare an auto pointer to DIYGoToDriver.
static std::unique_ptr<DIYGoToDriver> diygotodriver(new DIYGoToDriver());

DIYGoToDriver::DIYGoToDriver()
{
    setVersion(CDRIVER_VERSION_MAJOR, CDRIVER_VERSION_MINOR);
}

const char *DIYGoToDriver::getDefaultName()
{
    return "DIY Go-To Driver";
}


// Adding driver properties
bool DIYGoToDriver::initProperties()
{
    // initialize the parent's properties first
    INDI::DefaultDevice::initProperties();

    SayHelloSP[HELLO_COMMAND].fill(
    "HELLO_COMMAND",// The name of the VALUE
    "Say Hello",    // The label of the VALUE
    ISS_OFF         // The switch state
    );
    
    SayHelloSP.fill(
        getDeviceName(),  // The name of the device
        "SAY_HELLO",      // The name of the PROPERTY
        "Hello Commands", // The label of the PROPERTY
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
            
            // We're done!
            return true;
        }
    }

    // Nobody has claimed this, so let the parent handle it
    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

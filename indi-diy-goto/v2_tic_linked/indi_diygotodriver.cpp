#include "indi_diygotodriver.h"
// Pololu Tic library
#include <tic.hpp>
 

#include "indicom.h"
#include <cmath>
#include <memory>
 

// We declare an auto pointer to DIYGoTo
static std::unique_ptr<DIYGoTo> diygoto(new DIYGoTo());
 

/**************************************************************************************
** Class constructor 
***************************************************************************************/
DIYGoTo::DIYGoTo()
{
    // We add an additional debug level so we can log verbose scope status
    DBG_SCOPE = INDI::Logger::getInstance().addDebugLevel("Scope Verbose", "SCOPE");
}


/**************************************************************************************
** INDI is asking us for our default device name
***************************************************************************************/
const char *DIYGoTo::getDefaultName()
{
    return "DIY Go-to";
}

 
/**************************************************************************************
** We init our properties here. 
The only thing we want to init are the Debug controls
***************************************************************************************/
bool DIYGoTo::initProperties()
{
    // ALWAYS call initProperties() of parent first
    INDI::Telescope::initProperties();
 
    // Add Debug control so end user can turn debugging/loggin on and off
    addDebugControl();
 
    // Enable simulation mode so that serial connection in INDI::Telescope does not try
    // to attempt to perform a physical connection to the serial port.
    setSimulation(true);
 
    // Set telescope capabilities. 0 is for the the number of slew rates that we support. We have none for this simple driver.
    SetTelescopeCapability(TELESCOPE_CAN_GOTO | TELESCOPE_CAN_ABORT, 0);


    // RA Tic id
    // Text property: load value from last session, if available
    // We start by assigning the default value to a string
    char tidIdRADefault[256]={"00315372"};
    // Next we load the value, if any, from the config file. If the operation fails, we still have our default value.
    // If the operation succeeds, we get the config value.
    IUGetConfigText(getDeviceName(), "TIC_ID_RA", "TIC_ID_RA", tidIdRADefault, 256);
    // Next we intilize the property like before. This time we set the initial text to tidIdRADefault.
    TicIdRA[0].fill("TIC_ID_RA", "Tic ID RA?", tidIdRADefault);
    TicIdRA.fill(
          getDeviceName(), 
          "TID_ID_RA", 
          "Tic ID RA",
          MAIN_CONTROL_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&TicIdRA);

    // Same for the Dec Tic id
    char tidIdDecDefault[256]={"00315338"};
    IUGetConfigText(getDeviceName(), "TIC_ID_DEC", "TIC_ID_DEC", tidIdDecDefault, 256);
    TicIdDec[0].fill("TIC_ID_DEC", "Tic ID Dec?", tidIdDecDefault);
    TicIdDec.fill(
          getDeviceName(), 
          "TID_ID_DEC", 
          "Tic ID Dec",
          MAIN_CONTROL_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&TicIdDec);


    return true;
}



/**************************************************************************************
** Function that gets called when new text is entered
***************************************************************************************/
bool DIYGoTo::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    // Make sure it is for us.
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (TicIdRA.isNameMatch(name))
        {
            // Update the property to what the client sent
            // All elements in the property will now by synced with the client.
            TicIdRA.update(texts, names, n);
            // Set state to Idle
            TicIdRA.setState(IPS_IDLE);
            // Send back to client.
            TicIdRA.apply();
            //log the content of the property to the client
            LOG_INFO(TicIdRA[0].getText());
            return true;
        }
        // Same for Dec Tic id
        if (TicIdDec.isNameMatch(name))
        {
            TicIdDec.update(texts, names, n);
            TicIdDec.setState(IPS_IDLE);
            TicIdDec.apply();
            LOG_INFO(TicIdDec[0].getText());
            return true;
        }
    }
    // Nobody has claimed this, so let the parent handle it
    return INDI::Telescope::ISNewText(dev, name, texts, names, n);
}






/**************************************************************************************
** Pololu Tic function to connect to a Tic
***************************************************************************************/

//// Opens a handle to a Tic that can be used for communication.
////
//// To open a handle to any Tic:
////   tic_handle * handle = open_handle();
//// To open a handle to the Tic with serial number 01234567:
////   tic_handle * handle = open_handle("01234567");
//tic::handle open_handle(const char * desired_serial_number = nullptr)
//{
//  // Get a list of Tic devices connected via USB.
//  std::vector<tic::device> list = tic::list_connected_devices();
// 
//  // Iterate through the list and select one device.
//  for (const tic::device & device : list)
//  {
//    if (desired_serial_number &&
//      device.get_serial_number() != desired_serial_number)
//    {
//      // Found a device with the wrong serial number, so continue on to
//      // the next device in the list.
//      continue;
//    }
// 
//    // Open a handle to this device and return it.
//    return tic::handle(device);
//  }
// 
//  throw std::runtime_error("No device found.");
//}




 
/**************************************************************************************
** INDI is asking us to check communication with the device via a handshake
***************************************************************************************/
bool DIYGoTo::Handshake()
{
    // When communicating with a real mount, we check here if commands are receieved
    // and acknolowedged by the mount. For DIYGoTo, we simply return true.
    return true;
}
 
 
/**************************************************************************************
** Client is asking us to slew to a new position
***************************************************************************************/
bool DIYGoTo::Goto(double ra, double dec)
{
    targetRA  = ra;
    targetDEC = dec;
    char RAStr[64]={0}, DecStr[64]={0};
 
    // Parse the RA/DEC into strings
    fs_sexa(RAStr, targetRA, 2, 3600);
    fs_sexa(DecStr, targetDEC, 2, 3600);
 
    // Mark state as slewing
    TrackState = SCOPE_SLEWING;
 
    // Inform client we are slewing to a new position
    LOGF_INFO("Slewing to RA: %s - DEC: %s", RAStr, DecStr);
 
    // Success!
    return true;
}
 
/**************************************************************************************
** Client is asking us to abort our motion
***************************************************************************************/
bool DIYGoTo::Abort()
{
    return true;
}
 
/**************************************************************************************
** Client is asking us to report telescope status
***************************************************************************************/
bool DIYGoTo::ReadScopeStatus()
{
    static struct timeval ltv { 0, 0 };
    struct timeval tv { 0, 0 };
    double dt = 0, da_ra = 0, da_dec = 0, dx = 0, dy = 0;
    int nlocked;
 
    /* update elapsed time since last poll, don't presume exactly POLLMS */
    gettimeofday(&tv, nullptr);
 
    if (ltv.tv_sec == 0 && ltv.tv_usec == 0)
        ltv = tv;
 
    dt  = tv.tv_sec - ltv.tv_sec + (tv.tv_usec - ltv.tv_usec) / 1e6;
    ltv = tv;
 
    // Calculate how much we moved since last time
    da_ra  = SLEW_RATE * dt;
    da_dec = SLEW_RATE * dt;
 
    /* Process per current state. We check the state of EQUATORIAL_EOD_COORDS_REQUEST and act acoordingly */
    switch (TrackState)
    {
        case SCOPE_SLEWING:
            // Wait until we are "locked" into positon for both RA & DEC axis
            nlocked = 0;
 
            // Calculate diff in RA
            dx = targetRA - currentRA;
 
            // If diff is very small, i.e. smaller than how much we changed since last time, then we reached target RA.
            if (fabs(dx) * 15. <= da_ra)
            {
                currentRA = targetRA;
                nlocked++;
            }
            // Otherwise, increase RA
            else if (dx > 0)
                currentRA += da_ra / 15.;
            // Otherwise, decrease RA
            else
                currentRA -= da_ra / 15.;
 
            // Calculate diff in DEC
            dy = targetDEC - currentDEC;
 
            // If diff is very small, i.e. smaller than how much we changed since last time, then we reached target DEC.
            if (fabs(dy) <= da_dec)
            {
                currentDEC = targetDEC;
                nlocked++;
            }
            // Otherwise, increase DEC
            else if (dy > 0)
                currentDEC += da_dec;
            // Otherwise, decrease DEC
            else
                currentDEC -= da_dec;
 
            // Let's check if we recahed position for both RA/DEC
            if (nlocked == 2)
            {
                // Let's set state to TRACKING
                TrackState = SCOPE_TRACKING;
 
                LOG_INFO("Telescope slew is complete. Tracking...");
            }
            break;
 
        default:
            break;
    }
 
    char RAStr[64]={0}, DecStr[64]={0};
 
    // Parse the RA/DEC into strings
    fs_sexa(RAStr, currentRA, 2, 3600);
    fs_sexa(DecStr, currentDEC, 2, 3600);
 
    DEBUGF(DBG_SCOPE, "Current RA: %s Current DEC: %s", RAStr, DecStr);
 
    NewRaDec(currentRA, currentDEC);
    return true;
}

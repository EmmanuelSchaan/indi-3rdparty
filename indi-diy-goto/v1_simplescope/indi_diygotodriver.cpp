#include "indi_diygotodriver.h"
 
#include "indicom.h"
 
#include <cmath>
#include <memory>
 
static std::unique_ptr<DIYGoTo> diygoto(new DIYGoTo());
 
DIYGoTo::DIYGoTo()
{
    // We add an additional debug level so we can log verbose scope status
    DBG_SCOPE = INDI::Logger::getInstance().addDebugLevel("Scope Verbose", "SCOPE");
}
 
/**************************************************************************************
** We init our properties here. The only thing we want to init are the Debug controls
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
 
    return true;
}
 
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
** INDI is asking us for our default device name
***************************************************************************************/
const char *DIYGoTo::getDefaultName()
{
    return "Simple Scope";
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

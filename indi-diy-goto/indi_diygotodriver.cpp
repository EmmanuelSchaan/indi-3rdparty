#include "indi_diygotodriver.h"

// Pololu Tic library
//#include <tic.hpp>
//#include <iostream>  // Was in the tic code example; may not be needed

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
 
// Manu: I should probably comment this out
    // Enable simulation mode so that serial connection in INDI::Telescope does not try
    // to attempt to perform a physical connection to the serial port.
    setSimulation(true);
 
    // Set telescope capabilities.
    SetTelescopeCapability(TELESCOPE_CAN_GOTO |
//         TELESCOPE_CAN_SYNC |              
//         TELESCOPE_CAN_PARK |              
//         TELESCOPE_HAS_TIME |             
//         TELESCOPE_HAS_LOCATION |            
//         TELESCOPE_HAS_PIER_SIDE |          
//         TELESCOPE_HAS_PEC |                
//         TELESCOPE_HAS_TRACK_MODE |         
//         TELESCOPE_CAN_CONTROL_TRACK |       
//         TELESCOPE_HAS_TRACK_RATE |         
//         TELESCOPE_HAS_PIER_SIDE_SIMULATION |
//         TELESCOPE_CAN_TRACK_SATELLITE |
         TELESCOPE_CAN_ABORT,
         0);   // 0 is for the the number of slew rates that we support. We have none for this simple driver.






    // RA Tic id
    // Text property: load value from last session, if available
    // We start by assigning the default value to a string
    char ticIdRADefault[256]={"00315372"};
    // Next we load the value, if any, from the config file. If the operation fails, we still have our default value.
    // If the operation succeeds, we get the config value.
    IUGetConfigText(getDeviceName(), "TIC_ID_RA", "TIC_ID_RA", ticIdRADefault, 256);
    // Next we intilize the property like before. This time we set the initial text to ticIdRADefault.
    TicIdRA[0].fill("TIC_ID_RA", "Format 01234567", ticIdRADefault);
    TicIdRA.fill(
          getDeviceName(), 
          "TID_ID_RA", 
          "Tic ID RA",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&TicIdRA);

    // Same for the Dec Tic id
    char ticIdDecDefault[256]={"00315338"};
    IUGetConfigText(getDeviceName(), "TIC_ID_DEC", "TIC_ID_DEC", ticIdDecDefault, 256);
    TicIdDec[0].fill("TIC_ID_DEC", "Format 01234567", ticIdDecDefault);
    TicIdDec.fill(
          getDeviceName(), 
          "TID_ID_DEC", 
          "Tic ID Dec",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&TicIdDec);

    // Number of steps per rotation for the stepper motor
    char stepsPerRotationDefault[256]={"200"};
    IUGetConfigText(getDeviceName(), "STEPS_PER_ROTATION", "STEPS_PER_ROTATION", stepsPerRotationDefault, 256);
    StepsPerRotation[0].fill("STEPS_PER_ROTATION", "(Stepper motor spec)", stepsPerRotationDefault);
    StepsPerRotation.fill(
          getDeviceName(), 
          "STEPS_PER_ROTATION", 
          "Steps per rotation",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&StepsPerRotation);

    // Gear reduction factor (same for both motors)
    //  gearbox reduction: (99. + 104./2057.)
    //  pinion-wheel reduction: 84./20.
    //  --> total reduction: 416.0123480797
    char gearReductionFactorDefault[256]={"416.0123480797"};
    IUGetConfigText(getDeviceName(), "GEAR_REDUCTION_FACTOR", "GEAR_REDUCTION_FACTOR", gearReductionFactorDefault, 256);
    GearReductionFactor[0].fill("GEAR_REDUCTION_FACTOR", "motor angle / output angle", gearReductionFactorDefault);
    GearReductionFactor.fill(
          getDeviceName(), 
          "GEAR_REDUCTION_FACTOR", 
          "Gear reduction factor",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&GearReductionFactor);

    // Limiting current for stepper motors
    char motorMaxCurrentDefault[256]={"192"};
    IUGetConfigText(getDeviceName(), "MOTOR_MAX_CURRENT", "MOTOR_MAX_CURRENT", motorMaxCurrentDefault, 256);
    MotorMaxCurrent[0].fill("MOTOR_MAX_CURRENT", "[mA]", motorMaxCurrentDefault);
    MotorMaxCurrent.fill(
          getDeviceName(), 
          "MOTOR_MAX_CURRENT", 
          "Max current",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&MotorMaxCurrent);

    // Microstepping setting,
    // i.e. nb of microsteps per motor step
    char microsteppingDefault[256]={"192"};
    IUGetConfigText(getDeviceName(), "MICROSTEPPING", "MICROSTEPPING", microsteppingDefault, 256);
    Microstepping[0].fill("MICROSTEPPING", "Nb of microsteps", microsteppingDefault);
    Microstepping.fill(
          getDeviceName(), 
          "MICROSTEPPING", 
          "Microstepping",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&Microstepping);

    // Maximum pulse speed [1.e-4 pulses/sec]
    char maxPulseSpeedDefault[256]={"400000000"};
    IUGetConfigText(getDeviceName(), "maxPulseSpeed", "maxPulseSpeed", maxPulseSpeedDefault, 256);
    MaxPulseSpeed[0].fill("maxPulseSpeed", "[1.e-4 * pulses/sec]", maxPulseSpeedDefault);
    MaxPulseSpeed.fill(
          getDeviceName(), 
          "maxPulseSpeed", 
          "Max pulse speed",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&MaxPulseSpeed);

    // Maximum pulse accel/deceleration [1.e-4 pulses/sec^2]
    char maxPulseAccelDecelDefault[256]={"4000000"};
    IUGetConfigText(getDeviceName(), "maxPulseAccelDecel", "maxPulseAccelDecel", maxPulseAccelDecelDefault, 256);
    MaxPulseAccelDecel[0].fill("maxPulseAccelDecel", "[1.e-4 * pulses/sec^2]", maxPulseAccelDecelDefault);
    MaxPulseAccelDecel.fill(
          getDeviceName(), 
          "maxPulseAccelDecel", 
          "Max pulse accel/decel",
          INFO_TAB, IP_RW, 
          60, 
          IPS_IDLE);
    defineProperty(&MaxPulseAccelDecel);


    // Connect to the Pololu Tics
    LOG_INFO("Attempting connection to RA and Dec Tics");
    connectTics();
    // Set the Tic parameters
    setupTics();
    // Energize and exit safe start
    energizeTics();

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
            LOGF_INFO("RA Tic ID set to %s", TicIdRA[0].getText());
            // Re-connect to the Pololu Tics
            connectTics();
            setupTics();
            energizeTics();
            return true;
        }
        if (TicIdDec.isNameMatch(name))
        {
            TicIdDec.update(texts, names, n);
            TicIdDec.setState(IPS_IDLE);
            TicIdDec.apply();
            LOGF_INFO("Dec Tic ID set to %s", TicIdDec[0].getText());
            // Re-connect to the Pololu Tics
            connectTics();
            setupTics();
            energizeTics();
            return true;
        }
        if (StepsPerRotation.isNameMatch(name))
        {
            StepsPerRotation.update(texts, names, n);
            StepsPerRotation.setState(IPS_IDLE);
            StepsPerRotation.apply();
            LOGF_INFO("Step. mot. steps per rotation set to %s", StepsPerRotation[0].getText());
            return true;
        }
        if (GearReductionFactor.isNameMatch(name))
        {
            GearReductionFactor.update(texts, names, n);
            GearReductionFactor.setState(IPS_IDLE);
            GearReductionFactor.apply();
            LOGF_INFO("Gear reduction factor set to %s", GearReductionFactor[0].getText());
            return true;
        }
        if (MotorMaxCurrent.isNameMatch(name))
        {
            MotorMaxCurrent.update(texts, names, n);
            MotorMaxCurrent.setState(IPS_IDLE);
            MotorMaxCurrent.apply();
            LOGF_INFO("Motor max current set to %s [mA]", MotorMaxCurrent[0].getText());
            // Re-setup Tics
            setupTics();
            energizeTics();
            return true;
        }
        if (Microstepping.isNameMatch(name))
        {
            Microstepping.update(texts, names, n);
            Microstepping.setState(IPS_IDLE);
            Microstepping.apply();
            LOGF_INFO("Microstepping set to %s", Microstepping[0].getText());
            // Re-setup Tics
            setupTics();
            energizeTics();
            return true;
        }
        if (MaxPulseSpeed.isNameMatch(name))
        {
            MaxPulseSpeed.update(texts, names, n);
            MaxPulseSpeed.setState(IPS_IDLE);
            MaxPulseSpeed.apply();
            LOGF_INFO("Max pulse speed set to %s * 1.e-4 pulses/sec", MaxPulseSpeed[0].getText());
            // Re-setup Tics
            setupTics();
            energizeTics();
            return true;
        }
        if (MaxPulseAccelDecel.isNameMatch(name))
        {
            MaxPulseAccelDecel.update(texts, names, n);
            MaxPulseAccelDecel.setState(IPS_IDLE);
            MaxPulseAccelDecel.apply();
            LOGF_INFO("Max pulse accel/decel set to %s * 1.e-4 pulses/sec^2", MaxPulseAccelDecel[0].getText());
            // Re-setup Tics
            setupTics();
            energizeTics();
            return true;
        }
    }
    // Nobody has claimed this, so let the parent handle it
    return INDI::Telescope::ISNewText(dev, name, texts, names, n);
}


/**************************************************************************************
** Pololu Tic function to connect to a Tic
***************************************************************************************/

// Opens a handle to a Tic that can be used for communication.
//
// To open a handle to any Tic:
//   tic_handle * handle = open_handle();
// To open a handle to the Tic with serial number 01234567:
//   tic_handle * handle = open_handle("01234567");
tic::handle DIYGoTo::open_tic_handle(const char * desired_serial_number = nullptr)
{
  // Get a list of Tic devices connected via USB.
  std::vector<tic::device> list = tic::list_connected_devices();
 
  // Iterate through the list and select one device.
  for (const tic::device & device : list)
  {
    if (desired_serial_number &&
      device.get_serial_number() != desired_serial_number)
    {
      // Found a device with the wrong serial number, so continue on to
      // the next device in the list.
      continue;
    }
    // Open a handle to this device and return it.
    return tic::handle(device);
  }
  throw std::runtime_error("No Tic devices found.");
}


// Establish connection to both RA and Dec Tics,
// given the desired Tic ID,
// and get their handles and variables.
bool DIYGoTo::connectTics()
{
    try 
    {
       // RA Tic
       handleTicRA = open_tic_handle(TicIdRA[0].getText());
       varTicRA = handleTicRA.get_variables();
       // Dec Tic
       handleTicDec = open_tic_handle(TicIdDec[0].getText());
       varTicDec = handleTicDec.get_variables();
       
       LOG_INFO("Successfully connected to RA and Dec Tics.");
    }
    catch(const std::exception &error)
    {
       LOGF_ERROR("Could not connect to the RA and Dec Tics: %s", error.what());
       return false;
    }
    return true;
}


// Send all settings to RA and Dec Tics
bool DIYGoTo::setupTics()
{
    try 
    {
      // RA motor
      // Set the maximum motor current
      handleTicRA.set_current_limit(static_cast<uint32_t>(std::stoul(MotorMaxCurrent[0].getText())));
      //handleTicRA.set_current_limit_code(uint8_t code);
      // Set the microstepping mode
      handleTicRA.set_step_mode(static_cast<uint8_t>(std::stoul(Microstepping[0].getText())));
      // Set max speed, acceleration, deceleration, decay mode
      handleTicRA.set_max_speed(static_cast<uint32_t>(std::stoul(MaxPulseSpeed[0].getText())));
      handleTicRA.set_max_accel(static_cast<uint32_t>(std::stoul(MaxPulseAccelDecel[0].getText())));
      handleTicRA.set_max_decel(static_cast<uint32_t>(std::stoul(MaxPulseAccelDecel[0].getText())));
      // For my Tic T834, the 'mixed50' decay mode
      // corresponds
      //#define TIC_DECAY_MODE_T834_MIXED50 0
      //#define TIC_DECAY_MODE_T834_SLOW 1
      //#define TIC_DECAY_MODE_T834_FAST 2
      //#define TIC_DECAY_MODE_T834_MIXED25 3
      //#define TIC_DECAY_MODE_T834_MIXED75 4 to code 0
      handleTicRA.set_decay_mode(0);

      // Dec motor
      handleTicDec.set_current_limit(static_cast<uint32_t>(std::stoul(MotorMaxCurrent[0].getText())));
      handleTicDec.set_step_mode(static_cast<uint8_t>(std::stoul(Microstepping[0].getText())));
      handleTicDec.set_max_speed(static_cast<uint32_t>(std::stoul(MaxPulseSpeed[0].getText())));
      handleTicDec.set_max_accel(static_cast<uint32_t>(std::stoul(MaxPulseAccelDecel[0].getText())));
      handleTicDec.set_max_decel(static_cast<uint32_t>(std::stoul(MaxPulseAccelDecel[0].getText())));
      handleTicDec.set_decay_mode(0);

      LOG_INFO("Tic settings done.");
    }
    catch(const std::exception &error)
    {
       LOGF_ERROR("Could not input Tic settings: %s", error.what());
       return false;
    }
    return true;
}

// Energize Tics and exit safe start,
// so that motors can move
bool DIYGoTo::energizeTics()
{
    try 
    {
       // RA Tic
       handleTicRA.energize();
       handleTicRA.exit_safe_start();
       // Dec Tic
       handleTicDec.energize();
       handleTicDec.exit_safe_start();
       LOG_INFO("Successfully energized RA and Dec Tics.");
    }
    catch(const std::exception &error)
    {
       LOGF_ERROR("Could not energize RA and Dec Tics: %s", error.what());
       return false;
    }
    return true;
}


bool DIYGoTo::deenergizeTics()
{
    try 
    {
       handleTicRA.deenergize();
       handleTicDec.deenergize();
       LOG_INFO("Successfully de-energized RA and Dec Tics.");
    }
    catch(const std::exception &error)
    {
       LOGF_ERROR("Could not de-energize RA and Dec Tics: %s", error.what());
       return false;
    }
    return true;
}




//// Other useful Tic functions:
//void handle::set_target_position(int32_t position)
//void handle::set_target_velocity(int32_t velocity)
//void handle::halt_and_set_position(int32_t position)
//void handle::halt_and_hold()



 
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
** Driver loop, called periodically
***************************************************************************************/
void DIYGoTo::TimerHit()
{
    if (!isConnected())
        return;

//    LOG_INFO("timer hit");
    try 
    {
       // The Tic will stop moving unless a reset command timeout 
       // is sent every second.
       // This can be avoided by unchecking the box in the ticgui
       // called "Enable command timeout"
       handleTicRA.reset_command_timeout(); 
       handleTicDec.reset_command_timeout(); 
//       LOG_INFO("Tic timeout reset");
    }
    catch(const std::exception &error)
    {

       //       LOGF_ERROR("Could not reset Tic timeout: %s", error.what());
    }

    // If you don't call SetTimer, we'll never get called again, until we disconnect
    // and reconnect.
    //SetTimer(POLLMS); // Deprecated, in practice 1 second
    SetTimer(500);   // 500 milliseconds
}


/**************************************************************************************
** Client is asking us to abort our motion
***************************************************************************************/
bool DIYGoTo::Abort()
{
    try 
    {
       handleTicRA.halt_and_hold(); 
       handleTicDec.halt_and_hold(); 
       LOG_INFO("Aborted RA, Dec motion");
       return true;
    }
    catch(const std::exception &error)
    {
       LOGF_ERROR("Could not abort RA, Dec motion: %s", error.what());
       return false;
    }
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

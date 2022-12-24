#pragma once
 
#include "inditelescope.h"
 
// Pololu Tic library
#include <tic.hpp>
#include <iostream>  // Was in the tic code example; may not be needed

class DIYGoTo : public INDI::Telescope
{
public:
    DIYGoTo();

    // Function that gets called when a user enters text
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[],
                           int n) override;
 

protected:
    bool Handshake() override;
    const char *getDefaultName() override;
    // Adding driver properties
    bool initProperties() override;
 
    // Telescope specific functions
    bool ReadScopeStatus() override;
    bool Goto(double, double) override;
    bool Abort() override;
    void TimerHit() override;
 

private:
    double currentRA {0};
    double currentDEC {90};
    double targetRA {0};
    double targetDEC {0};
 
    // Debug channel to write mount logs to
    // Default INDI::Logger debugging/logging channel are Message, Warn, Error, and Debug
    // Since scope information can be _very_ verbose, we create another channel SCOPE specifically
    // for extra debug logs. This way the user can turn it on/off as desired.
    uint8_t DBG_SCOPE { INDI::Logger::DBG_IGNORE };
 
    // slew rate, degrees/s
    static const uint8_t SLEW_RATE = 3;

    
    // Connection to Pololu Tics
    tic::handle open_tic_handle(const char * desired_serial_number);
    bool connectTics();
    bool setupTics();
    bool energizeTics();
    bool deenergizeTics();
    // Tic variables
    tic::handle handleTicRA;
    tic::handle handleTicDec;
    tic::variables varTicRA;
    tic::variables varTicDec;

    // Text properties for Tics
    // RA and Dec Tic id
    // for me, it is "00315372" for RA and "00315338" for Dec
    INDI::PropertyText TicIdRA {1};
    INDI::PropertyText TicIdDec {1};
    // Settings and specs of stepper motor and gear train
    INDI::PropertyText StepsPerRotation {1};
    INDI::PropertyText GearReductionFactor {1};
    INDI::PropertyText MotorMaxCurrent {1};
    INDI::PropertyText Microstepping {1};
    INDI::PropertyText MaxPulseSpeed {1};
    INDI::PropertyText MaxPulseAccelDecel {1};

    // Utility functions: angles, pulses, speeds, etc
    double pulseToAngle(int, std::string);
    int angleToPulse(double, std::string);
    int computePulseSpeedTracking();

};


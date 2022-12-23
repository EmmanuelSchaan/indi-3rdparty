/*******************************************************************************
   Copyright(c) 2011 Gerry Rozema, Jasem Mutlaq. All rights reserved.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
 *******************************************************************************/
  
 #pragma once
  
 #include "defaultdevice.h"
 #include "libastro.h"
 #include <libnova/julian_day.h>
  
 #include <string>
  
 namespace INDI
 {
  
 class Telescope : public DefaultDevice
 {
     public:
         enum TelescopeStatus
         {
             SCOPE_IDLE,
             SCOPE_SLEWING,
             SCOPE_TRACKING,
             SCOPE_PARKING,
             SCOPE_PARKED
         };
         enum TelescopeMotionCommand
         {
             MOTION_START = 0,
             MOTION_STOP
         };
         enum TelescopeSlewRate
         {
             SLEW_GUIDE,
             SLEW_CENTERING,
             SLEW_FIND,
             SLEW_MAX
         };
         enum TelescopeTrackMode
         {
             TRACK_SIDEREAL,
             TRACK_SOLAR,
             TRACK_LUNAR,
             TRACK_CUSTOM
         };
         enum TelescopeTrackState
         {
             TRACK_ON,
             TRACK_OFF,
             TRACK_UNKNOWN
         };
         enum TelescopeParkData
         {
             PARK_NONE,
             PARK_RA_DEC,
             PARK_HA_DEC,
             PARK_AZ_ALT,
             PARK_RA_DEC_ENCODER,
             PARK_AZ_ALT_ENCODER
         };
         enum TelescopeLocation
         {
             LOCATION_LATITUDE,
             LOCATION_LONGITUDE,
             LOCATION_ELEVATION
         };
         enum TelescopePierSide
         {
             PIER_UNKNOWN = -1,
             PIER_WEST    = 0,
             PIER_EAST    = 1
         };
  
         enum TelescopePECState
         {
             PEC_UNKNOWN = -1,
             PEC_OFF     = 0,
             PEC_ON      = 1
         };
  
         enum DomeLockingPolicy
         {
             DOME_IGNORED,       
             DOME_LOCKS,         
         };
  
         enum
         {
             CONNECTION_NONE   = 1 << 0, 
             CONNECTION_SERIAL = 1 << 1, 
             CONNECTION_TCP    = 1 << 2  
         } TelescopeConnection;
  
         enum
         {
             TELESCOPE_CAN_GOTO                    = 1 << 0,  
             TELESCOPE_CAN_SYNC                    = 1 << 1,  
             TELESCOPE_CAN_PARK                    = 1 << 2,  
             TELESCOPE_CAN_ABORT                   = 1 << 3,  
             TELESCOPE_HAS_TIME                    = 1 << 4,  
             TELESCOPE_HAS_LOCATION                = 1 << 5,  
             TELESCOPE_HAS_PIER_SIDE               = 1 << 6,  
             TELESCOPE_HAS_PEC                     = 1 << 7,  
             TELESCOPE_HAS_TRACK_MODE              = 1 << 8,  
             TELESCOPE_CAN_CONTROL_TRACK           = 1 << 9,  
             TELESCOPE_HAS_TRACK_RATE              = 1 << 10, 
             TELESCOPE_HAS_PIER_SIDE_SIMULATION    = 1 << 11, 
             TELESCOPE_CAN_TRACK_SATELLITE         = 1 << 12, 
         } TelescopeCapability;
  
         Telescope();
         virtual ~Telescope();
  
         virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
         virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;
         virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
         virtual void ISGetProperties(const char *dev) override;
         virtual bool ISSnoopDevice(XMLEle *root) override;
  
         uint32_t GetTelescopeCapability() const
         {
             return capability;
         }
  
         void SetTelescopeCapability(uint32_t cap, uint8_t slewRateCount = 0);
  
         bool CanGOTO()
         {
             return capability & TELESCOPE_CAN_GOTO;
         }
  
         bool CanSync()
         {
             return capability & TELESCOPE_CAN_SYNC;
         }
  
         bool CanAbort()
         {
             return capability & TELESCOPE_CAN_ABORT;
         }
  
         bool CanPark()
         {
             return capability & TELESCOPE_CAN_PARK;
         }
  
         bool CanControlTrack()
         {
             return capability & TELESCOPE_CAN_CONTROL_TRACK;
         }
  
         bool HasTime()
         {
             return capability & TELESCOPE_HAS_TIME;
         }
  
         bool HasLocation()
         {
             return capability & TELESCOPE_HAS_LOCATION;
         }
  
         bool HasPierSide()
         {
             return capability & TELESCOPE_HAS_PIER_SIDE;
         }
  
         bool HasPierSideSimulation()
         {
             return capability & TELESCOPE_HAS_PIER_SIDE_SIMULATION;
         }
         bool CanTrackSatellite()
         {
             return capability & TELESCOPE_CAN_TRACK_SATELLITE;
         }
         bool HasPECState()
         {
             return capability & TELESCOPE_HAS_PEC;
         }
  
         bool HasTrackMode()
         {
             return capability & TELESCOPE_HAS_TRACK_MODE;
         }
  
         bool HasTrackRate()
         {
             return capability & TELESCOPE_HAS_TRACK_RATE;
         }
  
         virtual bool initProperties() override;
         virtual bool updateProperties() override;
  
         virtual bool Handshake();
  
         virtual void TimerHit() override;
  
         void SetParkDataType(TelescopeParkData type);
  
         bool InitPark();
  
         bool isParked();
  
         virtual void SetParked(bool isparked);
  
         double GetAxis1Park() const;
  
         double GetAxis1ParkDefault() const;
  
         double GetAxis2Park() const;
  
         double GetAxis2ParkDefault() const;
  
         void SetAxis1Park(double value);
  
         void SetAxis1ParkDefault(double steps);
  
         void SetAxis2Park(double steps);
  
         void SetAxis2ParkDefault(double steps);
  
         bool isLocked() const;
  
         // Joystick helpers
         static void joystickHelper(const char *joystick_n, double mag, double angle, void *context);
         static void axisHelper(const char *axis_n, double value, void *context);
         static void buttonHelper(const char *button_n, ISState state, void *context);
  
         void setTelescopeConnection(const uint8_t &value);
  
         uint8_t getTelescopeConnection() const;
  
         void setPierSide(TelescopePierSide side);
         TelescopePierSide getPierSide()
         {
             return currentPierSide;
         }
  
         void setPECState(TelescopePECState state);
         TelescopePECState getPECState()
         {
             return currentPECState;
         }
  
     protected:
         virtual bool saveConfigItems(FILE *fp) override;
  
         void NewRaDec(double ra, double dec);
  
         virtual bool ReadScopeStatus() = 0;
  
         virtual bool Goto(double ra, double dec);
  
         virtual bool Sync(double ra, double dec);
  
         virtual bool MoveNS(INDI_DIR_NS dir, TelescopeMotionCommand command);
  
         virtual bool MoveWE(INDI_DIR_WE dir, TelescopeMotionCommand command);
  
         virtual bool Park();
  
         virtual bool UnPark();
  
         virtual bool Abort();
  
         virtual bool SetTrackMode(uint8_t mode);
  
         virtual bool SetTrackRate(double raRate, double deRate);
  
         virtual int AddTrackMode(const char *name, const char *label, bool isDefault = false);
  
         virtual bool SetTrackEnabled(bool enabled);
  
         virtual bool updateTime(ln_date *utc, double utc_offset);
  
         virtual bool updateLocation(double latitude, double longitude, double elevation);
  
         void updateObserverLocation(double latitude, double longitude, double elevation);
  
         virtual bool SetParkPosition(double Axis1Value, double Axis2Value);
  
         virtual bool SetCurrentPark();
  
         virtual bool SetDefaultPark();
  
  
         virtual void SyncParkStatus(bool isparked);
  
         virtual bool SetSlewRate(int index);
  
         bool callHandshake();
  
         // Joystick
         void processNSWE(double mag, double angle);
         void processJoystick(const char *joystick_n, double mag, double angle);
         void processAxis(const char *axis_n, double value);
         void processSlewPresets(double mag, double angle);
         void processButton(const char *button_n, ISState state);
  
         TelescopePierSide expectedPierSide(double ra);
  
         // Geographic Location
         IGeographicCoordinates m_Location { 0, 0, 0 };
  
         bool LoadScopeConfig();
  
         bool HasDefaultScopeConfig();
  
         bool UpdateScopeConfig();
  
         std::string GetHomeDirectory() const;
  
         int GetScopeConfigIndex() const;
  
         bool CheckFile(const std::string &file_name, bool writable) const;
  
         TelescopeStatus TrackState {SCOPE_IDLE};
  
         TelescopeStatus RememberTrackState {SCOPE_IDLE};
  
         // All telescopes should produce equatorial co-ordinates
         INumberVectorProperty EqNP;
         INumber EqN[2];
  
         // When a goto is issued, domes will snoop the target property
         // to start moving the dome when a telescope moves
         INumberVectorProperty TargetNP;
         INumber TargetN[2];
  
         // Abort motion
         ISwitchVectorProperty AbortSP;
         ISwitch AbortS[1];
  
         // On a coord_set message, sync, or slew
         ISwitchVectorProperty CoordSP;
         ISwitch CoordS[3];
  
         // A number vector that stores lattitude and longitude
         INumberVectorProperty LocationNP;
         INumber LocationN[3];
  
         // A Switch in the client interface to park the scope
         ISwitchVectorProperty ParkSP;
         ISwitch ParkS[2];
  
         // Custom parking position
         INumber ParkPositionN[2];
         INumberVectorProperty ParkPositionNP;
  
         // Custom parking options
         ISwitch ParkOptionS[4];
         ISwitchVectorProperty ParkOptionSP;
         enum
         {
             PARK_CURRENT,
             PARK_DEFAULT,
             PARK_WRITE_DATA,
             PARK_PURGE_DATA,
         };
  
         // A switch for North/South motion
         ISwitch MovementNSS[2];
         ISwitchVectorProperty MovementNSSP;
  
         // A switch for West/East motion
         ISwitch MovementWES[2];
         ISwitchVectorProperty MovementWESP;
  
         // Slew Rate
         ISwitchVectorProperty SlewRateSP;
         ISwitch *SlewRateS {nullptr};
  
         // Telescope & guider aperture and focal length
         INumber ScopeParametersN[4];
         INumberVectorProperty ScopeParametersNP;
  
         // UTC and UTC Offset
         IText TimeT[2] {};
         ITextVectorProperty TimeTP;
         void sendTimeFromSystem();
  
         // Active GPS/Dome device to snoop
         ITextVectorProperty ActiveDeviceTP;
         IText ActiveDeviceT[2] {};
  
         // Switch to lock if dome is closed.
         ISwitchVectorProperty DomePolicySP;
         ISwitch DomePolicyS[2];
  
         // Switch for choosing between motion control by 4-way joystick or two seperate axes
         ISwitchVectorProperty MotionControlModeTP;
         ISwitch MotionControlModeT[2];
         enum
         {
             MOTION_CONTROL_JOYSTICK,
             MOTION_CONTROL_AXES
         };
  
         // Lock Joystick Axis to one direciton only
         ISwitch LockAxisS[2];
         ISwitchVectorProperty LockAxisSP;
  
         // Pier Side
         ISwitch PierSideS[2];
         ISwitchVectorProperty PierSideSP;
  
         // Pier Side Simulation
         ISwitchVectorProperty SimulatePierSideSP;
         ISwitch SimulatePierSideS[2];
         bool getSimulatePierSide() const;
         void setSimulatePierSide(bool value);
  
         // Pier Side
         TelescopePierSide lastPierSide, currentPierSide;
  
         const char * getPierSideStr(TelescopePierSide ps);
  
         // Satellite tracking
         ITextVectorProperty TLEtoTrackTP;
         IText TLEtoTrackT[1] {};
         enum
         {
             SAT_PASS_WINDOW_START, 
             SAT_PASS_WINDOW_END, 
             SAT_PASS_WINDOW_COUNT 
         } SatelliteWindow;
         ITextVectorProperty SatPassWindowTP;
         IText SatPassWindowT[SAT_PASS_WINDOW_COUNT] {};
         enum
         {
             SAT_TRACK, 
             SAT_HALT, 
             SAT_TRACK_COUNT 
         } SatelliteTracking;
         ISwitchVectorProperty TrackSatSP;
         ISwitch TrackSatS[SAT_TRACK_COUNT];
  
         // PEC State
         ISwitch PECStateS[2];
         ISwitchVectorProperty PECStateSP;
  
         // Track Mode
         ISwitchVectorProperty TrackModeSP;
         ISwitch *TrackModeS { nullptr };
  
         // Track State
         ISwitchVectorProperty TrackStateSP;
         ISwitch TrackStateS[2];
  
         // Track Rate
         INumberVectorProperty TrackRateNP;
         INumber TrackRateN[2];
  
         // PEC State
         TelescopePECState lastPECState {PEC_UNKNOWN}, currentPECState {PEC_UNKNOWN};
  
         uint32_t capability {0};
         int last_we_motion {-1}, last_ns_motion {-1};
  
         //Park
         const char *LoadParkData();
         bool WriteParkData();
         bool PurgeParkData();
  
         int PortFD                           = -1;
         Connection::Serial *serialConnection = nullptr;
         Connection::TCP *tcpConnection       = nullptr;
  
         // XML node names for scope config
         const std::string ScopeConfigRootXmlNode { "scopeconfig" };
         const std::string ScopeConfigDeviceXmlNode { "device" };
         const std::string ScopeConfigNameXmlNode { "name" };
         const std::string ScopeConfigScopeFocXmlNode { "scopefoc" };
         const std::string ScopeConfigScopeApXmlNode { "scopeap" };
         const std::string ScopeConfigGScopeFocXmlNode { "gscopefoc" };
         const std::string ScopeConfigGScopeApXmlNode { "gscopeap" };
         const std::string ScopeConfigLabelApXmlNode { "label" };
  
         // A switch to apply custom aperture/focal length config
         enum
         {
             SCOPE_CONFIG1,
             SCOPE_CONFIG2,
             SCOPE_CONFIG3,
             SCOPE_CONFIG4,
             SCOPE_CONFIG5,
             SCOPE_CONFIG6
         };
         ISwitch ScopeConfigs[6];
         ISwitchVectorProperty ScopeConfigsSP;
  
         // Scope config name
         ITextVectorProperty ScopeConfigNameTP;
         IText ScopeConfigNameT[1] {};
  
         const std::string ScopeConfigFileName;
  
         bool IsParked {false};
         TelescopeParkData parkDataType {PARK_NONE};
  
     private:
         bool processTimeInfo(const char *utc, const char *offset);
         bool processLocationInfo(double latitude, double longitude, double elevation);
         void triggerSnoop(const char *driverName, const char *propertyName);
  
         const char *LoadParkXML();
  
         bool IsLocked {true};
         const char *ParkDeviceName {nullptr};
         const std::string ParkDataFileName;
         XMLEle *ParkdataXmlRoot {nullptr}, *ParkdeviceXml {nullptr}, *ParkstatusXml {nullptr}, *ParkpositionXml {nullptr},
                *ParkpositionAxis1Xml {nullptr}, *ParkpositionAxis2Xml {nullptr};
  
         double Axis1ParkPosition {0};
         double Axis1DefaultParkPosition {0};
         double Axis2ParkPosition {0};
         double Axis2DefaultParkPosition {0};
  
         uint8_t nSlewRate {0};
         IPState lastEqState { IPS_IDLE };
  
         uint8_t telescopeConnection = (CONNECTION_SERIAL | CONNECTION_TCP);
  
         Controller *controller {nullptr};
  
         float motionDirNSValue {0};
         float motionDirWEValue {0};
  
         bool m_simulatePierSide;    // use setSimulatePierSide and getSimulatePierSide for public access
 };
  
 }

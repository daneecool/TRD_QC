#ifndef AMORE_TRD_PUBLISHERTESTMODULE_H
#define AMORE_TRD_PUBLISHERTESTMODULE_H

#include <MonitorObject.h>
#include <PublisherModule.h>
#include <TestModuleCommon.h>
#include "TImage.h"
#include "TText.h"
#include "TString.h"
#include <ctime>

#include "../common/GeoHelper.h"

#include "AliTRDrawStream.h"
#include "AliTRDdigitsManager.h"

class TClonesArray;

namespace amore {

namespace TRD {

namespace publisher {

class TestModule : public amore::publisher::PublisherModule, public amore::TRD::common::TestModuleCommon { // PublisherModule inheritance mandatory, TestModuleCommon inheritance optional for sharing the MonitorObject declarations with other modules

 public:
 
 TestModule();
 ~TestModule();
 virtual void BookMonitorObjects();
 virtual void StartOfCycle();
 virtual void EndOfCycle();
 virtual void StartOfRun(const amore::core::Run& run);
 virtual void EndOfRun(const amore::core::Run& run);
 virtual std::string Version(); // the version of the module
 virtual TImage* GetSummaryImage();

 protected:

 // #### Global vars
 Double_t                         fTimeWarningThreshold;
 Int_t                            fMaxAllowedVolumePerSector;
 Int_t                            fMaxExpectedVolumePerSector;
 unsigned int                     fRunNumber;
 TString                          fRunType;
 time_t                           fRunStartTime;
 amore::TRD::common::GeoHelper*   fGeoHelper;
 TString                          fProblematicChambersStr;
 char                             fDIMproblematicChambers[10800]; // the size is chosen to fit all chambers in it 10*18*5*6*2 characters
 vector<string>                   fTriggerClassNames;
 ULong64_t                        fTriggerMask;
 Int_t                            fDriftRegionStart;
 Int_t                            fDriftRegionEnd;
 TString                          fTriggerForMainPHandDV;
 TString                          fTriggerForMainPHandDVTechnical;
 Int_t                            fMeanAmplitudeMinThreshold;
 Int_t                            fMeanAmplitudeMaxThreshold;

 // #### Data validity flags
 Bool_t                           fIsProblematicPHPlot;
 Bool_t                           fIsProblematicMeanAmplitude;
 Bool_t                           fIsProblematicChambers;

 // #### Variables for LMEs
 Double_t                         fChambersWithLMEs[18][5][6][2];
 Double_t                         fChambersWithLessData[18][5][6][2];
 Double_t                         fLMEThresholdForDIM;
 TString                          fProblematicChambersTemporary;

 TString                          fProblematicChambersFED;
 TString                          fProblematicChambersHV;

 // #### Raw data and reader
 AliTRDrawStream*                 fTRDrawStream;
 AliTRDdigitsManager*             fDigMgr;
 TClonesArray*                    fMarkers;
 TClonesArray*                    fTracklets;
 TClonesArray*                    fGtuTrackArray;

 // #### Counters
 Int_t                            fNumberCycles;
 Int_t                            fNumberEventsInRun;
 Int_t                            fNumberOfMarkersInCycle;
 Int_t                            fNumberEventsInCycle;
 Int_t                            fNumberOfSendingHCsInCycle;
 Int_t                            fNumberOfFiredADCsInCycle;
 Int_t                            fNumberOfSignalADCsInCycle;
 Int_t                            fNumberOfTrackletsInCycle;
 Int_t                            fNumberProblematicChambers;
 Int_t                            fNoisySectorEventsInRun;
 Double_t                         fRunsizePerSector[18];
 
 TH1D*                            fClsAmpChamber[540];

 // #### List of trigger classes
 TObjArray*                       fTriggerListMB;
 TObjArray*                       fTriggerListHM;
 TObjArray*                       fTriggerListL1;

 // #### Event properties
 Bool_t                           fFiredTriggerMB;
 Bool_t                           fFiredTriggerHM;
 Bool_t                           fFiredTriggerL1Jet;

 // #### Values to send to DIM server
 Double_t                         fDriftVelocity;
 Double_t                         fGain;


 // #### Drawing functions
 void                             ClearSpecialMarkers();
 void                             DrawSpecialMarkers();
 void                             DrawWarnings();

 // #### Validity functions
 void                             CheckLMEs();
 void                             CheckDataValidity();

 // #### Helper functions
 void                             GetTriggerClasses();
 Double_t                         GetMinutesSinceSOR();
 Bool_t                           IsPhysicsRun();
 Bool_t                           IsStandaloneRun();
 void                             PrintDebug(const char* message)
 {
#ifdef DEBUG_MODE
std::cout << "====================================================== "; std::cout << "I-TRDAmore: " << message << std::endl;
#endif
 }

 Int_t                            FillBits(Int_t code, Int_t offset); 
 void                             UpdateTrend(amore::core::MonitorObjectH1D *hist, Double_t value);
 void                             CalculateValuesHV();
 void                             FitPHPlot(amore::core::MonitorObjectH1D* plotPH);
 static Double_t                  PH(const Double_t *x, const Double_t *par);
 static Double_t                  AsymmGauss(const Double_t *x, const Double_t *par);

 Bool_t                           IsTrigger(const char* subString);
 Bool_t                           IsTriggerTRDL1Jet();
 Bool_t                           IsTriggerHighMult();
 Bool_t                           IsTriggerMinBias();
 Bool_t                           IsChamberBlacklisted(Int_t sector, Int_t stack, Int_t layer, Bool_t isB);
 Bool_t                           IsChamberProblematicForHV(Int_t sector, Int_t stack, Int_t layer);
 void                             SetProblematicChambers();

 void                             ReadConfiguration();

 const static Int_t nMCMsPerChamber = 128;
 const static Int_t nMCMsperRob = 16;
 
 const static Int_t nSMs = 18;
 const static Int_t nChambers = 30;
 const static Int_t nADCs = 1024;
 const static Int_t nTimeBins = 30;
 
 const static Int_t nMCMs = 3840;
 const static Int_t nErrorCodes = 1000;

 const static Int_t kAmplificationPeakBin = 3;

 public:
 ClassDef(TestModule, 1);

};

};

};

};

#endif
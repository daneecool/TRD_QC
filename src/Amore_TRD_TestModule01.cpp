#include "TROOT.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TPaveText.h"

#include <AliTRDrawStream.h>
#include <AliTRDtrackletWord.h>
#include <AliESDTrdTrack.h>

#include <sstream>
#include <TRandom.h>
#include <Event.h>
#include <AliRawReaderDate.h>
#include <AliRawReader.h>
#include <AliTRDfeeParam.h>
#include <AliTRDdigitsManager.h>
#include <AliTRDarrayADC.h>
#include <AliTRDSignalIndex.h>
#include <AliTRDgeometry.h>
#include <AliMathBase.h>
#include <iostream>
#include <iomanip>

#include "TCanvas.h"
#include "TText.h"
#include "TLine.h"
#include "TMarker.h"
#include "TF1.h"

#include "TGaxis.h"
#include <LogBook.h>

#include "TestModule.h"

namespace amore {

namespace TRD {

namespace publisher {


void TestModule::BookMonitorObjects() {
// ##########
// ########## Publish monitoring objects (histograms)
// ##########
  PrintDebug("Processing BookMonitorObjects()");

  // local histos
  for(Int_t det = 0; det<540; det++) {
    fClsAmpChamber[det] = new TH1D(Form("clsAmp%d", det), "", 300, -0.5, 299.5);   
  }
  
  // Publish string with problematic chambers via DIM
  gPublisher->GetDimInterface()->Send("LMEs", (char*)(&fDIMproblematicChambers));
  // Publish HV values via DIM
  gPublisher->GetDimInterface()->Send("DriftVelocity", (Double_t&)fDriftVelocity);
  gPublisher->GetDimInterface()->Send("Gain", (Double_t&)fGain);


  // ############ AMORE UI tab: EC

  // Data volume per event and noise string
  Publish(moDataVolume, "DataVolume", "DataVolume", nSMs, -0.5, 17.5, 4000, 0, fMaxAllowedVolumePerSector);
  Publish(moMeanDV, "MeanDV", "MeanDV", nSMs, -0.5, 17.5);  
  moDataVolume->SetTitle("Data volume per sector;Sector;Volume (kB/event)");
  moDataVolume->GetYaxis()->SetRangeUser(0, fMaxExpectedVolumePerSector);
  moDataVolume->SetDescription("This plot shows the data volume distribution per sector. Supermodules 4, 5, and 14 show on average higher values.");

  Publish(moTrgClass, "TriggerClass", "TriggerClass", 65, -1.5, 63.5);
  moTrgClass->SetTitle("Trigger class;Trigger class;Counts");

  Publish(moNCls, "NCls", "NCls", nSMs, -0.5, 17.5);
  moNCls->SetTitle("Total number of clusters per sector;Sector;Counts");


  // ############ AMORE UI tab: Detector view

  Publish(moHCHeader, "HCHeader", "HCHeader", 36, -0.5, 17.5, 30, -0.5, 29.5);
  moHCHeader->SetTitle("Half-chambers sending data;Sector;Stack_Layer");
  moHCHeader->SetDescription("This plot shows which chambers are sending data. The plot should be uniformly colored. Differently colored pixels mean that the corresponding half-chambers have at least partially not sent out data.");

  Publish(moHCMOErrors, "HCMOErrors","HCMOErrors", 36, -0.5, 17.5, 30, -0.5, 29.5);  
  moHCMOErrors->SetTitle("Link monitor errors;Sector;Stack_Layer");
  moHCMOErrors->SetDescription("This plot shows which chambers with link monitor errors. Those errors appear if the chambers are for some reason not correctly or in time sending out the data. The box on the right indicates the problematic chambers.");

  Publish(moHCDecodeErrors, "HCDecodeErrors","HCDecodeErrors", 36, -0.5, 17.5, 30, -0.5, 29.5);   
  moHCDecodeErrors->SetTitle("Half-chambers with decoding errors;Sector;Stack_Layer");

  Publish(moHCADCs,"HCADCs", "HCADCs",36, -0.5, 17.5, 30, -0.5, 29.5);  
  moHCADCs->SetTitle("Number of ADCs channels in half-chamber;Sector;Stack_Layer");

  Publish(moADCvalue, "ADCvalue", "ADCvalue", 100, -0.5, 99.5);
  moADCvalue->SetTitle("ADC value distribution;ADC value;Counts");

  Publish(moErrorCode, "ErrorCode", "ErrorCode", 31, -0.5, 30.5);
  moErrorCode->SetTitle("Error codes;Code;Counts");


  // ############ AMORE UI tab: Trending

  Publish(moTrHCHeader, "TrHCHeader", "TrHCHeader", 100, -0.5, 99.5);
  moTrHCHeader->SetTitle("Number of connected half-chambers;Monitoring cycle;# HC");
  Publish(moTrHCMOErrors, "TrHCMOErrors", "TrHCMOErrors", 100, -0.5, 99.5);

  moTrHCMOErrors->SetTitle("Number of half-chambers with monitor errors;Monitoring cycle;# errors");
  Publish(moTrHCDecodeErrors, "TrHCDecodeErrors", "TrHcDecodeErrors", 100, -0.5, 99.5);
  moTrHCDecodeErrors->SetTitle("Number of half-chambers with decoding errors;Monitoring cycle;# errors");

  Publish(moTrHCADCs, "TrHCADCs", "TrHCADCs", 100, -0.5, 99.5);
  moTrHCADCs->SetTitle("Number of fired ADCs;Monitoring cycle;# channels");

  Publish(moTrHCSignalADCs, "TrHCSignalADCs", "TrHCSignalADCs", 100, -0.5, 99.5);
  moTrHCSignalADCs->SetTitle("Number of signal ADCs (above > 13);Monitoring cycle;# pads");

  Publish(moTrADCvalue, "TrADCvalue", "TrADCvalue", 100, -0.5, 99.5);
  moTrADCvalue->SetTitle("Mean ADC signal;Monitoring cycle;ADC value");


  // ############ AMORE UI tab: Event display

  Publish(moTrackGraph, "TrackGraph", "TrackGraph");

  Publish(moTrackHist, "TrackHist", "TrackHist", 144, -0.5, 143.5, 180, -0.5, 179.5);
  moTrackHist->SetTitle("Tracks, all sectors together;Pad;Global timebin");


  // ############ AMORE UI tab: Clusters

  Publish(moClsAmp, "ClsAmp", "ClsAmp", 200, -0.5, 1999.5);
  moClsAmp->SetTitle("Amplitude of clusters;Amplitude (ADC);Counts");

  Publish(moClsAmpDrift, "ClsAmpDrift", "ClsAmpDrift", 500, -0.5, 999.5);
  moClsAmpDrift->SetTitle("Amplitude of clusters in drift region;Amplitude (ADC);Counts");

  Publish(moClsTb, "ClsTb", "ClsTb", 30, -0.5, 29.5, 200, 0, 2000);
  moClsTb->SetTitle("Cluster amplitude vs. timebin;Timebin;Amplitude");

  Publish(moClsAmpTb, "ClsAmpTb", "ClsAmpTb", 30, -0.5, 29.5);

  Publish(moClsSector, "ClsSector", "ClsSector", nSMs, -0.5, 17.5, 500, -0.5, 999.5);
  moClsSector->SetTitle("Cluster amplitude per sector;Sector;Amplitude");

  Publish(moClsStack, "ClsStack", "ClsStack", 5, -0.5, 4.5, 500, -0.5, 999.5);
  moClsStack->SetTitle("Cluster amplitude per stack;Stack;Amplitude");


  // ############ AMORE UI tab: Illumination

  for(Int_t layer = 0; layer<6; layer++) {
    Publish(moNClsLayer[layer], Form("NClsLayer%d", layer), Form("NClsLayer%d", layer), 18*8, -0.5, 17.5, 76, -0.5, 75.5);
    moNClsLayer[layer]->SetTitle(Form("Illumination layer %d;Sectors;Padrows (z)", layer));
  }


  // ############ AMORE UI tab: Gain

  for(Int_t iSM=0; iSM < nSMs; iSM++) {
    Publish(moClsDetAmp[iSM], Form("ClsDetAmp%d", iSM), Form("ClsDetAmp%d", iSM), 30, -0.5, 29.5, 500, -0.5, 999.5);
    moClsDetAmp[iSM]->SetTitle(Form("Cluster amplitude chamberwise in SM%0d;Chamber;Amplitude", iSM));
  }


  // ############ AMORE UI tab: Drift

  for(Int_t iSM=0; iSM < nSMs; iSM++) {
    Publish(moClsDetTime[iSM], Form("ClsDetTime%d", iSM), Form("ClsDetTime%d", iSM), 30, -0.5, 29.5, 30, -0.5, 29.5);
    moClsDetTime[iSM]->SetTitle(Form("Cluster amplitude chamberwise in SM%0d;Chamber;Timebin", iSM));
  }


  // ############ AMORE UI tab: Triggers

  for(Int_t trgg=0; trgg<7; trgg++) {
    Publish(moClsChargeTbTigg[trgg], Form("ClsChargeTbTrgg%d", trgg), Form("ClsChargeTbTrgg%d", trgg), 30, -0.5, 29.5);
    if (trgg == 6)
    {
      moClsChargeTbTigg[trgg]->SetTitle("Total charge vs. time bin;time bin;total charge");
      moClsChargeTbTigg[trgg]->SetDescription("This plot shows the pulse height distribution. It should have a peak between the two indicated lines. If not, you will receive a warning");
    }
    else
      moClsChargeTbTigg[trgg]->SetTitle(Form("Total charge vs. timebin, trigger class %d;Timebin;Total charge", trgg));
  }


  // ############ AMORE UI tab: Tracklets

  Publish(moTrkNumber, "TrkNumber", "TrkNumber", 36, -0.5, 17.5, 30, -0.5, 29.5);
  moTrkNumber->SetTitle("Half-chambers sending tracklets;Sector;Stack_Layer");

  Publish(moTrkY, "TrkY", "TrkY", 200, -100, 100);
  moTrkY->SetTitle("Position of tracklets;y (cm);# tracklets");

  Publish(moTrkDY, "TrkDY", "TrkDY", 129, -64.5, 64.5);
  moTrkDY->SetTitle("Deflection of tracklets;dXdY bin;# tracklets");

  Publish(moTrkNCh, "TrkNCh", "TrkNCh", 50, -0.5, 49.5);
  moTrkNCh->SetTitle("Number of tracklets per HC;Number of tracklets;Counts");

  Publish(moClsChargeFirst, "ClsChargeFirst", "ClsChargeFirst", 100, 0, 1000, 101, -0.2, 0.2);
  moClsChargeFirst->SetTitle(";Timebin of the maximum signal;Timebin");
//TODO: no title set
  Publish(moClsNTb, "ClsNTb",  "ClsNTb", 30, -0.5, 29.5);
  moClsNTb->SetTitle("Number of clusters;Timebin");


  // ############ AMORE UI tab: DQM

  // All other MOs necessary for this tab are already published

  Publish(moClsAmpCh, "ClsAmpCh", "ClsAmpCh", 100, 25, 125);
  moClsAmpCh->SetTitle("Reconstructed mean amplitude;Amplitude (ADC);# chambers");
  moClsAmpCh->SetDescription("This plot shows the mean amplitude. The peak should be within the indicated lines (some outliers are OK, especially during COSMICS the distribution is much broader).");


  // ############ AMORE UI tab: Drift velocity

  Publish(moDriftVelocity, "DriftVelocity", "DriftVelocity", 1000, -0.5, 999.5);
  moDriftVelocity->SetTitle("Drift velocity trend;Cycle;cm per #mu#it{s}");
//  moDriftVelocity->SetMinimum(10);
//  moDriftVelocity->SetMaximum(30);
  moDriftVelocity->SetLineWidth(2);
  Publish(moGain, "Gain", "Gain", 1000, -0.5, 999.5);
  moGain->SetTitle("Total charge trend;Cycle;Total charge");
  moGain->SetLineWidth(2);

  Publish(moTrTracklets, "TrTracklets", "TrTracklets", 1000, -0.5, 999.5);
  moTrTracklets->SetTitle("Number of tracklets per event;Cycle");


  // ############ AMORE UI tab: Trigger class

  Publish(moDataVolumeTrigHM, "DataVolumeTrigHM", "DataVolumeTrigHM", nSMs, -0.5, 17.5, 4000, 0, fMaxAllowedVolumePerSector);
  moDataVolumeTrigHM->SetTitle("Data volume for HM trigger;Sector;Volume (kB/event)");
  moDataVolumeTrigHM->GetYaxis()->SetRangeUser(0, fMaxExpectedVolumePerSector);

  Publish(moDataVolumeTrigMinBias, "DataVolumeTrigMinBias", "DataVolumeTrigMinBias", nSMs, -0.5, 17.5, 4000, 0, fMaxAllowedVolumePerSector);
  moDataVolumeTrigMinBias->SetTitle("Data volume for MB trigger;Sector;Volume (kB/event)");
  moDataVolumeTrigMinBias->GetYaxis()->SetRangeUser(0, fMaxExpectedVolumePerSector);

  Publish(moDataVolumeTrigTRDL1, "DataVolumeTrigTRDL1", "DataVolumeTrigTRDL1", nSMs, -0.5, 17.5, 4000, 0, fMaxAllowedVolumePerSector);
  moDataVolumeTrigTRDL1->SetTitle("Data volume for L1 jet trigger;Sector;Volume (kB/event)");
  moDataVolumeTrigTRDL1->GetYaxis()->SetRangeUser(0, fMaxExpectedVolumePerSector);

  Publish(moClsChargeTbTrigHM, "ClsChargeTbTrigHM", "ClsChargeTbTrigHM", 30, -0.5, 29.5);
  moClsChargeTbTrigHM->SetTitle("Total charge vs. timeBin for HM trigger;Timebin;Total charge");

  Publish(moClsChargeTbTrigMinBias, "ClsChargeTbTrigMinBias", "ClsChargeTbTrigMinBias", 30, -0.5, 29.5);
  moClsChargeTbTrigMinBias->SetTitle("Total charge vs. timebin for MB trigger;Timebin;Total charge");

  Publish(moClsChargeTbTrigTRDL1, "ClsChargeTbTrigTRDL1", "ClsChargeTbTrigTRDL1", 30, -0.5, 29.5);
  moClsChargeTbTrigTRDL1->SetTitle("Total charge vs. timebin for L1 jet trigger;Timebin;Total charge");

  // ############ AMORE UI tab: Tracklets

  Publish(moTrackletY, "TrackletY","TrackletY", 120, -60, 60);
  moTrackletY->SetTitle("y position of the tracklets;y (cm)");

  Publish(moTrackletDy, "TrackletDy","TrackletDy", 129, -64.5, 64.5); 
  moTrackletDy->SetTitle("Deflection dy of the tracklets;dy (#times 140 #mum)");

  Publish(moTrackletDyY, "TrackletDyY","TrackletDyY", 120, -60, 60, 129, -64.5, 64.5);
  moTrackletDyY->SetTitle("Deflection dy vs. position y of the tracklets;dy (#times 140 #mum);y (cm)");

  Publish(moTrackletZ, "TrackletZ","TrackletZ", 15,0,15);
  moTrackletZ->SetTitle("z position of the tracklets;Padrow");

  Publish(moTrackletHC, "TrackletHC","TrackletHC",nSMs, 0, nSMs, 60, 0, 60);
  moTrackletHC->SetTitle("Number of tracklets in HC;Sector");

  Publish(moTrackletPID, "TrackletPID","TrackletPID",256, 0, 256);
  moTrackletPID->SetTitle("PID");


  // ############ AMORE UI tab: Tracks

  Publish(moTrackNTracklet, "TrackNTracklet","TrackNTracklet", 6, 0.5, 6.5);
  moTrackNTracklet->SetTitle("Number of tracklets per track;Number of tracklets;Yield");

  Publish(moTrackNTrackStack, "TrackNTrackStack","TrackNTrackStack",nSMs , 0, nSMs, 5, 0, 5);
  moTrackNTrackStack->SetTitle("Number of tracks per stack;Sector;Stack");

  Publish(moTrackPtPositive, "TrackPtPositive","TrackPtPositive",160,0,40.);
  moTrackPtPositive->SetTitle("#it{p}_{T} of the track (blue: positive charge, red: negative charge);#it{p}_{T} (GeV/#it{c})");
  Publish(moTrackPtNegative, "TrackPtNegative","TrackPtNegative",160,0,40.);

  Publish(moTrackTimingFirst, "TrackTimingFirst","TrackTimingFirst",160, 0., 8.);
  Publish(moTrackTimingLast, "TrackTimingLast","TrackTimingLast",160, 0., 8.);
  moTrackTimingFirst->SetTitle("Arrival time of first(blue) and last(red) tracklet;Arrival time (#mus)");

  Publish(moTrackPID, "TrackPID","TrackPID",256, 0, 256);
  moTrackPID->SetTitle("PID");


  // ############ AMORE UI tab: Error codes

  // Error codes
  for(Int_t iSM=0; iSM < nSMs; iSM++) {
    Publish(moErrorCodeSM[iSM],Form("ErrorCodesSM%i", iSM),Form("Error codes for SM%i", iSM),20,0,20,30,0,30);
    moErrorCodeSM[iSM]->SetTitle(Form("Error code for SM%i;Error code",iSM));
  }

  
  // ############ Misc or unused

  Publish(moEvSize,"Event_Size","Event_Size",10000,0.,10000.);
  Publish(moEvType,"Event_Type","Event_Type",100,0.,10.);
  Publish(moChvsSMtracklets,"Tracklets","Tracklets",nSMs,0.,nSMs,30,0.,30);
  Publish(moChvsSMpads,"Pads","Pads",nSMs,0.,nSMs,30,0.,30);
  Publish(moHCSignalADCs, "HCSignalADCs", "HCSignalADCs", 36, -0.5, 17.5, 30, -0.5, 29.5);   
  Publish(moTrNErrorCode, "TrNErrorCode", "TrNErrorCode", 100, -0.5, 99.5);
  Publish(moTrgStat[0], "trgStatN", "trgStatN", 200, -0.5, 199.5, 7, -1.5, 5.5);
  Publish(moClsBC, "ClsBC", "ClsBC", 400, -0.5, 3999.5, 30, -0.5, 29.5);
  Publish(moClsChargeTb, "ClsChargeTb",  "ClsChargeTb", 30, -0.5, 29.5);
  Publish(moClsChargeTbCycle, "ClsChargeTbCycle", "ClsChargeTbCycle", 30, -0.5, 29.5);


  // ##################### DQM DATA for every single supermodule
  char buffer[256];
  for(Int_t iSM=0; iSM < nSMs; iSM++) {
    sprintf(buffer, "HCMCM%d", iSM);
    Publish(moHCMCM[iSM], buffer, buffer, 76, -0.5, 75.5, 8*6, -0.5, 8*6-0.5);
    moHCMCM[iSM]->SetTitle("MCM in data stream;Padrow;MCM");

    sprintf(buffer, "ADCTB%d", iSM);
    Publish(moADCTB[iSM], buffer, buffer, 30, -0.5, 29.5, 128, -0.5, 127.5);
    moADCTB[iSM]->SetTitle("Signal in Timebins;Timebin;ADC");
    
    sprintf(buffer, "ADCTBfull%d", iSM);
    Publish(moADCTBfull[iSM], buffer, buffer, 30, -0.5, 29.5, 128, -0.5, 1023.5);
    moADCTBfull[iSM]->SetTitle("Signal in timebins;Timebin;ADC");

    sprintf(buffer, "ADC%d", iSM);
    Publish(moADC[iSM], buffer, buffer, 100, -0.5, 99.5);
    moADC[iSM]->SetTitle("ADC value distribution;ADC value;Counts");

    // clusters
    sprintf(buffer, "ClsSM%d", iSM);
    Publish(moClsSM[iSM], buffer, buffer, 100, 0, 200);

    sprintf(buffer, "ClsTbSM%d", iSM);
    Publish(moClsTbSM[iSM], buffer, buffer, 30, -0.5, 29.5, 10, 0, 200);
  }

  PrintDebug("BookMonitorObjects() done"); 
} 

void TestModule::StartOfCycle() {
// ##########
// ########## Starting one measurement cycle (consists of several events)
// ##########

  PrintDebug("Processing StartOfCycle()");
  
  // reset counters
  fNumberOfMarkersInCycle = 0;
  fNumberOfSendingHCsInCycle = 0;
  fNumberOfFiredADCsInCycle = 0;
  fNumberOfSignalADCsInCycle = 0;
  fNumberEventsInCycle = 0;
  fNumberOfTrackletsInCycle = 0; 

  // reset values
  fDriftVelocity = -1.;
  fGain = -1.;

  // reset histograms, add referene lines
// moHCHeader->Reset();
  moHCHeader->GetZaxis()->SetRange(0, 0);
//  moHCMOErrors->Reset();
  moHCDecodeErrors->Reset();
  moHCADCs->Reset();
  moHCSignalADCs->Reset();
  moADCvalue->Reset();
  moErrorCode->Reset();
  moClsAmpTb->Reset();
  moClsAmpCh->Reset(); 

  for(Int_t iSM=0; iSM<nSMs; iSM++) {

    moHCMCM[iSM]->Reset();
    moHCMCM[iSM]->SetTitle("MCM in data stream;padrow;mcm");

    moADCTB[iSM]->Reset();
    moADCTBfull[iSM]->Reset();
    moADC[iSM]->Reset();
  }

  PrintDebug("StartOfCycle() done.");
}

void TestModule::EndOfCycle() {
// ##########
// ########## Code executed at the end of a monitor cycle
// ##########
  PrintDebug("Processing EndOfCycle()");

  if(fNumberEventsInCycle)
  {
    // fill trending histograms  
    UpdateTrend(moTrHCHeader, 1. * fNumberOfSendingHCsInCycle / fNumberEventsInCycle);
    UpdateTrend(moTrHCMOErrors, 1. * fNumberProblematicChambers);
    UpdateTrend(moTrHCDecodeErrors, 1. * moHCDecodeErrors->GetEntries() / fNumberEventsInCycle);
    UpdateTrend(moTrHCADCs, 1. * fNumberOfFiredADCsInCycle / fNumberEventsInCycle);
    UpdateTrend(moTrHCSignalADCs, 1. * fNumberOfSignalADCsInCycle / fNumberEventsInCycle);
    UpdateTrend(moTrADCvalue, moADCvalue->GetMean());
    UpdateTrend(moTrNErrorCode, 1. * moErrorCode->GetEntries() / fNumberEventsInCycle);
    UpdateTrend(moTrTracklets, 1 * fNumberOfTrackletsInCycle / fNumberEventsInCycle);
  }

  // Change scale of HC sending data histogram
  Double_t maxValue = 0;
  for(Int_t i=0; i<moHCHeader->GetXaxis()->GetNbins(); i++)
    for(Int_t j=0; j<moHCHeader->GetYaxis()->GetNbins(); j++)
    {
      if(moHCHeader->GetBinContent(i+1,j+1) > maxValue)
        maxValue = moHCHeader->GetBinContent(i+1,j+1);
    }
  if(maxValue)
    moHCHeader->GetZaxis()->SetRangeUser(0, maxValue*2.);


  // #### Fill average data histograms
  for(Int_t sm=0; sm<nSMs; sm++) {
    if (fNumberEventsInRun)
      moMeanDV->SetBinContent(sm+1, fRunsizePerSector[sm] / fNumberEventsInRun);
  }

  for(Int_t i=1; i<31; i++) {
    Double_t v = moClsChargeTb->GetBinContent(i);
    Double_t n = moClsNTb->GetBinContent(i);
    if (n > 1)
      moClsAmpTb->SetBinContent(i, v/n);
    else 
      moClsAmpTb->SetBinContent(i, 0);
  }

  for(Int_t det=0; det<540; det++) {
    if (fClsAmpChamber[det]->GetEntries() > 0) 
      moClsAmpCh->Fill(fClsAmpChamber[det]->GetMean());
  }

  // assigna quality flag to plots
  
  //if (moHCHeaders->GetEntries() > 0) q->Error();
  //if (moClsChargeTbTigg->GetEntries() < 181) q->Error()
  //if (moClsChargeTbTigg->Integral() < 181) q->Warning();

  CalculateValuesHV();

  PrintDebug("Data validity checks and cosmetics...");

  CheckDataValidity();
  ClearSpecialMarkers();
  DrawSpecialMarkers();
  DrawWarnings();
  fNumberCycles++;

  PrintDebug("EndOfCycle() done");
}

};

};

};
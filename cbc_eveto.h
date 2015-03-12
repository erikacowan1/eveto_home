#include <iostream>
#include <string>
#include <getopt.h>
#include <stdio.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TTree.h>
#include <TChain.h>
#include <TSQLServer.h>
#include <TSQLStatement.h>
#include <THStack.h>
#include <TFile.h>
#include <TBrowser.h>
#include <TSystem.h>
#include <TCut.h>
#include <TEventList.h>
#include <TROOT.h>


namespace eveto {
  int cbc_eveto_main( 
      Long_t gps_start_time, 
      Long_t gps_end_time, 
      TString* detector, 
      TString* safe_channel_file,
      TString* cbc_trigger_database, 
      Float_t cbc_snr_threshold,
      TString* omicron_trigger_path, 
      Double_t omicron_snr_threshold, 
      Double_t omicron_cluster_window,
      TString* output_directory, 
      Float_t sig_threshold,
      Float_t dumb_veto_window,      
      Int_t max_rounds,
      bool verbose
      );

  int read_omicron_triggers(
      TTree* clustered_veto_trigger_tree[],
      TTree* veto_segment_tree[],
      TTree* safe_channels,
      TString* omicron_trigger_path,
      Double_t omicron_snr_threshold,
      Double_t omicron_cluster_window,
      bool verbose );

  int read_cbc_triggers(
      TTree** cbc_trigger_tree,
      TString* cbc_trigger_database,
      Long_t gps_start_time,
      Long_t gps_end_time,
      TString* detector,
      Float_t cbc_snr_threshold,
      bool verbose );

  int calc_dumb_sig(
      TTree* cbc_trigs_round, 
      TTree* omicron_trigs_round, 
      Float_t dumb_veto_window, 
      bool verbose );

};

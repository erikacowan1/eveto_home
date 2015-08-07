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
#include <string.h>
#include <iomanip>
namespace eveto {
  int cbc_eveto_main( 
      Long_t gps_start_time, 
      Long_t gps_end_time, 
      TString* detector, 
      TString* main_channel,
      TString* safe_channel_file,
      TString* cbc_trigger_database, 
      Float_t cbc_snr_threshold,
      TString* omicron_trigger_path, 
      Double_t omicron_snr_threshold, 
      Double_t omicron_cluster_window,
      TString* cwb_trigger_path,
      Double_t cwb_snr_threshold,
      Double_t cluster_time_window,
      Double_t cluster_snr_threshold, 
      Float_t sig_threshold,
      Float_t dumb_veto_window,      
      Int_t max_rounds,
      TString* output_directory, 
      bool verbose
      );

  int read_omicron_triggers(
      TTree* clustered_veto_trigger_tree[],
      TTree* veto_segment_tree[],
      TTree* safe_channels,
      TString* omicron_trigger_path,
      Double_t omicron_snr_threshold,
      Double_t omicron_cluster_window,
      Long64_t omicron_start_time,
      Long64_t omicron_end_time,
      Double_t cluster_time_window,
      Double_t cluster_snr_threshold,
      bool verbose );

  int read_cbc_triggers(
      TTree** cbc_trigger_tree,
      TString* cbc_trigger_database,
      Long_t gps_start_time,
      Long_t gps_end_time,
      TString* detector,
      Float_t cbc_snr_threshold,
      bool verbose );

  int read_cwb_triggers(
      TTree* cwb_unclustered_tree,
      TChain* input_cwb_chain,
      TString* cwb_trigger_path,
      Long64_t cwb_start_time,
      Long64_t cwb_end_time,
      bool verbose);

  int calc_dumb_sig(
      TTree* main_channel_trigger_tree_ptr,
      TTree* omicron_trigger_tree_ptr,
      TString* main_channel, 
      float dumb_time_seg,
      bool verbose );

  int veto_triggers(
      TTree* omicron_trigs_round,
      bool verbose );

  
  TTree* remove_main_channel_triggers(
        TTree* main_channel_trigger_tree_in_ptr,
        TTree* omicron_trigger_tree_veto_ptr,
        TString* main_channel, 
        //TTree* cbc_segs_tree_ptr,
        //TTree* omicron_segs_tree_ptr,
        bool verbose);

  TTree* remove_omicron_triggers(
      TTree* omicron_trigger_tree_in_ptr,
      TTree* omicron_trigger_tree_veto_ptr,     
      bool verbose );

  int cbc_eveto_analysis(
      TTree* cbc_trigger_tree_ptr,
      TTree* omicron_trigger_tree_ptr,
      bool verbose );

};


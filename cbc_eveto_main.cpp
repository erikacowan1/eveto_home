#include "cbc_eveto.h"

int eveto::cbc_eveto_main( 
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
    bool verbose
    )
{
  int retcode = 0;

  //
  // Read in the veto channel data.
  //
  // For each channel, we get a tree containing the clustered omicron triggers
  // and a tree containing the segments for which vetoes are generated. These
  // are stored in two arrays, each entry in the array is a tree for an input
  // channel.
  //


  // Create a tree containing the list of safe veto channels to be processed
  TTree* safe_channels = new TTree( "safe_channel_list", "List of safe channels to process" );
  Long64_t num_veto_channels = safe_channels->ReadFile( safe_channel_file->Data(), "channel/C" );
  if ( verbose ) {
    std::cout << "Read " << num_veto_channels << " safe channel names" << std::endl;
    safe_channels->Print();
  }

  TTree* clustered_veto_trigger_tree[num_veto_channels];
  TTree* veto_segment_tree[num_veto_channels];

  // Read in the omicron triggers for the interval that we want to process
  retcode = eveto::read_omicron_triggers(
      clustered_veto_trigger_tree, // output
      veto_segment_tree, // output
      safe_channels, // input
      omicron_trigger_path, // input
      omicron_snr_threshold, // input
      omicron_cluster_window, // input
      verbose );

  if ( retcode ) {
    std::cerr << "error reading omicron triggers" << std::endl;
    return 1;
  }

  //
  // Read in cbc triggers from database.
  //

  TTree* cbc_trigger_tree;

  retcode = eveto::read_cbc_triggers(
      &cbc_trigger_tree, // output
      cbc_trigger_database, // input
      gps_start_time, // input
      gps_end_time, // input
      detector, // input
      cbc_snr_threshold, // input
      verbose );

  if ( retcode ) {
    std::cerr << "error reading cbc triggers" << std::endl;
    return 1;
  }

  return 0;
}

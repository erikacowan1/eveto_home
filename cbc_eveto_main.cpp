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

  //
  //Veto Algorithm
  //currently the segments are stored in omicron_triggers.root/segments. For now
  //assume that all segments are science segments. Grab segments from segment tree
  //and perform veto algorithm. 

  for ( Long64_t i = 0; i < safe_channels->GetEntries(); i++ )
  {
    if ( verbose ) {
      clustered_veto_trigger_tree[i]->Print();
      veto_segment_tree[i]->Print();
    }
 
  //increment number of triggers in TTree
  int num_line_trig_tree = clustered_veto_trigger_tree[i]->GetEntries();

    for(Long64_t j=0; j<num_line_trig_tree; ++j){
      clustered_veto_trigger_tree[i]->GetEntry(j);

    //increment number of segments in TTree
    int num_line_seg_tree = veto_segment_tree[i]->GetEntries();

       for (Long64_t k=0; k<num_line_seg_tree; ++k){
         veto_segment_tree[i]->GetEntry(k);
         std::cout << "Read " << veto_segment_tree[i]->GetEntry(k) << "veto segments from TTree" << std::endl;

         double_t Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
         TTree* r1_clustered_tree;

         //make r1_clustered_tree
         r1_clustered_tree->Branch("time",       &Ctime,      "time/D");
         r1_clustered_tree->Branch("tstart",     &Ctstart,    "tstart/D");
         r1_clustered_tree->Branch("tend",       &Ctend,      "tend/D");
         r1_clustered_tree->Branch("frequency",  &Cfreq,      "frequency/D");
         r1_clustered_tree->Branch("fstart",     &Cfstart,    "fstart/D");
         r1_clustered_tree->Branch("fend",       &Cfend,      "fend/D");
         r1_clustered_tree->Branch("snr",        &Csnr,       "snr/D");
         r1_clustered_tree->Branch("amplitude",  &Camp,       "amplitude/D");
         r1_clustered_tree->Branch("q",          &Cq,         "q/D");

         //if trigger time j is greater than or equal to the start time k, and less than the 
         //end time k, clone TTree and save new triggers to file? see how florent did it in the clustering
         //could also print to ascii file, but would be unnecessary.
         //this will eventually be done in rounds (10), so there will be 11 different TTrees at the
         //end of the eveto run.
      
         if(tstart[j] >= tstart[k] && time[j] <tend[k]) {

           //print vetoed trigs to TTree
           r1_clustered_tree->Fill();
    
         }
   
      }

      return 0;
   }
  }

}

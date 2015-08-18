#include "cbc_eveto.h"

// Function that reads triggers and segments from root files into chains
bool read_omicron_channel( 
    TChain* veto_trigger_chain, 
    TChain* veto_segment_chain, 
    TString* omicron_trigger_path, 
    const char* channel_name,
    bool verbose ) 
{
  Int_t retcode;

  if ( verbose ) std::cout << "reading triggers and segments for " << channel_name << std::endl;

  char *veto_file_pattern = new char[512];
  snprintf( veto_file_pattern, 512, "%s/%s/*.root", omicron_trigger_path->Data(), channel_name );

  if ( verbose ) std::cout << "Adding root files from " << veto_file_pattern << std::endl;

  // read trigger tree in to chain
  retcode = veto_trigger_chain->Add(veto_file_pattern);
  if ( retcode <= 0 ) {
    std::cerr << "could not read any triggers from " << veto_file_pattern << std::endl;
    return false;
  }

  // read segment tree into chain
  retcode = veto_segment_chain->Add(veto_file_pattern);
  if ( retcode <= 0 ) {
    std::cerr << "could not read any segments from " << veto_file_pattern << std::endl;
    return false;
  }

  return true;
}

// Function that performs simple time clustering of omicron triggers
bool simple_omicron_time_cluster( 
    TTree* clustered_tree, 
    TChain* unclustered_tree,
    Long64_t omicron_start_time,
    Long64_t omicron_end_time,
    double cluster_time_window,
    double cluster_snr_threshold,
    bool verbose
    )
    {
  if ( verbose ) std::cout << "simple_omicron_time_cluster: "<< unclustered_tree->GetEntries() << " unclustered triggers" << std::endl;

  double Ttime, Ttstart, Ttend, Tfreq, Tfstart, Tfend, Tsnr, Tamp, Tq;

  if(unclustered_tree->SetBranchAddress("time",&Ttime)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("frequency",&Tfreq)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing frequency branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("q",&Tq)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing q branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("snr",&Tsnr)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing snr branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("tstart",&Ttstart)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing tstart branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("tend",&Ttend)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing tend branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("fstart",&Tfstart)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing fstart branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("fend",&Tfend)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing fend branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("amplitude", &Tamp)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }

  double Ltime, Ltstart, Ltend, Lfreq, Lfstart, Lfend, Lsnr, Lamp, Lq;
  Long64_t Lfirstentry, Lsize;

  clustered_tree->Branch("time",       &Ltime,      "time/D");
  clustered_tree->Branch("tstart",     &Ltstart,    "tstart/D");
  clustered_tree->Branch("tend",       &Ltend,      "tend/D");
  clustered_tree->Branch("frequency",  &Lfreq,      "frequency/D");
  clustered_tree->Branch("fstart",     &Lfstart,    "fstart/D");
  clustered_tree->Branch("fend",       &Lfend,      "fend/D");
  clustered_tree->Branch("snr",        &Lsnr,       "snr/D");
  clustered_tree->Branch("amplitude",  &Lamp,       "amplitude/D");
  clustered_tree->Branch("q",          &Lq,         "q/D");
  clustered_tree->Branch("firstentry", &Lfirstentry,"firstentry/L");
  clustered_tree->Branch("size",       &Lsize,      "size/L");

  Lfirstentry=-1;
  Ltend=0.0;

  // loop over time sorted triggers
  for(Long64_t t=0; t<unclustered_tree->GetEntries(); t++){
    unclustered_tree->GetEntry(t);
    if(Ttend < omicron_start_time || Ttstart > omicron_end_time) {
      continue;
    }

    // this is the same cluster...
    if(Ttstart-Ltend<cluster_time_window){
      Lsize++; // one more tile
      if(Ttend   > Ltend)   Ltend=Ttend;    // update cluster end
      if(Ttstart < Ltstart) Ltstart=Ttstart;// update cluster tstart
      if(Tfend   > Lfend)   Lfend=Tfend;    // update cluster end
      if(Tfstart < Lfstart) Lfstart=Tfstart;// update cluster tstart
      if(Tsnr>Lsnr){  // this tile is louder
        Lsnr  = Tsnr; // update cluster SNR
        Lamp  = Tamp; // update cluster amplitude - FIXME: could be better!
        Ltime = Ttime;// update cluster time
        Lfreq = Tfreq;// update cluster frequency
        Lq    = Tq;   // update cluster Q
      }
    }
    //... or start a new cluster
    else{
      if(t&&Lsnr>=cluster_snr_threshold){
        clustered_tree->Fill();  // fill tree with previous entry
      }
      Ltend       = Ttend;  // cluster t end
      Ltstart     = Ttstart;// cluster t start
      Lfend       = Tfend;  // cluster f end
      Lfstart     = Tfstart;// cluster f start
      Lsize       = 1;      // cluster size
      Lsnr        = Tsnr;   // cluster SNR
      Lamp        = Tamp;   // cluster amplitude
      Ltime       = Ttime;  // cluster time
      Lfreq       = Tfreq;  // cluster frequency
      Lfirstentry = t;      // cluster first entry
    }
  }

  // save last cluster
  if(unclustered_tree->GetEntries()&&Lsnr>=cluster_snr_threshold){
    clustered_tree->Fill();
  }

  if ( verbose ) std::cout << "simple_omicron_time_cluster: "<< clustered_tree->GetEntries() << " clusters were found" << std::endl;

  return true;
}

// Function that reads omicron files and returns clustered triggers and segments
int eveto::read_omicron_triggers(
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
      bool verbose )
{
  // We iterate over the safe channels, so create storage for the channel name
  char *channel_name = new char[256];
  char *tree_name = new char[256];
  safe_channels->SetBranchAddress("channel", channel_name);

  for( Long64_t i = 0; i < safe_channels->GetEntries(); i++ )
  {
    // fill the channel name with the current veto channel
    safe_channels->GetEntry(i);
    if ( verbose ) std::cout << "Reading channel " << channel_name <<  std::endl;

    // Create a chain to store the unclustered triggers and read from file
    // The names of these chains must match the trees that omicron creates
    TChain* veto_trigger_chain = new TChain( "triggers", channel_name );
    TChain* veto_segment_chain = new TChain( "segments", channel_name );

    // read in the triggers and segments for this channel
    bool read_retcode = read_omicron_channel( veto_trigger_chain, veto_segment_chain, 
        omicron_trigger_path, channel_name, verbose );
    if ( ! read_retcode ){
      std::cerr << "error reading omicron triggers" << std::endl;
      return 1;
    }

    // create a tree to store clustered triggers and store its address in the tree array
    snprintf( tree_name, 256, "VETOTRIGS:%s", channel_name );
    clustered_veto_trigger_tree[i] = new TTree( tree_name, tree_name );
    simple_omicron_time_cluster( clustered_veto_trigger_tree[i], veto_trigger_chain, 
        omicron_start_time, omicron_end_time,
        cluster_time_window, cluster_snr_threshold, verbose );
    delete veto_trigger_chain;

    // Create a new tree containing the veto segments with the correct name for eveto
    snprintf( tree_name, 256, "SEGMENTS:%s", channel_name );
    veto_segment_tree[i] = veto_segment_chain->CloneTree();
    veto_segment_tree[i]->SetObject( tree_name, tree_name );

  }

  delete channel_name;
  delete tree_name;

  return 0;
}

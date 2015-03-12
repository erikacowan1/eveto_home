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
bool simple_time_omicron_cluster( 
    TTree* clustered_tree, 
    TChain* unclustered_tree,
    double Cdelta_t,
    double cl_snr_thr,
    bool verbose
    )
    {
  if ( verbose ) std::cout << "simple_time_omicron_cluster: "<< unclustered_tree->GetEntries() << " unclustered triggers" << std::endl;

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

  double Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
  Long64_t Cfirstentry, Csize;

  clustered_tree->Branch("time",       &Ctime,      "time/D");
  clustered_tree->Branch("tstart",     &Ctstart,    "tstart/D");
  clustered_tree->Branch("tend",       &Ctend,      "tend/D");
  clustered_tree->Branch("frequency",  &Cfreq,      "frequency/D");
  clustered_tree->Branch("fstart",     &Cfstart,    "fstart/D");
  clustered_tree->Branch("fend",       &Cfend,      "fend/D");
  clustered_tree->Branch("snr",        &Csnr,       "snr/D");
  clustered_tree->Branch("amplitude",  &Camp,       "amplitude/D");
  clustered_tree->Branch("q",          &Cq,         "q/D");
  clustered_tree->Branch("firstentry", &Cfirstentry,"firstentry/L");
  clustered_tree->Branch("size",       &Csize,      "size/L");

  Cfirstentry=-1;
  Ctend=0.0;

  // loop over time sorted triggers
  for(Long64_t t=0; t<unclustered_tree->GetEntries(); t++){
    unclustered_tree->GetEntry(t);

    // this is the same cluster...
    if(Ttstart-Ctend<Cdelta_t){
      Csize++; // one more tile
      if(Ttend   > Ctend)   Ctend=Ttend;    // update cluster end
      if(Ttstart < Ctstart) Ctstart=Ttstart;// update cluster tstart
      if(Tfend   > Cfend)   Cfend=Tfend;    // update cluster end
      if(Tfstart < Cfstart) Cfstart=Tfstart;// update cluster tstart
      if(Tsnr>Csnr){  // this tile is louder
        Csnr  = Tsnr; // update cluster SNR
        Camp  = Tamp; // update cluster amplitude - FIXME: could be better!
        Ctime = Ttime;// update cluster time
        Cfreq = Tfreq;// update cluster frequency
        Cq    = Tq;   // update cluster Q
      }
    }
    //... or start a new cluster
    else{
      if(t&&Csnr>=cl_snr_thr){
        clustered_tree->Fill();  // fill tree with previous entry
      }
      Ctend       = Ttend;  // cluster t end
      Ctstart     = Ttstart;// cluster t start
      Cfend       = Tfend;  // cluster f end
      Cfstart     = Tfstart;// cluster f start
      Csize       = 1;      // cluster size
      Csnr        = Tsnr;   // cluster SNR
      Camp        = Tamp;   // cluster amplitude
      Ctime       = Ttime;  // cluster time
      Cfreq       = Tfreq;  // cluster frequency
      Cfirstentry = t;      // cluster first entry
    }
  }

  // save last cluster
  if(unclustered_tree->GetEntries()&&Csnr>=cl_snr_thr){
    clustered_tree->Fill();
  }

  if ( verbose ) std::cout << "simple_time_omicron_cluster: "<< clustered_tree->GetEntries() << " clusters were found" << std::endl;

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
    clustered_veto_trigger_tree[i] = new TTree( tree_name, channel_name );
    simple_time_omicron_cluster( clustered_veto_trigger_tree[i], veto_trigger_chain, 
        omicron_snr_threshold, omicron_cluster_window, verbose );
    delete veto_trigger_chain;

    // Create a new tree containing the veto segments with the correct name for eveto
    snprintf( tree_name, 256, "SEGMENTS:%s", channel_name );
    veto_segment_tree[i] = veto_segment_chain->CloneTree();
    veto_segment_tree[i]->SetObject( tree_name, channel_name );

  }

  delete channel_name;
  delete tree_name;

  return 0;
}

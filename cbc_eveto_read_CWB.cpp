#include "cbc_eveto.h"

// Function that reads triggers and segments from root files into chains
bool read_cwb_channel( 
    TChain* veto_trigger_chain, 
    TChain* veto_segment_chain, 
    TString* cwb_trigger_path, 
    bool verbose ) 
{
  Int_t retcode;

  if ( verbose ) std::cout << "reading triggers and segments for " << cwb_trigger_path << std::endl;

  char *veto_file_pattern = new char[512];
  snprintf( veto_file_pattern, 512, "%s/wave_111*.root", cwb_trigger_path->Data());

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

// Function that performs simple time clustering of cwb triggers
bool simple_cwb_time_cluster( 
    TTree* clustered_tree, 
    TChain* unclustered_tree,
    Long64_t cwb_start_time,
    Long64_t cwb_end_time,
    Double_t cluster_time_window,
    Double_t cluster_snr_threshold,
    bool verbose
    )
    {
  if ( verbose ) std::cout << "simple_cwb_time_cluster: "<< unclustered_tree->GetEntries() << " unclustered triggers" << std::endl;

  double Ttime, Tfrequency, Tduration, Tstart, Tstop, Tsnr, Trho, Tnetcc, Tneted, Tlikelihood, Tecor, TECOR, Tifo, Trate, Tphi, Ttheta, Tpsi,Tnull, Thrss, Tnoise, Tlow, Thigh, Tbandwidth, Tstrain, Trun, Tsize;

  if(unclustered_tree->SetBranchAddress("time",&Ttime)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("frequency",&Tfrequency)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing frequency branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("duration",&Tduration)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing q branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("start",&Tstart)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing tstart branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("stop",&Tstop)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing tend branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("snr",&Tsnr)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing snr branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("rho",&Trho)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing fstart branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("netcc",&Tnetcc)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing fend branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("neted", &Tneted)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("likelihood", &Tlikelihood)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("ecor", &Tecor)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("ECOR", &TECOR)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("ifo", &Tifo)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("rate", &Trate)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("phi", &Tphi)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("theta", &Ttheta)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("psi", &Tpsi)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("null", &Tnull)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("hrss", &Thrss)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("noise", &Tnoise)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("low", &Tlow)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("high", &Thigh)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("bandwidth", &Tbandwidth)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("strain", &Tstrain)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("run", &Trun)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  if(unclustered_tree->SetBranchAddress("size", &Tsize)<0){
    std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    return false;
  }
  double Ctime, Cfrequency, Cduration, Cstart, Cstop, Csnr, Crho, Cnetcc, Cneted, Clikelihood, Cecor, CECOR, Cifo, Crate, Cphi, Ctheta, Cpsi, Cnull, Chrss, Cnoise, Clow, Chigh, Cbandwidth, Cstrain, Crun, Csize;

  clustered_tree->Branch("time",       &Ctime,      "time/D");
  clustered_tree->Branch("frequency",  &Cfrequency, "frequency/D");
  clustered_tree->Branch("duration",   &Cduration,  "duration/D");
  clustered_tree->Branch("start",      &Cstart,     "start/D");
  clustered_tree->Branch("stop",       &Cstop,      "stop/D");
  clustered_tree->Branch("snr",        &Csnr,       "snr/D");
  clustered_tree->Branch("rho",        &Crho,       "rho/D");
  clustered_tree->Branch("netcc",      &Cnetcc,     "netcc/D");
  clustered_tree->Branch("neted",      &Cneted,     "neted/D");
  clustered_tree->Branch("likelihood", &Clikelihood,"likelihood/D");
  clustered_tree->Branch("ecor",       &Cecor,      "ecor/D");
  clustered_tree->Branch("ECOR",       &CECOR,      "ECOR/D");
  clustered_tree->Branch("ifo",        &Cifo,       "ifo/D");
  clustered_tree->Branch("rate",       &Crate,      "rate/D");
  clustered_tree->Branch("phi",        &Cphi,       "phi/D");
  clustered_tree->Branch("theta",      &Ctheta,     "theta/D");
  clustered_tree->Branch("psi",        &Cpsi,       "psi/D");
  clustered_tree->Branch("null",       &Cnull,      "null/D");
  clustered_tree->Branch("hrss",       &Chrss,      "hrss/D");
  clustered_tree->Branch("noise",      &Cnoise,     "noise/D");
  clustered_tree->Branch("low",        &Clow,       "low/D");
  clustered_tree->Branch("high",       &Chigh,      "high/D");
  clustered_tree->Branch("bandwidth",  &Cbandwidth, "bandwidth/D");
  clustered_tree->Branch("strain",     &Cstrain,    "strain/D");
  clustered_tree->Branch("run",        &Crun,       "run/D");
  clustered_tree->Branch("size",       &Csize,      "size/D");

  Cstop=0.0;

  // loop over time sorted triggers
  for(Long64_t t=0; t<unclustered_tree->GetEntries(); t++){
    unclustered_tree->GetEntry(t);
    if(Tstop < cwb_start_time || Tstart > cwb_end_time) {
      continue;
    }

    // this is the same cluster...
    if(Tstart-Cstop<cluster_time_window){
      Csize++; // one more tile
      if(Tstop   > Cstop)   Cstop=Tstop;    // update cluster end
      if(Tstart < Cstart) Cstart=Tstart;// update cluster tstart
//      if(Tfend   > Cfend)   Cfend=Tfend;    // update cluster end
//      if(Tfstart < Cfstart) Cfstart=Tfstart;// update cluster tstart
      if(Tsnr>Csnr){  // this tile is louder
        Csnr  = Tsnr; // update cluster SNR
        Ctime = Ttime;// update cluster time
        Cfrequency = Tfrequency;// update cluster frequency
      }
    }
    //... or start a new cluster
    else{
      if(t&&Csnr>=cluster_snr_threshold){
        clustered_tree->Fill();  // fill tree with previous entry
      }
      Cstop       = Tstop;  // cluster t end
      Cstart     = Tstart;// cluster t start
      //Cfend       = Tfend;  // cluster f end
      //Cfstart     = Tfstart;// cluster f start
      Csize       = 1;      // cluster size
      Csnr        = Tsnr;   // cluster SNR
      Ctime       = Ttime;  // cluster time
      Cfrequency       = Tfrequency;  // cluster frequency
    }
  }

  // save last cluster
  if(unclustered_tree->GetEntries()&&Csnr>=cluster_snr_threshold){
    clustered_tree->Fill();
  }

  if ( verbose ) std::cout << "simple_cwb_time_cluster: "<< clustered_tree->GetEntries() << " clusters were found" << std::endl;

  return true;
}

// Function that reads cwb files and returns clustered triggers and segments
 int  eveto::read_cwb_triggers(
      TTree* clustered_veto_trigger_tree[],
      TTree* veto_segment_tree[],
      TTree* safe_channels,
      TString* cwb_trigger_path,
      Double_t cwb_snr_threshold,
      Double_t cbc_cluster_window,
      Long64_t gps_start_time,
      Long64_t gps_end_time,
      Double_t cluster_time_window,
      Double_t cluster_snr_threshold,
      bool verbose )
{
  // We iterate over the safe channels, so create storage for the channel name
  char *tree_name = new char[256];

  
    // fill the channel name with the current veto channel

    // Create a chain to store the unclustered triggers and read from file
    // The names of these chains must match the trees that cwb creates
    TChain* veto_trigger_chain = new TChain( "waveburst", "cwb_unclustered_tree" );

    // read in the triggers and segments for this channel
    bool read_retcode = read_cwb_channel( veto_trigger_chain, veto_segment_chain, cwb_trigger_path, verbose );
    if ( ! read_retcode ){
      std::cerr << "error reading cwb triggers" << std::endl;
      return 1;
    }

    // create a tree to store clustered triggers and store its address in the tree array
    snprintf( tree_name, 256, "CWB_Trigger_Tree" );
    clustered_veto_trigger_tree[i] = new TTree( tree_name, tree_name );
    simple_cwb_time_cluster( clustered_veto_trigger_tree[i], veto_trigger_chain, 
        cwb_start_time, cwb_end_time,
        cluster_time_window, cluster_snr_threshold, verbose );
    delete veto_trigger_chain;

    // Create a new tree containing the veto segments with the correct name for eveto
    //snprintf( tree_name, 256, "SEGMENTS:%s", channel_name );
    //veto_segment_tree[i] = veto_segment_chain->CloneTree();
    //veto_segment_tree[i]->SetObject( tree_name, tree_name );

  

  delete tree_name;

  return 0;
}

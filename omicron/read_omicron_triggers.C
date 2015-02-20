#include <iostream>
#include <string>
#include <getopt.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TTree.h>
#include <TSQLServer.h>
#include <TSQLStatement.h>
#include <stdio.h>
#include <string.h>
#include <THStack.h>
#include <TBrowser.h>
#include <TFile.h>
#include <TChain.h>

TChain* read_omicron_channel( TString trigger_directory, const char* channel_name ) {

  std::cout << "Creating triggers chain for " << channel_name << std::endl;
  TChain* veto_trigger_chain = new TChain( "triggers", channel_name );

  char *veto_file_pattern = new char[512];
  snprintf( veto_file_pattern, 512, "%s/%s/*.root", trigger_directory.Data(), channel_name );

  std::cout << "Adding files " << veto_file_pattern << std::endl;
  veto_trigger_chain->Add(veto_file_pattern);//grabs florent's data and puts in veto_trigger_chain

  return veto_trigger_chain;
}

bool simple_time_veto_cluster( TTree* clustered_tree, TChain* unclustered_tree )
{

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
  double Cdelta_t = 1.0; // seconds
  double cl_snr_thr = 10.0; // SNR threshold for clustering
  double Cmeandur;

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
  Cmeandur=0.0;

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
        Cmeandur+=(Ctend-Ctstart);
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
    Cmeandur+=(Ctend-Ctstart);
  }
  if(clustered_tree->GetEntries()) Cmeandur/=(double)(clustered_tree->GetEntries());
  std::cout << "simple_time_veto_cluster: "<< clustered_tree->GetEntries() << " clusters were found" << std::endl;

  return true;
}

int read_omicron_triggers( TString trigger_directory, TString safe_channel_file ) {

  //create ROOT file that will store omicron triggers
  TFile* f = new TFile("omicron_triggers.root","RECREATE");

  std::cout << "Reading safe channel list from " << safe_channel_file << std::endl;

  //create TTree that will store the safe channel list, and reads in safe channels 
  TTree* safe_channel_tree = new TTree( "safe_channel_list", "List of safe channels to process" );
  Long64_t num_veto_channels = safe_channel_tree->ReadFile( safe_channel_file, "channel/C" );
  std::cout << "Read " << num_veto_channels << " channel names" << std::endl;

  safe_channel_tree->Print();

  std::cout << "Reading triggers from directory " << trigger_directory << std::endl;

  //create array of pointers to TChain, of size num_safe_channels
  TChain* veto_trigger_tree_array[num_veto_channels]; //already alive from Florent
  TTree* clustered_veto_trigger_tree_array[num_veto_channels]; //we create this
 
  char *channel_name = new char[256];
  safe_channel_tree->SetBranchAddress("channel", channel_name); //creates association btwn channel TTree and string channel_name

  std::cout << "number of channels = " << num_veto_channels << std::endl;
  for( Long64_t i = 0; i < num_veto_channels; i++ )
  {
    std::cout << "i = " << i << std::endl;
    safe_channel_tree->GetEntry(i); //sets channel name to ith leaf
    std::cout << "Reading channel " << channel_name <<  std::endl;
    veto_trigger_tree_array[i] = read_omicron_channel( trigger_directory, channel_name );

    // create a tree to store clustered triggers and store its address in the tree array
    clustered_veto_trigger_tree_array[i] = new TTree( channel_name, channel_name );

    simple_time_veto_cluster( clustered_veto_trigger_tree_array[i], veto_trigger_tree_array[i] );
  }

  new TBrowser;
  return 0;
}


int main( int argc, char* argv[] )
{
  // Create arrays to store command line arguments
  char path[512];
  char safe_file[512];

  // print and store command line arguments
  fprintf( stderr, "%s %s %s\n", argv[0], argv[1], argv[2] );
  snprintf( path, 512, argv[1] );
  snprintf( safe_file, 512, argv[2] );

  // create ROOT application and execute trigger read in function
  TApplication* rootapp = new TApplication("read_omicron_triggers", &argc, argv);
  read_omicron_triggers( path, safe_file );
  rootapp->Run();
  return 0;
}


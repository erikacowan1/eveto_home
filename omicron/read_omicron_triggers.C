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
  veto_trigger_chain->Add(veto_file_pattern);

  return veto_trigger_chain;
}

TTree* simple_time_veto_cluster( TChain* unclustered_trigger_tree )
{
  return NULL;
}

int read_omicron_triggers( TString trigger_directory, TString safe_channel_file ) {

  TFile *f = new TFile("omicron_triggers.root","RECREATE");

  std::cout << "Reading safe channel list from " << safe_channel_file << std::endl;

  TTree* safe_channel_tree = new TTree( "safe_channel_list", "List of safe channels to process" );
  Long64_t num_veto_channels = safe_channel_tree->ReadFile( safe_channel_file, "channel/C" );
  std::cout << "Read " << num_veto_channels << " channel names" << std::endl;

  safe_channel_tree->Print();

  std::cout << "Reading triggers from directory " << trigger_directory << std::endl;
  Long64_t num_safe_channels = safe_channel_tree->GetEntries();
  TChain* veto_trigger_tree_array[num_safe_channels];
  TTree* clustered_veto_trigger_tree_array[num_safe_channels];
 
  char *channel_name = new char[256];
  safe_channel_tree->SetBranchAddress("channel", channel_name);

  std::cout << "number of channels = " << num_safe_channels << std::endl;
  for( Long64_t i = 0; i < num_safe_channels; i++ )
  {
    std::cout << "i = " << i << std::endl;
    safe_channel_tree->GetEntry(i);
    std::cout << "Reading channel " << channel_name <<  std::endl;
    veto_trigger_tree_array[i] = read_omicron_channel( trigger_directory, channel_name );

    clustered_veto_trigger_tree_array[i] = simple_time_veto_cluster( veto_trigger_tree_array[i] );

    veto_trigger_tree_array[i]->Print();
  }

  new TBrowser;
  return 0;
}


int main( int argc, char *argv[] )
{
  char path[512];
  char safe_file[512];

  fprintf( stderr, "%s %s %s\n", argv[0], argv[1], argv[2] );
  snprintf( path, 512, argv[1] );
  snprintf( safe_file, 512, argv[2] );

  TApplication* rootapp = new TApplication("read_omicron_triggers", &argc, argv);
  read_omicron_triggers( path, safe_file );
  rootapp->Run();
  return 0;
}


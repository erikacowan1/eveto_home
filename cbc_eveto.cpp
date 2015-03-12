#include "cbc_eveto.h"

int main( int argc, char *argv[] )
{
  int getopt_result;

  Long_t gps_start_time = 0;
  Long_t gps_end_time = 0;
  TString* detector = new TString();
  TString* safe_channel_file = new TString();
  
  TString* cbc_trigger_database = new TString();
  Float_t cbc_snr_threshold = 0;

  TString* omicron_trigger_path = new TString();
  Double_t omicron_snr_threshold = 0;
  Double_t omicron_cluster_window = 0;

  Float_t sig_threshold = 0;
  Float_t dumb_veto_window = 0;
  int max_rounds = 0;

  TString* output_directory = new TString();
  bool verbose;

  // Parse command line arguments before passing them to root
  while( 1 )
  {
    static struct option long_options[] = {
      // options that determine the data to process
      {"gps-start-time", required_argument, 0, 's' },
      {"gps-end-time", required_argument, 0, 'e' },
      {"detector", required_argument, 0, 'd' },      
      {"safe-channel-file", required_argument, 0, 'c' },      

      // options that control the input cbc triggers
      {"cbc-trigger-database", required_argument, 0, 'i' },
      {"cbc-snr-threshold", required_argument, 0, 't' },

      // options that control the input veto triggers from omicron
      {"omicron-trigger-path", required_argument, 0, 'I' },
      {"omicron-snr-threshold", required_argument, 0, 'T' },
      {"omicron-cluster-window", required_argument, 0, 'W' },

      // options that control significance and veto 
      {"sig-threshold", required_argument, 0, 'S' },
      {"dumb-veto-window", required_argument, 0, 'D' },
      {"max-rounds", required_argument, 0, 'm' },

      // options that control the output
      {"output-directory", required_argument, 0, 'o' },
      {"verbose", no_argument, 0, 'v'},
      {0, 0, 0, 0}
    };
    int option_index = 0;

    getopt_result = getopt_long( argc, argv, "s:e:d:c:i:t:I:T:W:o:S:D:m:v:", long_options, &option_index );

    if ( getopt_result == -1 ) break;

    switch( getopt_result )
    {
      case 's':
        gps_start_time = atol( optarg );
        break;

      case 'e':
        gps_end_time = atol( optarg );
        break;

      case 'd':
        detector->Append( optarg );
        break;

      case 'c':
        safe_channel_file->Append( optarg );
        break;

      case 'i':
        cbc_trigger_database->Append( optarg );
        break;

      case 't':
        cbc_snr_threshold = atof( optarg );
        break;

      case 'I':
        omicron_trigger_path->Append( optarg );
        break;

      case 'T':
        omicron_snr_threshold = atof( optarg );
        break;

      case 'W':
        omicron_cluster_window = atof( optarg );
        break;
        
      case 'o':
        output_directory->Append( optarg );
        break;

      case 'S':
        sig_threshold = atof( optarg );
        break;

      case 'D':
        dumb_veto_window = atof( optarg );
        break;

      case 'm':
        max_rounds = atof( optarg );
        break;

      case 'v':
        verbose = true;
        break;
        
      case '?':
        break;

      default:
        std::cerr << "Error parsing command line arguments" << std::endl;
        exit( 1 );
    }
  }

  if ( verbose ) {
    std::cout << "Starting cbc_eveto for " << detector->Data() << " data" << std::endl;
    std::cout << "Processing time interval [" << gps_start_time << "," << gps_end_time << ")" << std::endl;
    std::cout << "Reading safe channel list from " << safe_channel_file->Data() << std::endl;
    std::cout << "Reading cbc triggers from " << cbc_trigger_database->Data() << std::endl;
    std::cout << "Applying cbc snr threshold " << cbc_snr_threshold << std::endl;
    std::cout << "Reading omicron triggers from directory " << omicron_trigger_path->Data() << std::endl;
    std::cout << "Applying omicron snr threshold " << omicron_snr_threshold << std::endl;
    std::cout << "Applying omicron cluster window " << omicron_cluster_window << std::endl;
    std::cout << "Writing output to " << output_directory->Data() << std::endl;
  }

  // create a new root application
  TApplication* rootapp = new TApplication("cbc_eveto", &argc, argv);

  TFile* f = new TFile("cbc_eveto.root","RECREATE");

  // call the main function that actually does the work
  int retcode = eveto::cbc_eveto_main( 
      gps_start_time, gps_end_time, detector, safe_channel_file,
      cbc_trigger_database, cbc_snr_threshold,
      omicron_trigger_path, omicron_snr_threshold, omicron_cluster_window,
      output_directory, verbose
      );

  // check the return code from eveto main function
  if ( retcode ) {
    std::cerr << "cbc_eveto_main failed, exiting with code " << retcode << std::endl;
    exit( retcode );
  } else {
    // run the app until the user quits
    new TBrowser;
    rootapp->Run();
  }

  f->Close();

  // exit with success
  exit( 0 );
}

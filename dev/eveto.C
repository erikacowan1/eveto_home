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
using namespace std;


//import cbc_triggers from .root file

int import_cbc_trigs(int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* outputdir, char* sql_db, int number, Double_t x_low, Double_t x_high, int n_bins)
{

 // Return a tree containing triggers from a database 
  TTree* get_triggers_from_database( TString db_filename ) {

 // Create variables for storing triggers 
    Float_t snr;
    Float_t chisq;
    Float_t chisqdof;    
    Float_t newsnr;   
    Float_t mass1;
    Float_t mass2;
    Float_t mtotal;
    Float_t mchirp;
    Float_t ttotal;
    Float_t eta;
    Float_t snr_sq;

   // Create strings for the SQL statements  
    TString select_snr_chisq( "select sngl_inspiral.event_id,sngl_inspiral.snr,sngl_inspiral.chisq,sngl_inspiral.chisq_dof, sngl_inspiral.mass1,sngl_inspiral.mass2, sngl_inspiral.mtotal, sngl_inspiral.mchirp, sngl_inspiral.ttotal, sngl_inspiral.eta from sngl_inspiral where sngl_inspiral.ifo = detector[] " );
        TString zero_lag_clause( " and experiment_summary.datatype = 'all_data'" );
        TString time_slide_clause( " and experiment_summary.datatype = 'slide'" );
        TString snr_chisq_join_clause( " and experiment_summary.experiment_summ_id = experiment_map.experiment_summ_id and experiment_map.coinc_event_id = coinc_event.coinc_event_id and coinc_event.coinc_event_id = coinc_event_map.coinc_event_id and coinc_event_map.event_id = sngl_inspiral.event_id" );

// Create a TTree to store the triggers 
    TTree *cbc_trigs_tree = new TTree("T","cbc-trigger-original");
    cbc_trigs_tree->Branch( "snr" , &snr, "snr/F" );
    cbc_trigs_tree->Branch( "chisq" , &chisq, "chisq/F" );
    cbc_trigs_tree->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
    cbc_trigs_tree->Branch( "newsnr" , &newsnr, "newsnr/F" );
    cbc_trigs_tree->Branch( "mass1", &mass1, "mass1/F" );
    cbc_trigs_tree->Branch( "mass2", &mass2, "mass2/F" );
    cbc_trigs_tree->Branch( "mtotal", &mtotal, "mtotal/F" );
    cbc_trigs_tree->Branch( "mchirp", &mchirp, "mchirp/F" );
    cbc_trigs_tree->Branch( "ttotal", &ttotal, "ttotal/F" );
    cbc_trigs_tree->Branch( "eta", &eta, "eta/F" );
    cbc_trigs_tree->Branch( "snr_sq", &snr_sq, "snr_sq/F" );
    cbc_trigs_tree->Branch( "background", &background, "background/I");
    cbc_trigs_tree->Branch( "database" , &db_filename );


// Create a connection to the sqlite database 

    TSQLServer* serv = TSQLServer::Connect( db_filename, "", "" );

	if ( background ) {
    		cout << "loading background triggers" << endl;
    		select_snr_chisq = select_snr_chisq + time_slide_clause + snr_chisq_join_clause;
	}

	else {
    		cout << "loading zero lag triggers" << endl;

	}

// SQL statement to get triggers from the database

   TSQLStatement* stmt = serv->Statement(select_snr_chisq, 50000);

 // Process the database query 
 	if ( stmt->Process() ) {
	
	// Store the result of the statement in the buffer 
  	stmt->StoreResult();

	// process the data returned in the buffer 
	while ( stmt->NextResultRow() ) {
        	snr = stmt->GetDouble(1);
        	chisq = stmt->GetDouble(2);
        	chisqdof = 2.0 * stmt->GetDouble(3) - 2.0;
        	mass1 = stmt->GetDouble(4);
        	mass2 = stmt->GetDouble(5);
        	snr_sq = snr*snr;
	

	// compute the new SNR 
	if ( chisq > chisqdof ) {
        	newsnr = snr * pow(((1.0 + pow((chisq/chisqdof), 3.0))/2.0), (-1.0 /6.0));
	} 
	
	else {
         	newsnr = snr;
        }

 	// store the data in the tree and the histograms;    

	cbc_trigs_tree->Fill();
	}
	}

	else {
		cout << "Error processing SQL" << endl;
	}


	// Close the database 
	delete serv;

    return cbc_trigs_tree;
  }


//import omicron triggers from database
//int import_omicron_trigs(int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* outputdir, char* sql_db, int number, Double_t x_low, Double_t x_high, int n_bins) {


bool read_omicron_channel( TChain* veto_trigger_chain, TChain* veto_segment_chain, TString trigger_directory, const char* channel_name ) {

  std::cout << "Creating triggers chain for " << channel_name << std::endl;

  //declares pointer of veto_file_pattern to array of size 512
  char *veto_file_pattern = new char[512];
  //connects the list of safe channels to the information stored in Florent's omicron files
  snprintf( veto_file_pattern, 512, "%s/%s/*.root", trigger_directory.Data(), channel_name );

  std::cout << "Adding files " << veto_file_pattern << std::endl;

  //gets triggers/segments and stores them in TChain veto_trigger_chain and veto_segement_chain respectively
  veto_trigger_chain->Add(veto_file_pattern);
  veto_segment_chain->Add(veto_file_pattern);

  return true;

//algorithm for simple time veto clustering, based on Florent's clustering algorithm
bool simple_time_veto_cluster( TTree* clustered_tree, TChain* unclustered_tree )
{
  std::cout << "simple_time_veto_cluster: "<< unclustered_tree->GetEntries() << " unclustered triggers" << std::endl;

  //define variables used in unclustered trigger tree
  double Ttime, Ttstart, Ttend, Tfreq, Tfstart, Tfend, Tsnr, Tamp, Tq;

  //set branch addresses for variables in unclustered trigger tree
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



  //define variables used in clustered trigger tree
  double Cdelta_t, cl_snr_thr, Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
  Long64_t Cfirstentry, Csize;

  //USE COMMAND LINE ARG-PARSE
 // double Cdelta_t = 1.0; // seconds
 // double cl_snr_thr = 10.0; // SNR threshold for clustering

  //set branch addresses for variables in clustered trigger tree
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

  //initialize two branches
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


  std::cout << "simple_time_veto_cluster: "<< clustered_tree->GetEntries() << " clusters were found" << std::endl;

  return true;
}


int read_omicron_triggers( TString trigger_directory, TString safe_channel_file ) {

  //create ROOT file that will store omicron triggers- ALL TTrees created in this algorithm will be saved here
  TFile* f = new TFile("omicron_triggers.root","RECREATE");

  std::cout << "Reading safe channel list from " << safe_channel_file << std::endl;

  //create TTree that will store the safe channel list, and reads in safe channels 
  TTree* safe_channel_tree = new TTree( "safe_channel_list", "List of safe channels to process" );
  Long64_t num_veto_channels = safe_channel_tree->ReadFile( safe_channel_file, "channel/C" );
  std::cout << "Read " << num_veto_channels << " channel names" << std::endl;

  safe_channel_tree->Print();

  std::cout << "Reading triggers from directory " << trigger_directory << std::endl;

  //create array of pointers to TChain, of size num_safe_channels
  TTree* clustered_veto_trigger_tree_array[num_veto_channels]; //we create this
  TTree* veto_segment_tree_array[num_veto_channels]; //already alive from Florent

  //creates pointers to arrays channel_name, and channel_name_segments of size 246 each
  char *channel_name = new char[256];
  char *channel_name_segments = new char[256];

  //creates association btwn channel TTree and string channel_name
  safe_channel_tree->SetBranchAddress("channel", channel_name);

  //reads in TChain of channels(segs/trigs), clusters, saves clusters, and saves TChain
  std::cout << "number of channels = " << num_veto_channels << std::endl;
  for( Long64_t i = 0; i < num_veto_channels; i++ )
  {
    std::cout << "i = " << i << std::endl;
    safe_channel_tree->GetEntry(i); //sets channel name to ith leaf
    std::cout << "Reading channel " << channel_name <<  std::endl;

    // Create a chain to store the unclustered triggers/segments and read from file
    TChain* veto_trigger_chain = new TChain( "triggers", channel_name );
    TChain* veto_segment_chain = new TChain( "segments", channel_name );

    //read in omicron triggers to veto_trigger-chain and veto_segment_chain
    read_omicron_channel( veto_trigger_chain, veto_segment_chain, trigger_directory, channel_name );

    // create a tree to store clustered triggers and store its address in the tree array
    clustered_veto_trigger_tree_array[i] = new TTree( channel_name, channel_name );

    //clusters triggers, and deletes unclustered chain
    simple_time_veto_cluster( clustered_veto_trigger_tree_array[i], veto_trigger_chain );
    delete veto_trigger_chain;

    //clone veto_segment_tree, and renames it to channel_name + segments
    snprintf( channel_name_segments, 256, "%s_segments", channel_name );
    veto_segment_tree_array[num_veto_channels] = veto_segment_chain->CloneTree();
    veto_segment_tree_array[num_veto_channels]->SetObject( channel_name_segments, channel_name );
    delete veto_segment_chain;
  }

  new TBrowser;
  return 0;
}


/*
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
*/



}




//MAIN Hierarchical Veto function

int eveto_main( int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* outputdir, char* sql_db, int number, Double_t x_low, Double_t x_high, int n_bins, double cl_snr_thr, double Cdelta_t )
{
  gROOT->Reset();

// Open ROOT file for triggers
  TFile f("cbc-trigger-original.root");

//Create histogram
  TH1F *trigger = new TH1F("trigger", "Histogram of triggers", nbins, x_low, x_high);


//state variables
    Float_t snr;
    Float_t chisq;
    Float_t chisqdof;
    Float_t newsnr;
    Float_t mass1;
    Float_t mass2;
    Float_t mtotal;
    Float_t mchirp;
    Float_t ttotal;
    Float_t eta;
    Float_t snr_sq;

//read triggers (CBC)
import_cbc_trigs(gps_start_time, gps_end_time, cbc_trigger_snr_threshold,  detector, outputdir, sql_db,  number, x_low, x_high, n_bins)

//read omicron triggers
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

//run core hveto algorithm

int veto(){

  float* trigtimes;
  float* starttimes;
  float* endtimes;
  FILE* fptrig;
  FILE* fpseg;
  float dummytime;
  float dummystart;
  float dummyend;
  int retcode = 0;
  int numtriglines = 0;
  int numseglines = 0;
  int *trigs;
  int i,j,k;

  int num_line_trig_tree;
  int num_line_seg_tree;

  //currently the segments are stored in omicron_triggers.root/segments. For now
  //assume that all segments are science segments. Grab segments from segment tree
  //and perform veto algorithm. 

  //veto triggers
  //increment number of triggers in TTree
  for(Long64_t j=0; j<clustered_tree->GetEntries(); ++j){ //make trigger_tree variable called num_line_trig_tree
    clustered_tree->GetEntry(j);
  //increment number of segments in TTree
    for (Long64_t k=0; k<segment_tree->GetEntries();++k){ //make segment_tree variable called num_line_seg_tree
    segment_tree->GetEntry(k);

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
  r1_clustered_tree->Branch("firstentry", &Cfirstentry,"firstentry/L");
  r1_clustered_tree->Branch("size",       &Csize,      "size/L");




   //if trigger time j is greater than or equal to the start time k, and less than the 
   //end time k, clone TTree and save new triggers to file? see how florent did it in the clustering
   //could also print to ascii file, but would be unnecessary.
   //this will eventually be done in rounds (10), so there will be 11 different TTrees at the
   //end of the eveto run.
    if(time[j] >= tstart[k] && time[j] <tend[k]) { //change name of time[j] to trig_time[j] and start_time[k] to start_seg_time[k] and dito for endtime in segment and trigger tree, back in other algorithm
  

      //print vetoed trigs to TTree
    r1_clustered_tree->Fill();
    }

    }

    else {
    break;    
    }
    break;
    }
  }
}

























//Make TTree with vetoed triggers and times
	//import vetoed trigs from ascii to TTree
	TString dir = "/home/erika.cowan/src/eveto/";//eventually make this a less specific directory, but OK for now
	dir.ReplaceAll("vetoed_trigs.dat","");
	dir.ReplaceAll("/./","/");	

	TFile *f = new TFile("vetoed_trigs.root","RECREATE");
	TH1F *h1 = new TH1F("h1", "x distribution",100,-4,4);
	TTree *T1 = new TTree("ntuple", "data from ascii file");
	Long64_t nlines = T1->ReadFile(Form("%svetoed_trigs.dat",dir.Data()),"snr:start_time:end_time");
	printf("found %11d points\n", nlines);
	T1->Draw("snr");
	T1->Write();

//call vetoed trigger histogram into variable or TCut


	//established *algorithm* for taking a TTree and use the variables to SUBTRACT triggers and times


	//TCuts algorithm -> use to cut data, then to export to new .root file/histogram





}


//DON'T TOUCH-command line parsing- to be put in the pre-process eveto.C

int main( int argc, char *argv[] )
{
 //declaration, allotment, & initialization of variables
  int n_bins;
  int c;
  int number;
  int gps_start_time = 0;
  int gps_end_time = 0;
  Double_t x_low;
  Double_t x_high;
  Double_t cbc_trigger_snr_threshold = 0;
  size_t outputdir_len = 2048;
  size_t detector_len = 10;
  size_t sql_db_len = 2048;
  char outputdir[outputdir_len];
  char detector[detector_len];
  char sql_db[sql_db_len];
  memset( sql_db, 0, sql_db_len * sizeof(char) );
  memset( outputdir, 0, outputdir_len * sizeof(char) );
  memset( detector, 0, detector_len * sizeof(char) );

  TApplication* rootapp = new TApplication("eveto", &argc, argv);

  while( 1 )
  {
    static struct option long_options[] = {
      {"gps-start-time", required_argument, 0, 's' },
      {"gps-end-time", required_argument, 0, 'e' },
      {"cbc-trigger-snr-threshold", required_argument, 0, 't' },
      {"detector", required_argument, 0, 'd' },      
      {"output-directory", required_argument, 0, 'o' },
      {"cbc-trigger-database", required_argument, 0, 'q' },
      {"number", required_argument, 0, 'n' },
      {"x_low", required_argument, 0, 'x' },
      {"x_high", required_argument, 0, 'X' },
      {"n_bins", required_argument, 0, 'N' },
      {"cl_snr_thr", required_argument, 0, 'S' },
      {"Cdelta_t", required_argument, 0, 'D' },
      {0, 0, 0, 0}
    };
    int option_index = 0;

    c = getopt_long( argc, argv, "s:e:t:d:o:q:n:x:X:N:S:D:", long_options, &option_index );

    if ( c == -1 ) break;

    switch( c )
    {
      case 's':
        gps_start_time = atoi( optarg );
        if ( gps_start_time < 800000000 ) 
	{
		fprintf( stderr, "bad start time\n" );
		exit( 1 );
	}
        break;

      case 'e':
        gps_end_time = atof( optarg );
        break;

      case 't':
        cbc_trigger_snr_threshold = atof( optarg );
        break;

      case 'd':
	if(strlen(optarg) < detector_len) {
		strncpy(detector,optarg,detector_len*sizeof(char));
	}      
	else {
		printf("length of detector name is greater than allotted array length");
		exit( 1 );
	}
	break;

      case 'o':
	if(strlen(optarg) < outputdir_len) {
		strncpy(outputdir,optarg,outputdir_len*sizeof(char));
	}
	else  {
		printf("length of output directory (outputdir) is greater than alotted array length");
		exit( 1 );
	}
	break;

       case 'q':
        if(strlen(optarg) < sql_db_len) {
                strncpy(sql_db,optarg,sql_db_len*sizeof(char));
        }
        else  {
                printf("length of database name is greater than alotted array length");
                exit( 1 );
        } 
        break;
	
      case 'n':
        number = atoi( optarg );
        break;

      case 'x':
        x_low = atof( optarg );
        break;

      case 'X':
        x_high = atof( optarg );
        break;
      
      case 'N':
	n_bins = atof( optarg );
	break;

      case 'S':
        cl_snr_thr = atof( optarg );
        break; 


      case 'D':
        Cdelta_t = atof( optarg );
        break;

      case '?':
        break;

      default:
        fprintf( stderr, "error between argument map and case statement\n" );
        abort();
    }
  }

  if ( ! gps_start_time || ! gps_end_time || ! cbc_trigger_snr_threshold || ! outputdir[0] || ! detector[0] || ! sql_db[0] || ! number || ! x_low || ! x_high || ! n_bins )
  {
	  fprintf( stderr, "you forgot an argument\n" );
	  exit( 1 );
  }

  eveto_main( gps_start_time, gps_end_time, cbc_trigger_snr_threshold, detector, outputdir , sql_db, number, x_low, x_high, n_bins );

  rootapp->Run();
  return 0;
}

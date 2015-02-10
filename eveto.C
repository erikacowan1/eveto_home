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

int import_omicron_trigs(int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* outputdir, char* sql_db, int number, Double_t x_low, Double_t x_high, int n_bins) {
 // Return a tree containing either foreground or background triggers from a database                 
  TTree* get_triggers_from_database( Int_t background, TString db_filename ) 

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
 
    // Create a TTree to store the triggers                                                            
    TTree *aux_trigs_tree = new TTree("T","single inspiral triggers");
    aux_trigs_tree->Branch( "snr" , &snr, "snr/F" );
    aux_trigs_tree->Branch( "chisq" , &chisq, "chisq/F" );
    aux_trigs_tree->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
    aux_trigs_tree->Branch( "newsnr" , &newsnr, "newsnr/F" );
    aux_trigs_tree->Branch( "mass1", &mass1, "mass1/F" );
    aux_trigs_tree->Branch( "mass2", &mass2, "mass2/F" );
    aux_trigs_tree->Branch( "mtotal", &mtotal, "mtotal/F" );
    aux_trigs_tree->Branch( "mchirp", &mchirp, "mchirp/F" );
    aux_trigs_tree->Branch( "ttotal", &ttotal, "ttotal/F" );
    aux_trigs_tree->Branch( "eta", &eta, "eta/F" );
    aux_trigs_tree->Branch( "snr_sq", &snr_sq, "snr_sq/F" );       
    aux_trigs_tree->Branch( "background", &background, "background/I");
    aux_trigs_tree->Branch( "database" , &db_filename );

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
	} else {
	  newsnr = snr;
	}

	// store the data in the tree and the histograms;                                              
	aux_trigs_tree->Fill();
    return aux_trigs_tree;
  }
}


//MAIN Hierarchical Veto function

int eveto_main( int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* outputdir, char* sql_db, int number, Double_t x_low, Double_t x_high, int n_bins )
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

//read triggers (CBC or omicron)


//run core hveto algorithm


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
      {0, 0, 0, 0}
    };
    int option_index = 0;

    c = getopt_long( argc, argv, "s:e:t:d:o:q:n:x:X:", long_options, &option_index );

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

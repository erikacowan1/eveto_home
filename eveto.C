#include <iostream>
#include <string>
#include <getopt.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TLegend.h>
#include <TTree.h>
#include <TSQLServer.h>
#include <TSQLStatement.h>
#include <stdio.h>
#include <string.h>

using namespace std;

//int eveto_main( int number, Double_t x_low, Double_t x_high )
//{
  // Return a tree containing either foreground or background triggers from a database                 
  TTree* get_triggers_from_database( Int_t background, TString db_filename ) {

    // Create variables for storing triggers                                                           
    Float_t snr;
    Float_t chisq;
    Float_t chisqdof;
    Float_t newsnr;
    Float_t mass1;
    Float_t mass2;
    Float_t mass_tot;

    // Create strings for the SQL statements                                                           
    TString select_snr_chisq( "select distinct sngl_inspiral.event_id,sngl_inspiral.snr,sngl_inspiral.\
chisq,sngl_inspiral.chisq_dof from experiment_summary,experiment_map,coinc_event,coinc_event_map,sng\
l_inspiral where sngl_inspiral.ifo = 'H1'" );
    TString zero_lag_clause( " and experiment_summary.datatype = 'all_data'" );
    TString time_slide_clause( " and experiment_summary.datatype = 'slide'" );
    TString snr_chisq_join_clause( " and experiment_summary.experiment_summ_id = experiment_map.experi\
ment_summ_id and experiment_map.coinc_event_id = coinc_event.coinc_event_id and coinc_event.coinc_ev\
ent_id = coinc_event_map.coinc_event_id and coinc_event_map.event_id = sngl_inspiral.event_id" );

    // Create a TTree to store the triggers                                                            
    TTree *tree = new TTree("T","single inspiral triggers");
    tree->Branch( "snr" , &snr, "snr/F" );
    tree->Branch( "chisq" , &chisq, "chisq/F" );
    tree->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
    tree->Branch( "newsnr" , &newsnr, "newsnr/F" );
    tree->Branch( "background", &background, "background/I");
    tree->Branch( "database" , &db_filename );
    tree->Branch( "mass1", &mass1, "mass1/F" );
    tree->Branch( "mass2", &mass2, "mass2/F" );
    tree->Branch( "mass_tot", &mass_tot, "mass_tot/F" );

    // Create a connection to the sqlite database                                                      
    TSQLServer* serv = TSQLServer::Connect( db_filename, "", "" );

    if ( background ) {
      cout << "loading background triggers" << endl;
      select_snr_chisq = select_snr_chisq + time_slide_clause + snr_chisq_join_clause;
    } else {
      cout << "loading zero lag triggers" << endl;
      select_snr_chisq = select_snr_chisq + zero_lag_clause + snr_chisq_join_clause;
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

	// compute the new SNR                                                                         
	if ( chisq > chisqdof ) {
	  newsnr = snr * pow(((1.0 + pow((chisq/chisqdof), 3.0))/2.0), (-1.0 /6.0));
	} else {
	  newsnr = snr;
	}
	//compute total mass
	mass_tot = mass1 + mass2;

	// store the data in the tree and the histograms;                                              
	tree->Fill();
      }
    }
    else {
      cout << "Error processing SQL" << endl;
    }

    // Close the database                                                                              
    delete serv;

    return tree;
  }
int eveto_main( int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* output_path )
{
  //Read in Triggers
  TTree* clustered_cbc_trigs = get_triggers_from_database( 1, "sqlite://l1-lock-clustered.sqlite" );
  Int_t n_bins = 50;
  Int_t x_low = 0;
  Int_t x_high = 20;
  //Create Overlay of SNR and newSNR histograms
  TH1F *h1 = new TH1F("h1","Background CBC Triggers",n_bins,x_low,x_high);
  TH1F *h2 = new TH1F("h1","Background CBC Triggers",n_bins,x_low,x_high);
  clustered_cbc_trigs->Draw("snr>>h1");
  clustered_cbc_trigs->Draw("newsnr>>h2");

  h1->SetLineColor(1);
  h2->SetLineColor(2);

  h1->Draw();
  h2->Draw("same");

  TLegend* leg_hist_1 = new TLegend(0.5,0.5,0.79,0.70);
  leg_hist_1->SetHeader("SNR and newSNR Triggers");
  leg_hist_1->AddEntry(h1,"SNR","l");
  leg_hist_1->AddEntry(h2,"newSNR","l");
  leg_hist_1->Draw();

  //Make Scatterplot of SNR v. Mass_total and newSNR v. Mass_total
  TCanvas* c2 = new TCanvas();
  //  gStyle->SetOptStat(0);
   c2->Divide(1,2);

  TH1F *h3 = new TH1F("h3","SNR v. Total Mass",n_bins,x_low,x_high);                                  
  TH1F *h4 = new TH1F("h4","newSNR v. Total Mass",n_bins,x_low,x_high); 

  c2->cd(1);
  clustered_cbc_trigs->Draw("snr:mass_tot>>h3");
 
  c2->cd(2);
  clustered_cbc_trigs->Draw("newsnr:mass_tot>>h4");

  h3->SetLineColor(3);                                                                               
  h4->SetLineColor(4);                                                                                               
  
  h4->Draw();                                                                                         
  h3->Draw();

  //  c2->SetLogy();

  return 0;
}

int main( int argc, char *argv[] )
{
  int c;
  int gps_start_time = 0;
  int gps_end_time = 0;
  Double_t cbc_trigger_snr_threshold = 0;
  size_t outputdir_len = 2048;
  size_t detector_len = 10;
  char outputdir[outputdir_len];
  char detector[detector_len];

  memset( outputdir, 0, outputdir_len * sizeof(char) );
  memset( detector, 0, detector_len * sizeof(char) );

  TApplication* rootapp = new TApplication("eveto", &argc, argv);

  while( 1 )
  {
    static struct option long_options[] = {
      //{"number", required_argument, 0, 'n' },
      {"gps-start-time", required_argument, 0, 's' },
      {"gps-end-time", required_argument, 0, 'e' },
      {"cbc-trigger-snr-threshold", required_argument, 0, 't' },
      {"detector", required_argument, 0, 'd' },      
      {"output-directory", required_argument, 0, 'o' },
      {0, 0, 0, 0}
    };
    int option_index = 0;

    c = getopt_long( argc, argv, "s:e:t:d:o:", long_options, &option_index );

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

      case '?':
        break;

      default:
        fprintf( stderr, "error between argument map and case statement\n" );
        abort();
    }
  }

  if ( ! gps_start_time || ! gps_end_time || ! cbc_trigger_snr_threshold || ! outputdir[0] || ! detector[0] )
  {
	  fprintf( stderr, "you forgot an argument\n" );
	  exit( 1 );
  }

  eveto_main( gps_start_time, gps_end_time, cbc_trigger_snr_threshold, detector, outputdir );

  rootapp->Run();
  return 0;
}

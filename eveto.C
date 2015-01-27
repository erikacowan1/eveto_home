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
    Float_t mtotal;
    Float_t mchirp;
    Float_t ttotal;
    Float_t eta;
    Float_t snr_sq;
 
   // Create strings for the SQL statements                                                           
    TString select_snr_chisq( "select sngl_inspiral.event_id,sngl_inspiral.snr,sngl_inspiral.chisq,sngl_inspiral.chisq_dof, sngl_inspiral.mass1,sngl_inspiral.mass2, sngl_inspiral.mtotal, sngl_inspiral.mchirp, sngl_inspiral.ttotal, sngl_inspiral.eta from sngl_inspiral where sngl_inspiral.ifo = 'L1'" );

    // Create a TTree to store the triggers                                                            
    TTree *tree = new TTree("T","single inspiral triggers");
    tree->Branch( "snr" , &snr, "snr/F" );
    tree->Branch( "chisq" , &chisq, "chisq/F" );
    tree->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
    tree->Branch( "newsnr" , &newsnr, "newsnr/F" );
    tree->Branch( "mass1", &mass1, "mass1/F" );
    tree->Branch( "mass2", &mass2, "mass2/F" );
    tree->Branch( "mtotal", &mtotal, "mtotal/F" );
    tree->Branch( "mchirp", &mchirp, "mchirp/F" );
    tree->Branch( "ttotal", &ttotal, "ttotal/F" );
    tree->Branch( "eta", &eta, "eta/F" );
    tree->Branch( "snr_sq", &snr_sq, "snr_sq/F" );
    
    // Create a connection to the sqlite database                                                      
    TSQLServer* serv = TSQLServer::Connect( db_filename, "", "" );

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
        //mass_tot = mass1 + mass2;
	snr_sq = snr*snr;

	// compute the new SNR                                                                         
	if ( chisq > chisqdof ) {
	  newsnr = snr * pow(((1.0 + pow((chisq/chisqdof), 3.0))/2.0), (-1.0 /6.0));
	} else {
	  newsnr = snr;
	}

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

int eveto_main( int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* output_path, char* sql_db, int number, int x_low, int x_high, int n_bins )
{
  //Read in Triggers
  TTree* clustered_cbc_trigs = get_triggers_from_database( 1, sql_db );
  //Int_t n_bins = 50;
  //Int_t x_low = 0;
  //Int_t x_high = 20;
  //Int_t y_low = 0;
  // Int_t y_high = 20;
  //Create Overlay of SNR and newSNR histograms
//  TCanvas* c1 = new TCanvas();
  TH1F *h1 = new TH1F("h1","Background CBC Triggers",n_bins,x_low,x_high);
  TH1F *h2 = new TH1F("h2","Background CBC Triggers",n_bins,x_low,x_high);
  h1->GetXaxis()->SetTitle("Background CBC Triggers");
  h1->GetYaxis()->SetTitle("# triggers per bin"); 
  h2->GetXaxis()->SetTitle("Background CBC Triggers");
  h2->GetYaxis()->SetTitle("# triggers per bin");
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

/*
TCanvas* canvas1 = new TCanvas("canvas1", "Histogram stacking test");
canvas1->Divide(1,1);
canvas1->cd(1);
gPad->SetFrameFillColor(17);
 THStack *a = new THStack("a","Stacked 2D histograms");
TH2F *h3 = new TH2F("h3","SNR v. Total Mass",n_bins,x_low,x_high,n_bins,y_low,y_high);
h3->SetFillColor(38);
clustered_cbc_trigs->Draw("snr:mtotal>>h3");
TH2F *h3a = new TH2F("h3a","SNR v. Total Mass",n_bins,x_low,x_high,n_bins,y_low,y_high);
h3a->SetFillColor(46);
clustered_cbc_trigs->Draw("snr:mchirp>>h3a");
a->Add(h3);
a->Add(h3a);
a->Draw();
*/

//chisq vs. snr_sq
TCanvas* c1 = new TCanvas();
TH1F *ch2 = new TH1F("ch2","Chisq vs. SNR_sq",n_bins,x_low,x_high);
clustered_cbc_trigs->Draw("chisq:snr_sq>>ch2");
ch2->Draw();
/*
//Scatterplots- Overlay
//SNR vs. (Chirp, Total) Mass
TCanvas* c2 = new TCanvas("c2");
c2->Divide(1,1);
c2->cd(1);
TH1F *h3 = new TH1F("h3","SNR v. Total Mass",n_bins,x_low,x_high);
//TH1F *h3a = new TH1F("h3a","SNR v. Chirp Mass",n_bins,x_low,x_high);
clustered_cbc_trigs->Draw("snr:mtotal>>h3");
h3->SetMarkerColor(kGreen);
h3->Draw();
gPad->Update();
clustered_cbc_trigs->Draw("snr:mchirp", "", "same");

//SNR vs. (ttotal, eta) 
TCanvas* c3 = new TCanvas("c3");
c3->Divide(1,1);
c3->cd(1);
TH1F *h4 = new TH1F("h4","newSNR v. Total Mass",n_bins,x_low,x_high);
clustered_cbc_trigs->Draw("snr:ttotal>>h5");
h4->SetMarkerColor(kGreen);
h4->Draw();
gPad->Update();
clustered_cbc_trigs->Draw("snr:eta", "", "same");

//newSNR vs. (Chirp, Total) Mass
TCanvas* c4 = new TCanvas("c4");
c4->Divide(1,1);
c4->cd(1);
TH1F *h5 = new TH1F("h5","newSNR v. Total Mass",n_bins,x_low,x_high);
//TH1F *h3a = new TH1F("h3a","SNR v. Chirp Mass",n_bins,x_low,x_high);
clustered_cbc_trigs->Draw("newsnr:mtotal>>h5");
h5->SetMarkerColor(kGreen);
h5->Draw();
gPad->Update();
clustered_cbc_trigs->Draw("newsnr:mchirp", "", "same");

//newSNR vs. (ttotal, eta) 
TCanvas* c5 = new TCanvas("c5");
c5->Divide(1,1);
c5->cd(1);
TH1F *h6 = new TH1F("h6","newSNR v. Total Mass",n_bins,x_low,x_high);
clustered_cbc_trigs->Draw("newsnr:ttotal>>h6");
h6->SetMarkerColor(kGreen);
h6->Draw();
gPad->Update();
clustered_cbc_trigs->Draw("newsnr:eta", "", "same");

*/
//Make Scatterplot of SNR v. Mass_total and newSNR v. Mass_total
  //TCanvas* c2 = new TCanvas("c2");
  //  gStyle->SetOptStat(0);
  //c2->Divide(2,2);
  //c2->cd(1);
  //TH1F *h3 = new TH1F("h3","SNR v. Total Mass",n_bins,x_low,x_high);
  //clustered_cbc_trigs->Draw("snr:mtotal>>h3");  
 
  //h3->GetXaxis()->SetTitle("mtotal,mchirp");
  //h3->GetYaxis()->SetTitle("SNR");
  //h3->SetMarkerColor(kGreen);
  //gPad->Update();                              
  //TH1F *h3a = new TH1F("h3a","SNR v. Chirp Mass",n_bins,x_low,x_high);
  //clustered_cbc_trigs->Draw("snr:mtotal>>h3");  
  //clustered_cbc_trigs->Draw("snr:mchirp", "","same");
  //gPad->Update();
  /*h3->SetLineColor(1);
  h3a->SetLineColor(2);
  h3->Draw();
  h3a->Draw("same");
*/


/*
  TCanvas* c3 = new TCanvas();
  //c2->cd(3);
  TH1F *h4 = new TH1F("h4","newSNR v. Total Mass",n_bins,x_low,x_high); 
  TH1F *h4a = new TH1F("h4a","newSNR v. Chirp Mass",n_bins,x_low,x_high);  
  clustered_cbc_trigs->Draw("newsnr:mtotal>>h4");
  clustered_cbc_trigs->Draw("newsnr:mchirp>>h4a");
  h4->SetLineColor(1);
  h4a->SetLineColor(2);
  h4->Draw();
  h4a->Draw("same");

TCanvas* c4 = new TCanvas();
 // c2->cd(2);
  TH1F *h5 = new TH1F("h5","SNR v. Total Time",n_bins,x_low,x_high);
  TH1F *h5a = new TH1F("h5a","SNR v. Eta",n_bins,x_low,x_high);  
  clustered_cbc_trigs->Draw("snr:ttotal>>h5");
  clustered_cbc_trigs->Draw("snr:eta>>h5a");
  h5->SetLineColor(1);
  h5a->SetLineColor(2);
  h5->Draw();
  h5a->Draw("same");


TCanvas* c5 = new TCanvas();
 // c2->cd(4);
  TH1F *h6 = new TH1F("h6","newSNR v. Total Time",n_bins,x_low,x_high);
  TH1F *h6a = new TH1F("h6a","newSNR v. Eta",n_bins,x_low,x_high);
  clustered_cbc_trigs->Draw("newsnr:ttotal>>h6");
  clustered_cbc_trigs->Draw("newsnr:eta>>h6a");
  h6->SetLineColor(1);
  h6a->SetLineColor(2);
  h6->Draw();
  h6a->Draw("same");
*/

  //h3->SetBit(TH1::kCanRebin);
  //  c2->SetLogy();

  return 0;
}

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

#include <iostream>
#include <string>
#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TLegend.h>
#include <TTree.h>
#include <TSQLServer.h>
#include <TSQLStatement.h>

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

	TString select_snr_chisq( "select sngl_inspiral.event_id,sngl_inspiral.snr,sngl_inspiral.chisq,sngl_inspiral.chisq_dof, sngl_inspiral.mass1,sngl_inspiral.mass2, sngl_inspiral.mtotal, sngl_inspiral.mchirp, sngl_inspiral.ttotal, sngl_inspiral.eta from experiment_summary,experiment_map,coinc_event,coinc_event_map,sngl_inspiral where sngl_inspiral.ifo = 'H1'" );
	TString zero_lag_clause( " and experiment_summary.datatype = 'all_data'" );
	TString time_slide_clause( " and experiment_summary.datatype = 'slide'" );
	TString snr_chisq_join_clause( " and experiment_summary.experiment_summ_id = experiment_map.experiment_summ_id and experiment_map.coinc_event_id = coinc_event.coinc_event_id and coinc_event.coinc_event_id = coinc_event_map.coinc_event_id and coinc_event_map.event_id = sngl_inspiral.event_id" );
  

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
	tree->Branch( "mtotal", &mtotal, "mtotal/F" );
	tree->Branch( "mchirp", &mchirp, "mchirp/F" );
	tree->Branch( "ttotal", &ttotal, "ttotal/F" );
	tree->Branch( "eta", &eta, "eta/F" );
	tree->Branch( "snr_sq", &snr_sq, "snr_sq/F" );


// Create a connection to the sqlite database

  TSQLServer* serv = TSQLServer::Connect( db_filename, "", "" );

if ( background ) {
    cout << "loading background triggers" << endl;
    select_snr_chisq = select_snr_chisq + time_slide_clause + snr_chisq_join_clause;
} 
else {
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

void post_eveto() {

  
// Read in the triggers from each database to TTree

  TTree* ss_s5_cat4_bg = get_triggers_from_database( 1, "sqlite://single_stage_s5/H1H2L1-FULL_DATA_CAT_4_VETO__CLUSTERED_CBC_RESULTS-852393970-2419200.sqlite" );

  TTree* ss_gaussian_cat4_bg = get_triggers_from_database( 1, "sqlite://single_stage_gaussian/H1H2L1-FULL_DATA_CAT_4_VETO__CLUSTERED_CBC_RESULTS-852393970-2419200.sqlite");


  // Read in the triggers from each database to TTree

  TTree* ss_s5_cat4_fg = get_triggers_from_database( 0, "sqlite://single_stage_s5/H1H2L1-FULL_DATA_CAT_4_VETO__CLUSTERED_CBC_RESULTS-852393970-2419200.sqlite" );

  TTree* ss_gaussian_cat4_fg = get_triggers_from_database( 0, "sqlite://single_stage_gaussian/H1H2L1-FULL_DATA_CAT_4_VETO__CLUSTERED_CBC_RESULTS-852393970-2419200.sqlite");


// Plot Background CBC Triggers
  TCanvas* c1 = new TCanvas();
  c1->SetLogy();
  //Int_t n_bins = 70;
  //Double_t x_low = 5.5;
  //Double_t x_high = 9.0;
  TH1F *h1 = new TH1F("h1","Background CBC Triggers",n_bins,x_low,x_high);
  TH1F *h2 = new TH1F("h2","Background CBC Triggers",n_bins,x_low,x_high);

  h1->GetXaxis()->SetTitle("newSNR for Background CBC Triggers");
  h1->GetYaxis()->SetTitle("# triggers per bin");
  h2->GetXaxis()->SetTitle("newSNR for Background CBC Triggers");
  h2->GetYaxis()->SetTitle("# triggers per bin");

  ss_s5_cat4_bg->Draw("newsnr>>h1");
  ss_gaussian_cat4_bg->Draw("newsnr>>h2");

  h1->SetLineColor(1);
  h2->SetLineColor(2);

  h1->Draw();
  h2->Draw("same");

  TLegend* leg_hist_1 = new TLegend(0.5,0.5,0.79,0.70);
  leg_hist_1->SetHeader("New SNR Triggers");
  leg_hist_1->AddEntry(h1,"Single Stage S5","l");
  leg_hist_1->AddEntry(h2,"Single Stage Gaussian","l");
  leg_hist_1->Draw();


// Plot Foreground CBC Triggers
  TCanvas* c2 = new TCanvas();
  TH1F *h3 = new TH1F("h3","Foreground CBC Triggers",n_bins,x_low,x_high);
  TH1F *h4 = new TH1F("h4","Foreground CBC Triggers",n_bins,x_low,x_high);

  h3->GetXaxis()->SetTitle("newSNR for Foreground CBC Triggers");
  h3->GetYaxis()->SetTitle("# triggers per bin");
  h4->GetXaxis()->SetTitle("newSNR for Foreground CBC Triggers");
  h4->GetYaxis()->SetTitle("# triggers per bin");

  ss_s5_cat4_fg->Draw("newsnr>>h3");
  ss_gaussian_cat4_fg->Draw("newsnr>>h4");

  h3->SetLineColor(3);
  h4->SetLineColor(4);

  h4->Draw();
  h3->Draw("same");

  TLegend* leg_hist_2 = new TLegend(0.5,0.5,0.79,0.70);
  leg_hist_2->SetHeader("New SNR Triggers");
  leg_hist_2->AddEntry(h3,"Single Stage S5","l");
  leg_hist_2->AddEntry(h4,"Single Stage Gaussian","l");
  leg_hist_2->Draw();

// Plot Eta v. MChirp, Background CBC Triggers
  TCanvas* c3 = new TCanvas();
  TH2F *h5 = new TH2F("h5","Eta v. MChirp Background CBC Triggers", n_bins, x_low, x_high);

  h5->GetXaxis()->SetTitle("MChirp"); //need to add units
  h5->GetYaxis()->SetTitle("Eta");
  ss_s5_cat4_bg->Draw("eta:mchirp>>h5");


// Plot Mass1 v. Mass2
  TCanvas* c4 = new TCanvas();
  TH2F *h6 = new TH2F("h6","Mass1 v. Mass2 Background CBC Triggers", n_bins, x_low, x_high);

  h6->GetXaxis()->SetTitle("Mass2"); //need to add units
  h6->GetYaxis()->SetTitle("Mass1");
  ss_s5_cat4_bg->Draw("mass1:mass2>>h6");

// Plot Chisq v. SNRsq
  TCanvas* c5 = new TCanvas(); 
  TH2F *h7 = new TH2F("h7","Chisq v. SNR_sq Background CBC Triggers", n_bins, x_low, x_high);

  h7->GetXaxis()->SetTitle("snr_sq"); //need to add units
  h7->GetYaxis()->SetTitle("chisq");
  ss_s5_cat4_bg->Draw("chisq:snr_sq>>h7");


  return;
}

int main( int argc, char *argv[] )
{
  TApplication* rootapp = new TApplication("post_eveto",&argc, argv);
  histogram_cbc_triggers();
  rootapp->Run();
  return 0;
}


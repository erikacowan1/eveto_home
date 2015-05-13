//////////////////////////////////
//				//
//        post_eveto.C		//
//        			//
//    Analysis and Plotting     //
//    for eveto.C, designed     //
//    for CBC searches.		//
//				//
//////////////////////////////////				

#include <iostream>
#include <string>
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

  gSystem->ProcessEvents();

  TImage *img = TImage::Create();
  img->FromPad(c1);
  img->WriteImage("newSNR_background_cbc_trigs.png");
  
  delete img;



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

  gSystem->ProcessEvents();

  TImage *img = TImage::Create();
  img->FromPad(c2);
  img->WriteImage("newSNR_foreground_cbc_trigs.png");
  
  delete img;

// Plot Eta v. MChirp, Background CBC Triggers
  TCanvas* c3 = new TCanvas();
  TH2F *h5 = new TH2F("h5","Eta v. MChirp Background CBC Triggers", n_bins, x_low, x_high);

  gSystem->ProcessEvents();

  h5->GetXaxis()->SetTitle("MChirp"); //need to add units
  h5->GetYaxis()->SetTitle("Eta");
  ss_s5_cat4_bg->Draw("eta:mchirp>>h5");
  
  delete img;

// Plot Mass1 v. Mass2
  TCanvas* c4 = new TCanvas();
  TH2F *h6 = new TH2F("h6","Mass1 v. Mass2 Background CBC Triggers", n_bins, x_low, x_high);

  h6->GetXaxis()->SetTitle("Mass2"); //need to add units
  h6->GetYaxis()->SetTitle("Mass1");
  ss_s5_cat4_bg->Draw("mass1:mass2>>h6");

  gSystem->ProcessEvents();

  TImage *img = TImage::Create();
  img->FromPad(c4);
  img->WriteImage("mass1_v_mass2.png");

  delete img;

// Plot Chisq v. SNR_sq
  TCanvas* c5 = new TCanvas(); 
  TH2F *h7 = new TH2F("h7","Chisq v. SNR_sq Background CBC Triggers", n_bins, x_low, x_high);

  h7->GetXaxis()->SetTitle("snr_sq"); //need to add units
  h7->GetYaxis()->SetTitle("chisq");
  ss_s5_cat4_bg->Draw("chisq:snr_sq>>h7");

  gSystem->ProcessEvents();

  TImage *img = TImage::Create();
  img->FromPad(c5);
  img->WriteImage("chisq_v_snrsq.png");

  delete img;

  return;
}

int main( int argc, char *argv[] )
{
  TApplication* rootapp = new TApplication("post_eveto",&argc, argv);
  histogram_cbc_triggers();
  rootapp->Run();
  return 0;
}


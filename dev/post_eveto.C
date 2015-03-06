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

//NEEDS TO BE PUT IN LOOP SO AS TO PLOT/EXPORT ALL VERSIONS//

void post_eveto() {

//Import cbc trigger tree, currently is -same- directory as eveto.C code- to be changed 
  TFile *_file0=TFile::Open("cbc-trigger-original.root");
  TTree* tree = (TTree*)_file0->Get("ntuple"); //or whatever the ntuple is *actually* called

//Import aux trigger tree, currently in -same- directory as post-eveto.C, rename tree,tree1 to more appropriate names
  TFile *_file1=TFile::Open("aux-trigger-original.root");
  TTree* tree1 = (TTree*)_file1->Get("ntuple"); //or whatever the ntuple is *actually* called

// Plot CBC Triggers
  TCanvas* c1 = new TCanvas();
  c1->SetLogy();
  //Int_t n_bins = 70;
  //Double_t x_low = 5.5;
  //Double_t x_high = 9.0;
  TH1F *h1 = new TH1F("h1","CBC Triggers",n_bins,x_low,x_high);
  TH1F *h2 = new TH1F("h2","Background CBC Triggers",n_bins,x_low,x_high);

  h1->GetXaxis()->SetTitle("newSNR for CBC Triggers");
  h1->GetYaxis()->SetTitle("# triggers per bin");
  h2->GetXaxis()->SetTitle("newSNR for Aux Triggers");
  h2->GetYaxis()->SetTitle("# triggers per bin");

  tree->Draw("newsnr>>h1");
  tree1->Draw("newsnr>>h2");
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



// Pustered_cbc_trigs = get_triggers_from_database( 1, sql_db );ot Foreground CBC Triggers
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


//MISC. CODE TO BE SORTED LATER


//int eveto_main( int gps_start_time, int gps_end_time, Double_t cbc_trigger_snr_threshold, char* detector, char* output_path, char* sql_db, int number, int x_low, int x_high, int n_bins )
//  //Read in Triggers
//    TTree* clustered_cbc_trigs = get_triggers_from_database( 1, sql_db );

//SNR vs. (Chirp, Total) Mass
//TCanvas* c2 = new TCanvas("c2");
//c2->Divide(1,1);
//c2->cd(1);
//TH1F *h3 = new TH1F("h3","SNR v. Total Mass",n_bins,x_low,x_high);
////TH1F *h3a = new TH1F("h3a","SNR v. Chirp Mass",n_bins,x_low,x_high);
//clustered_cbc_trigs->Draw("snr:mtotal>>h3");
//h3->SetMarkerColor(kGreen);
//h3->Draw();
//gPad->Update();
//clustered_cbc_trigs->Draw("snr:mchirp", "", "same");
//
////SNR vs. (ttotal, eta) 
//TCanvas* c3 = new TCanvas("c3");
//c3->Divide(1,1);
//c3->cd(1);
//TH1F *h4 = new TH1F("h4","newSNR v. Total Mass",n_bins,x_low,x_high);
//clustered_cbc_trigs->Draw("snr:ttotal>>h5");
//h4->SetMarkerColor(kGreen);
//h4->Draw();
//gPad->Update();
//clustered_cbc_trigs->Draw("snr:eta", "", "same");
//
////newSNR vs. (Chirp, Total) Mass
//TCanvas* c4 = new TCanvas("c4");
//c4->Divide(1,1);
//c4->cd(1);
//TH1F *h5 = new TH1F("h5","newSNR v. Total Mass",n_bins,x_low,x_high);
////TH1F *h3a = new TH1F("h3a","SNR v. Chirp Mass",n_bins,x_low,x_high);
//clustered_cbc_trigs->Draw("newsnr:mtotal>>h5");
//h5->SetMarkerColor(kGreen);
//h5->Draw();
//gPad->Update();
//clustered_cbc_trigs->Draw("newsnr:mchirp", "", "same");
//
////newSNR vs. (ttotal, eta) 
//TCanvas* c5 = new TCanvas("c5");
//c5->Divide(1,1);
//c5->cd(1);
//TH1F *h6 = new TH1F("h6","newSNR v. Total Mass",n_bins,x_low,x_high);
//clustered_cbc_trigs->Draw("newsnr:ttotal>>h6");
//h6->SetMarkerColor(kGreen);
//h6->Draw();
//gPad->Update();
//clustered_cbc_trigs->Draw("newsnr:eta", "", "same");
//
///Make Scatterplot of SNR v. Mass_total and newSNR v. Mass_total
//  //TCanvas* c2 = new TCanvas("c2");
//    //  gStyle->SetOptStat(0);
//      //c2->Divide(2,2);
//        //c2->cd(1);
//          //TH1F *h3 = new TH1F("h3","SNR v. Total Mass",n_bins,x_low,x_high);
//            //clustered_cbc_trigs->Draw("snr:mtotal>>h3");  
//
//              //h3->GetXaxis()->SetTitle("mtotal,mchirp");
//                //h3->GetYaxis()->SetTitle("SNR");
//                  //h3->SetMarkerColor(kGreen);
//                    //gPad->Update();                              
//                      //TH1F *h3a = new TH1F("h3a","SNR v. Chirp Mass",n_bins,x_low,x_high);
//                        //clustered_cbc_trigs->Draw("snr:mtotal>>h3");  
//                          //clustered_cbc_trigs->Draw("snr:mchirp", "","same");
//                            //gPad->Update();
//                              /*h3->SetLineColor(1);
//                                h3a->SetLineColor(2);
//                                  h3->Draw();
//                                    h3a->Draw("same");
//                                    */
//
//
//                                    /*
//                                      TCanvas* c3 = new TCanvas();
//                                        //c2->cd(3);
//                                          TH1F *h4 = new TH1F("h4","newSNR v. Total Mass",n_bins,x_low,x_high); 
//                                            TH1F *h4a = new TH1F("h4a","newSNR v. Chirp Mass",n_bins,x_low,x_high);  
//                                              clustered_cbc_trigs->Draw("newsnr:mtotal>>h4");
//                                                clustered_cbc_trigs->Draw("newsnr:mchirp>>h4a");
//                                                  h4->SetLineColor(1);
//                                                    h4a->SetLineColor(2);
//                                                      h4->Draw();
//                                                        h4a->Draw("same");
//
//                                                        TCanvas* c4 = new TCanvas();
//                                                         // c2->cd(2);
//                                                           TH1F *h5 = new TH1F("h5","SNR v. Total Time",n_bins,x_low,x_high);
//                                                             TH1F *h5a = new TH1F("h5a","SNR v. Eta",n_bins,x_low,x_high);  
//                         clustered_cbc_trigs->Draw("snr:ttotal>>h5");
//                                                                 clustered_cbc_trigs->Draw("snr:eta>>h5a");
//
// h5->SetLineColor(1);
//   h5a->SetLineColor(2);
//     h5->Draw();
//       h5a->Draw("same");
//
//
//       TCanvas* c5 = new TCanvas();
//        // c2->cd(4);
//          TH1F *h6 = new TH1F("h6","newSNR v. Total Time",n_bins,x_low,x_high);
//            TH1F *h6a = new TH1F("h6a","newSNR v. Eta",n_bins,x_low,x_high);
//              clustered_cbc_trigs->Draw("newsnr:ttotal>>h6");
//                clustered_cbc_trigs->Draw("newsnr:eta>>h6a");
//                  h6->SetLineColor(1);
//                    h6a->SetLineColor(2);
//                      h6->Draw();
//                        h6a->Draw("same");
//                        */
//
//                          //h3->SetBit(TH1::kCanRebin);
//                            //  c2->SetLogy();
//

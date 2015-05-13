//////////////////////////////////
//				//
//        post_eveto.C		//
//        			//
//    Analysis and Plotting     //
//    for eveto.C, designed     //
//    for CBC searches.		//
//				//
//////////////////////////////////				

//NEEDS TO BE PUT IN LOOP SO AS TO PLOT/EXPORT ALL VERSIONS//

void post_eveto(TTree* cbc_trigger_tree_ptr,
      TTree* omicron_trigger_tree_ptr,
bool verbose) {

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


#include "cbc_eveto.h"

//function that does a simple significance calculation
int eveto::calc_dumb_sig(
    TTree* cbc_trigger_tree_ptr,
    TTree* omicron_trigger_tree_ptr,
    float dumb_time_seg,
    bool verbose )
{
  Long64_t n;
  int num_coinc_triggers = 0;
  Double_t start_time, end_time;
  Float_t snr, chisq, chisqdof, newsnr, snr_sq, mass1, mass2, mtotal, mchirp, eta, ttotal;

  cbc_trigger_tree_ptr->SetBranchAddress("start_time",&start_time);
  cbc_trigger_tree_ptr->SetBranchAddress("end_time",&end_time);
  cbc_trigger_tree_ptr->SetBranchAddress("snr",&snr);
  cbc_trigger_tree_ptr->SetBranchAddress("chisq",&chisq);
  cbc_trigger_tree_ptr->SetBranchAddress("chisqdof",&chisqdof);
  cbc_trigger_tree_ptr->SetBranchAddress("newsnr",&newsnr);
  cbc_trigger_tree_ptr->SetBranchAddress("snr_sq",&snr_sq);
  cbc_trigger_tree_ptr->SetBranchAddress("mass1",&mass1);
  cbc_trigger_tree_ptr->SetBranchAddress("mass2",&mass2);
  cbc_trigger_tree_ptr->SetBranchAddress("mtotal",&mtotal);
  cbc_trigger_tree_ptr->SetBranchAddress("mchirp",&mchirp);
  cbc_trigger_tree_ptr->SetBranchAddress("eta",&eta);
  cbc_trigger_tree_ptr->SetBranchAddress("ttotal",&ttotal);

  int num_cbc_triggers = cbc_trigger_tree_ptr->GetEntries();	

  for (Int_t c=0; c<num_cbc_triggers; ++c) {
    cbc_trigger_tree_ptr->GetEntry(c);

    char *time_window_string = new char[256];
    snprintf( time_window_string, 256, "(tend > %f) && (tend < %f)", 
        end_time - dumb_time_seg, end_time + dumb_time_seg );

    if ( verbose ) std::cout << time_window_string << std::endl;

    n = omicron_trigger_tree_ptr->Draw("tend", time_window_string, "goff" );

    if ( n == -1 ) {
       std::cerr << "calc_dumb_sig: error drawing veto triggers" << std::endl;
       abort();
       } else {
         if ( verbose ) std::cout << "   Found " << n << " veto triggers" << std::endl;
         num_coinc_triggers += n;
       }
  }

  int num_omicron_triggers = omicron_trigger_tree_ptr->GetEntries();

  return (float)num_coinc_triggers/(float)num_omicron_triggers;
}


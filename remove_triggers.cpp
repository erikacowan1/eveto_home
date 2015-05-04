#include "cbc_eveto.h"

//code to remove veteod triggers from cbc trigs//
//does this code also remove segments
//remove_triggers(cbc_trigs_round[r-1], omicron_trigs_round[r-1][max_sig_index],cbc_segs_round[r-1], omicron_segs_round[r-1][max_sig_index]);

//function that does a simple significance calculation
TTree* eveto::remove_triggers(
    TTree* cbc_trigger_tree_ptr,
    TTree* omicron_trigger_tree_ptr,
//    TTree* cbc_segs_tree_ptr,
//    TTree* omicron_segs_tree_ptr,
    bool verbose )
    {

  // Set up the variables to access the input cbc triggers
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

  // Set up the variables to access the input omicron triggers
  double Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
  Long64_t Cfirstentry, Csize;

  omicron_trigger_tree_ptr->SetBranchAddress("time",       &Ctime);
  omicron_trigger_tree_ptr->SetBranchAddress("tstart",     &Ctstart);
  omicron_trigger_tree_ptr->SetBranchAddress("tend",       &Ctend);
  omicron_trigger_tree_ptr->SetBranchAddress("frequency",  &Cfreq);
  omicron_trigger_tree_ptr->SetBranchAddress("fstart",     &Cfstart);
  omicron_trigger_tree_ptr->SetBranchAddress("fend",       &Cfend);
  omicron_trigger_tree_ptr->SetBranchAddress("snr",        &Csnr);
  omicron_trigger_tree_ptr->SetBranchAddress("amplitude",  &Camp);
  omicron_trigger_tree_ptr->SetBranchAddress("q",          &Cq);
  omicron_trigger_tree_ptr->SetBranchAddress("firstentry", &Cfirstentry);
  omicron_trigger_tree_ptr->SetBranchAddress("size",       &Csize);

  int num_omicron_triggers = omicron_trigger_tree_ptr->GetEntries();

  // Create a new tree that will store the output data
  TTree* cbc_trigger_tree_minus_trigs = new TTree("cbc-triggers","cbc-triggers");

  // Create the branches for the tree
  cbc_trigger_tree_minus_trigs->Branch( "start_time" , &start_time, "start_time/D" );
  cbc_trigger_tree_minus_trigs->Branch( "end_time" , &end_time, "end_time/D" );
  cbc_trigger_tree_minus_trigs->Branch( "snr" , &snr, "snr/F" );
  cbc_trigger_tree_minus_trigs->Branch( "chisq" , &chisq, "chisq/F" );
  cbc_trigger_tree_minus_trigs->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
  cbc_trigger_tree_minus_trigs->Branch( "newsnr" , &newsnr, "newsnr/F" );
  cbc_trigger_tree_minus_trigs->Branch( "snr_sq" , &snr_sq, "snr_sq/F" );
  cbc_trigger_tree_minus_trigs->Branch( "mass1", &mass1, "mass1/F" );
  cbc_trigger_tree_minus_trigs->Branch( "mass2", &mass2, "mass2/F" );
  cbc_trigger_tree_minus_trigs->Branch( "mtotal", &mtotal, "mtotal/F" );
  cbc_trigger_tree_minus_trigs->Branch( "mchirp", &mchirp, "mchirp/F" );
  cbc_trigger_tree_minus_trigs->Branch( "eta", &eta, "eta/F" );
  cbc_trigger_tree_minus_trigs->Branch( "ttotal", &ttotal, "ttotal/F" );

  for (Int_t c=0; c<num_cbc_triggers; ++c) {

    cbc_trigger_tree_ptr->GetEntry(c);

      for(Int_t o=0; o<num_omicron_triggers; ++o) {
       omicron_trigger_tree_ptr->GetEntry(o);

        if(Ctend > start_time && Ctstart < end_time) {
	  cbc_trigger_tree_minus_trigs->Fill();
        }
      }
  }

  return cbc_trigger_tree_minus_trigs;
}            

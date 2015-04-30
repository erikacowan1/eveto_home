#include "cbc_eveto.h"

//code to remove veteod triggers from cbc trigs//
//does this code also remove segments
//remove_triggers(cbc_trigs_round[r-1], omicron_trigs_round[r-1][max_sig_index],cbc_segs_round[r-1], omicron_segs_round[r-1][max_sig_index]);

//function that does a simple significance calculation
int eveto::remove_triggers(
    TTree* cbc_trigger_tree_ptr,
    TTree* omicron_trigger_tree_ptr,
    TTree*   cbc_trigger_tree_minus_trigs, 
    TTree* omicron_trigger_tree_minus_trigs, 
//    TTree* cbc_segs_tree_ptr,
//    TTree* omicron_segs_tree_ptr,
    bool verbose )
    {
Long64_t n=0;
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
  double Cmtime, Ctmstart, Ctmend, Cmfreq, Cfmstart, Cfmend, Cmsnr, Cmamp, Cmq;
  Long64_t Cmfirstentry, Cmsize;
  cbc_trigger_tree_minus_trigs->Branch("time",       &Cmtime, "time/D");
  cbc_trigger_tree_minus_trigs->Branch("tstart",     &Ctmstart,    "tstart/D");
  cbc_trigger_tree_minus_trigs->Branch("tend",       &Ctmend,      "tend/D");
  cbc_trigger_tree_minus_trigs->Branch("frequency",  &Cmfreq,      "frequency/D");
  cbc_trigger_tree_minus_trigs->Branch("fstart",     &Cfmstart,    "fstart/D");
  cbc_trigger_tree_minus_trigs->Branch("fend",       &Cfmend,      "fend/D");
  cbc_trigger_tree_minus_trigs->Branch("snr",        &Cmsnr,       "snr/D");
  cbc_trigger_tree_minus_trigs->Branch("amplitude",  &Cmamp,       "amplitude/D");
  cbc_trigger_tree_minus_trigs->Branch("q",          &Cmq,         "q/D");
  cbc_trigger_tree_minus_trigs->Branch("firstentry", &Cmfirstentry,"firstentry/L");
  cbc_trigger_tree_minus_trigs->Branch("size",       &Cmsize,      "size/L");

  Cmfirstentry=-1;
  Cmtend=0.0;

  double mtime, tmstart, tmend, mfreq, fmstart, fmend, msnr, mamp, mq;
  Long64_t mfirstentry, msize;
  omicron_trigger_tree_minus_trigs->Branch("time",       &mtime, "time/D");
  omicron_trigger_tree_minus_trigs->Branch("tstart",     &tmstart,    "tstart/D");
  omicron_trigger_tree_minus_trigs->Branch("tend",       &tmend,      "tend/D");
  omicron_trigger_tree_minus_trigs->Branch("frequency",  &mfreq,      "frequency/D");
  omicron_trigger_tree_minus_trigs->Branch("fstart",     &fmstart,    "fstart/D");
  omicron_trigger_tree_minus_trigs->Branch("fend",       &fmend,      "fend/D");
  omicron_trigger_tree_minus_trigs->Branch("snr",        &msnr,       "snr/D");
  omicron_trigger_tree_minus_trigs->Branch("amplitude",  &mamp,       "amplitude/D");
  omicron_trigger_tree_minus_trigs->Branch("q",          &mq,         "q/D");
  omicron_trigger_tree_minus_trigs->Branch("firstentry", &mfirstentry,"firstentry/L");
  omicron_trigger_tree_minus_trigs->Branch("size",       &msize,      "size/L");



  mfirstentry=-1;
  mtend=0.0;
    cbc_trigger_tree_ptr->GetEntry(c);

      for(Int_t o=0; o<num_omicron_triggers; ++o) {
       omicron_trigger_tree_ptr->GetEntry(o);

        if(Ctend > start_time && Ctstart < end_time) {
	  cbc_trigger_tree_minus_trigs->Fill();
          omicron_trigger_tree_minus_trigs->Fill();
        }
        else{
        std::cerr << "Remove Trigs algorithm not doing its job" << std::endl;
        }
      }

  }

}            

#include <limits>
#include <iomanip>
#include "cbc_eveto.h"

//code to remove veteod triggers from cbc trigs//
//does this code also remove segments
//remove_triggers(cbc_trigs_round[r-1], omicron_trigs_round[r-1][max_sig_index],cbc_segs_round[r-1], omicron_segs_round[r-1][max_sig_index]);

TTree* eveto::remove_cbc_triggers(
    TTree* cbc_trigger_tree_in_ptr,
    TTree* omicron_trigger_tree_veto_ptr,
//    TTree* cbc_segs_tree_ptr,
//    TTree* omicron_segs_tree_ptr,
    bool verbose )
    {

  // Set up the variables to access the input cbc triggers
  Double_t start_time, end_time;
  Float_t snr, chisq, chisqdof, newsnr, snr_sq, mass1, mass2, mtotal, mchirp, eta, ttotal;

  cbc_trigger_tree_in_ptr->SetBranchAddress("start_time",&start_time);
  cbc_trigger_tree_in_ptr->SetBranchAddress("end_time",&end_time);
  cbc_trigger_tree_in_ptr->SetBranchAddress("snr",&snr);
  cbc_trigger_tree_in_ptr->SetBranchAddress("chisq",&chisq);
  cbc_trigger_tree_in_ptr->SetBranchAddress("chisqdof",&chisqdof);
  cbc_trigger_tree_in_ptr->SetBranchAddress("newsnr",&newsnr);
  cbc_trigger_tree_in_ptr->SetBranchAddress("snr_sq",&snr_sq);
  cbc_trigger_tree_in_ptr->SetBranchAddress("mass1",&mass1);
  cbc_trigger_tree_in_ptr->SetBranchAddress("mass2",&mass2);
  cbc_trigger_tree_in_ptr->SetBranchAddress("mtotal",&mtotal);
  cbc_trigger_tree_in_ptr->SetBranchAddress("mchirp",&mchirp);
  cbc_trigger_tree_in_ptr->SetBranchAddress("eta",&eta);
  cbc_trigger_tree_in_ptr->SetBranchAddress("ttotal",&ttotal);

  int num_cbc_triggers = cbc_trigger_tree_in_ptr->GetEntries();

  // Set up the variables to access the input omicron triggers
  double Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
  Long64_t Cfirstentry, Csize;

  omicron_trigger_tree_veto_ptr->SetBranchAddress("time",       &Ctime);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("tstart",     &Ctstart);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("tend",       &Ctend);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("frequency",  &Cfreq);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("fstart",     &Cfstart);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("fend",       &Cfend);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("snr",        &Csnr);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("amplitude",  &Camp);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("q",          &Cq);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("firstentry", &Cfirstentry);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("size",       &Csize);

  int num_omicron_triggers = omicron_trigger_tree_veto_ptr->GetEntries();

  // Create a new tree that will store the output data
  TTree* cbc_trigger_tree_out_ptr = new TTree("cbc-triggers","cbc-triggers");

  // Create the branches for the tree
  cbc_trigger_tree_out_ptr->Branch( "start_time" , &start_time, "start_time/D" );
  cbc_trigger_tree_out_ptr->Branch( "end_time" , &end_time, "end_time/D" );
  cbc_trigger_tree_out_ptr->Branch( "snr" , &snr, "snr/F" );
  cbc_trigger_tree_out_ptr->Branch( "chisq" , &chisq, "chisq/F" );
  cbc_trigger_tree_out_ptr->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
  cbc_trigger_tree_out_ptr->Branch( "newsnr" , &newsnr, "newsnr/F" );
  cbc_trigger_tree_out_ptr->Branch( "snr_sq" , &snr_sq, "snr_sq/F" );
  cbc_trigger_tree_out_ptr->Branch( "mass1", &mass1, "mass1/F" );
  cbc_trigger_tree_out_ptr->Branch( "mass2", &mass2, "mass2/F" );
  cbc_trigger_tree_out_ptr->Branch( "mtotal", &mtotal, "mtotal/F" );
  cbc_trigger_tree_out_ptr->Branch( "mchirp", &mchirp, "mchirp/F" );
  cbc_trigger_tree_out_ptr->Branch( "eta", &eta, "eta/F" );
  cbc_trigger_tree_out_ptr->Branch( "ttotal", &ttotal, "ttotal/F" );

  Int_t o;

  for (Int_t c=0; c<num_cbc_triggers; ++c) {
    cbc_trigger_tree_in_ptr->GetEntry(c);

    for(o=0; o<=num_omicron_triggers; ++o) {
      if ( o < num_omicron_triggers ) {
        omicron_trigger_tree_veto_ptr->GetEntry(o);
        if( Ctend > start_time && Ctstart < end_time) {
          if (verbose) std::cout << std::setprecision(15) << "CBC [" << start_time << "," << end_time << ") overlaps veto [" << Ctstart << "," << Ctend << "). Vetoing" << std::endl;
          break;
        }
      }
    }

    if ( o == num_omicron_triggers ) cbc_trigger_tree_out_ptr->Fill();
  }

  if (verbose) {
    std::cout << "On input " << cbc_trigger_tree_in_ptr->GetName() << " had " << cbc_trigger_tree_in_ptr->GetEntries() << " triggers" << std::endl;
    std::cout << "Used " << omicron_trigger_tree_veto_ptr->GetName() << " veto triggers from " << omicron_trigger_tree_veto_ptr->GetEntries() << " triggers" << std::endl;
    std::cout << "On output " << cbc_trigger_tree_out_ptr->GetName() << " had " << cbc_trigger_tree_out_ptr->GetEntries() << " triggers" << std::endl;
  }

  return cbc_trigger_tree_out_ptr;
}


TTree* eveto::remove_omicron_triggers(
    TTree* omicron_trigger_tree_in_ptr,
    TTree* omicron_trigger_tree_veto_ptr,
//    TTree* cbc_segs_tree_ptr,
//    TTree* omicron_segs_tree_ptr,
    bool verbose )
    {

  // Set up the variables to access the input omicron triggers
  double Itime, Itstart, Itend, Ifreq, Ifstart, Ifend, Isnr, Iamp, Iq;
  Long64_t Ifirstentry, Isize;

  omicron_trigger_tree_in_ptr->SetBranchAddress("time",       &Itime);
  omicron_trigger_tree_in_ptr->SetBranchAddress("tstart",     &Itstart);
  omicron_trigger_tree_in_ptr->SetBranchAddress("tend",       &Itend);
  omicron_trigger_tree_in_ptr->SetBranchAddress("frequency",  &Ifreq);
  omicron_trigger_tree_in_ptr->SetBranchAddress("fstart",     &Ifstart);
  omicron_trigger_tree_in_ptr->SetBranchAddress("fend",       &Ifend);
  omicron_trigger_tree_in_ptr->SetBranchAddress("snr",        &Isnr);
  omicron_trigger_tree_in_ptr->SetBranchAddress("amplitude",  &Iamp);
  omicron_trigger_tree_in_ptr->SetBranchAddress("q",          &Iq);
  omicron_trigger_tree_in_ptr->SetBranchAddress("firstentry", &Ifirstentry);
  omicron_trigger_tree_in_ptr->SetBranchAddress("size",       &Isize);

  int num_omicron_in_triggers = omicron_trigger_tree_in_ptr->GetEntries();

  // Set up the variables to access the input omicron triggers
  double Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
  Long64_t Cfirstentry, Csize;

  omicron_trigger_tree_veto_ptr->SetBranchAddress("time",       &Ctime);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("tstart",     &Ctstart);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("tend",       &Ctend);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("frequency",  &Cfreq);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("fstart",     &Cfstart);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("fend",       &Cfend);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("snr",        &Csnr);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("amplitude",  &Camp);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("q",          &Cq);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("firstentry", &Cfirstentry);
  omicron_trigger_tree_veto_ptr->SetBranchAddress("size",       &Csize);

  int num_omicron_triggers = omicron_trigger_tree_veto_ptr->GetEntries();

  // Create a new tree that will store the output data
  TTree* omicron_trigger_tree_out_ptr = new TTree(omicron_trigger_tree_in_ptr->GetName(),omicron_trigger_tree_in_ptr->GetName());

  // Create the branches for the tree
  omicron_trigger_tree_out_ptr->Branch("time",       &Itime,        "time/D");
  omicron_trigger_tree_out_ptr->Branch("tstart",     &Itstart,      "tstart/D");
  omicron_trigger_tree_out_ptr->Branch("tend",       &Itend,        "tend/D");
  omicron_trigger_tree_out_ptr->Branch("frequency",  &Ifreq,        "frequency/D");
  omicron_trigger_tree_out_ptr->Branch("fstart",     &Ifstart,      "fstart/D");
  omicron_trigger_tree_out_ptr->Branch("fend",       &Ifend,        "fend/D");
  omicron_trigger_tree_out_ptr->Branch("snr",        &Isnr,         "snr/D");
  omicron_trigger_tree_out_ptr->Branch("amplitude",  &Iamp,         "amplitude/D");
  omicron_trigger_tree_out_ptr->Branch("q",          &Iq,           "q/D");
  omicron_trigger_tree_out_ptr->Branch("firstentry", &Ifirstentry,  "firstentry/L");
  omicron_trigger_tree_out_ptr->Branch("size",       &Isize,        "size/L");

  Int_t o;

  for (Int_t c=0; c<num_omicron_in_triggers; ++c) {
    omicron_trigger_tree_in_ptr->GetEntry(c);

    for(o=0; o<=num_omicron_triggers; ++o) {
      if ( o < num_omicron_triggers ) {
        omicron_trigger_tree_veto_ptr->GetEntry(o);
        if( Ctend > Itstart && Ctstart < Itend ) {
          if (verbose) std::cout << std::setprecision(15) << "omicron [" << Itstart << "," << Itend << ") overlaps veto [" << Ctstart << "," << Ctend << "). Vetoing" << std::endl;
          break;
          }
      }
    }

    if ( o == num_omicron_triggers ) omicron_trigger_tree_out_ptr->Fill();
  }

  if (verbose) {
    std::cout << "On input " << omicron_trigger_tree_in_ptr->GetName() << " had " << omicron_trigger_tree_in_ptr->GetEntries() << " triggers" << std::endl;
    std::cout << "Used " << omicron_trigger_tree_veto_ptr->GetName() << " veto triggers from " << omicron_trigger_tree_veto_ptr->GetEntries() << " triggers" << std::endl;
    std::cout << "On output " << omicron_trigger_tree_out_ptr->GetName() << " had " << omicron_trigger_tree_out_ptr->GetEntries() << " triggers" << std::endl;
  }

  return omicron_trigger_tree_out_ptr;
}

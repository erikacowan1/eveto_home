#include "cbc_eveto.h"

//function that does a simple significance calculation
int eveto::calc_dumb_sig(
    TTree* cbc_trigger_tree_ptr,
    TTree* cwb_trigger_tree_ptr,
    TTree* omicron_trigger_tree_ptr,
    TString* detector, 
    float dumb_time_seg,
    bool verbose )
{
  if ( verbose ) {
    std::cerr << "calc_dumb_sig() got a cbc tree at " << cbc_trigger_tree_ptr << std::endl;
    // cbc_trigger_tree_ptr->Print();

    std::cerr << "calc_dumb_sig() got a cwb tree at " << cwb_trigger_tree_ptr << std::endl;
    cwb_trigger_tree_ptr->Print();

    std::cerr << "calc_dumb_sig() got a veto tree at " << omicron_trigger_tree_ptr << std::endl;
    omicron_trigger_tree_ptr->Print();
  }

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


  int num_coinc_triggers = 0;

  TString cbc = "cbc";
  if (detector == &cbc)
  {
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

  	if ( num_omicron_triggers == 0 ){
    		if (verbose) std::cout << "no omicron triggers, returning zero significance" << std::endl;
    		return 0;
  	}

  	for (Int_t c=0; c<num_cbc_triggers; ++c) {
    		cbc_trigger_tree_ptr->GetEntry(c);

      		for(Int_t o=0; o<num_omicron_triggers; ++o) {
       			omicron_trigger_tree_ptr->GetEntry(o);
   
        		if(Ctend > start_time && Ctstart < end_time) {
          			num_coinc_triggers++;
        		}
     		}

  	}
  }

  TString cwb = "cwb";
  if (detector == &cwb);
  {
		//double time, frequency, duration, start, stop, snr, rho, netcc, neted, likelihood, ecor, ECOR, ifo, rate, phi, theta, psi,null, hrss, noise, low, high, bandwidth, strain, run;
		
		Long64_t Csize;


	    	Double_t Ctime, Cfrequency, Cduration, Cstart, Cstop, Csnr, Crho, Cnetcc, Cneted, Clikelihood, Cecor, CECOR, Cifo, Crate, Cphi, Ctheta, Cpsi, Cnull, Chrss, Cnoise, Clow, Chigh, Cbandwidth, Cstrain, Crun;

		cwb_trigger_tree_ptr->SetBranchAddress("time",&Ctime);
		cwb_trigger_tree_ptr->SetBranchAddress("frequency",&Cfrequency);
  		cwb_trigger_tree_ptr->SetBranchAddress("duration",&Cduration);
 		cwb_trigger_tree_ptr->SetBranchAddress("start",&Cstart);
 		cwb_trigger_tree_ptr->SetBranchAddress("stop",&Cstop);
		cwb_trigger_tree_ptr->SetBranchAddress("snr",&Csnr);
		cwb_trigger_tree_ptr->SetBranchAddress("rho",&Crho);
  		cwb_trigger_tree_ptr->SetBranchAddress("netcc",&Cnetcc);
  		cwb_trigger_tree_ptr->SetBranchAddress("neted", &Cneted);
  		cwb_trigger_tree_ptr->SetBranchAddress("likelihood", &Clikelihood);
  		cwb_trigger_tree_ptr->SetBranchAddress("ecor", &Cecor);
  		cwb_trigger_tree_ptr->SetBranchAddress("ECOR", &CECOR);
  		cwb_trigger_tree_ptr->SetBranchAddress("ifo", &Cifo);
  		cwb_trigger_tree_ptr->SetBranchAddress("rate", &Crate);
  		cwb_trigger_tree_ptr->SetBranchAddress("phi", &Cphi);
  		cwb_trigger_tree_ptr->SetBranchAddress("theta", &Ctheta);
  		cwb_trigger_tree_ptr->SetBranchAddress("psi", &Cpsi);
  		cwb_trigger_tree_ptr->SetBranchAddress("null", &Cnull);
  		cwb_trigger_tree_ptr->SetBranchAddress("hrss", &Chrss);
  		cwb_trigger_tree_ptr->SetBranchAddress("noise", &Cnoise);
  		cwb_trigger_tree_ptr->SetBranchAddress("low", &Clow);
  		cwb_trigger_tree_ptr->SetBranchAddress("high", &Chigh);
  		cwb_trigger_tree_ptr->SetBranchAddress("bandwidth", &Cbandwidth);
  		cwb_trigger_tree_ptr->SetBranchAddress("strain", &Cstrain);
  		cwb_trigger_tree_ptr->SetBranchAddress("run", &Crun);
  		cwb_trigger_tree_ptr->SetBranchAddress("size", &Csize);

		int num_cwb_triggers = cwb_trigger_tree_ptr->GetEntries();
 
  		if ( num_omicron_triggers == 0 ){
    		if (verbose) std::cout << "no omicron triggers, returning zero significance" << std::endl;
    			return 0;
 		}

  		for (Int_t c=0; c<num_cwb_triggers; ++c) {
    			cwb_trigger_tree_ptr->GetEntry(c);

      			for(Int_t o=0; o<num_omicron_triggers; ++o) {
       			omicron_trigger_tree_ptr->GetEntry(o);
   
        			if(Ctend > Cstart && Ctstart < Cstop) {
          				num_coinc_triggers++;
        			}
     			}

  		}
  	}
  	float dumb_significance_value = (float)num_coinc_triggers/(float)num_omicron_triggers;

  	if (verbose) std::cout << "dumb significance = " << num_coinc_triggers << "/" << num_omicron_triggers << " = " << dumb_significance_value << std::endl;

  	return dumb_significance_value;
}

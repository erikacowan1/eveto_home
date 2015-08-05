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

    std::cerr << "calc_dumb_sig() got a veto tree at " << omicron_trigger_tree_ptr << std::endl;
    // omicron_trigger_tree_ptr->Print();
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
		double time, frequency, duration, start, stop, snr, rho, netcc, neted, likelihood, ecor, ECOR, ifo, rate, phi, theta, psi,null, hrss, noise, low, high, bandwidth, strain, run, size;

		cwb_trigger_tree_ptr->SetBranchAddress("time",&time);
		cwb_trigger_tree_ptr->SetBranchAddress("frequency",&frequency);
  		cwb_trigger_tree_ptr->SetBranchAddress("duration",&duration);
 		cwb_trigger_tree_ptr->SetBranchAddress("start",&start);
 		cwb_trigger_tree_ptr->SetBranchAddress("stop",&stop);
		cwb_trigger_tree_ptr->SetBranchAddress("snr",&snr);
		cwb_trigger_tree_ptr->SetBranchAddress("rho",&rho);
  		cwb_trigger_tree_ptr->SetBranchAddress("netcc",&netcc);
  		cwb_trigger_tree_ptr->SetBranchAddress("neted", &neted);
  		cwb_trigger_tree_ptr->SetBranchAddress("likelihood", &likelihood);
  		cwb_trigger_tree_ptr->SetBranchAddress("ecor", &ecor);
  		cwb_trigger_tree_ptr->SetBranchAddress("ECOR", &ECOR);
  		cwb_trigger_tree_ptr->SetBranchAddress("ifo", &ifo);
  		cwb_trigger_tree_ptr->SetBranchAddress("rate", &rate);
  		cwb_trigger_tree_ptr->SetBranchAddress("phi", &phi);
  		cwb_trigger_tree_ptr->SetBranchAddress("theta", &theta);
  		cwb_trigger_tree_ptr->SetBranchAddress("psi", &psi);
  		cwb_trigger_tree_ptr->SetBranchAddress("null", &null);
  		cwb_trigger_tree_ptr->SetBranchAddress("hrss", &hrss);
  		cwb_trigger_tree_ptr->SetBranchAddress("noise", &noise);
  		cwb_trigger_tree_ptr->SetBranchAddress("low", &low);
  		cwb_trigger_tree_ptr->SetBranchAddress("high", &high);
  		cwb_trigger_tree_ptr->SetBranchAddress("bandwidth", &bandwidth);
  		cwb_trigger_tree_ptr->SetBranchAddress("strain", &strain);
  		cwb_trigger_tree_ptr->SetBranchAddress("run", &run);
  		cwb_trigger_tree_ptr->SetBranchAddress("size", &size);

		int num_cwb_triggers = cwb_trigger_tree_ptr->GetEntries();
 
  		if ( num_omicron_triggers == 0 ){
    		if (verbose) std::cout << "no omicron triggers, returning zero significance" << std::endl;
    			return 0;
 		}

  		for (Int_t c=0; c<num_cwb_triggers; ++c) {
    			cwb_trigger_tree_ptr->GetEntry(c);

      			for(Int_t o=0; o<num_omicron_triggers; ++o) {
       			omicron_trigger_tree_ptr->GetEntry(o);
   
        			if(Ctend > start && Ctstart < stop) {
          				num_coinc_triggers++;
        			}
     			}

  		}
  	}
  	float dumb_significance_value = (float)num_coinc_triggers/(float)num_omicron_triggers;

  	if (verbose) std::cout << "dumb significance = " << num_coinc_triggers << "/" << num_omicron_triggers << " = " << dumb_significance_value << std::endl;

  	return dumb_significance_value;
}

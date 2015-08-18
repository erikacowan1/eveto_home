#include "cbc_eveto.h"

int eveto:: calc_dumb_sig(
	TTree* main_channel_trigger_tree_ptr,
	TTree* omicron_trigger_tree_ptr,
	TString* main_channel, 
	float dumb_time_seg,
	bool verbose )
{

	std::cerr << "main_channel" << main_channel->Data() << std::endl;
	if( verbose ) {
		std::cerr << "calc_dumb_sig() got main channel " << main_channel->Data() << "tree at " << main_channel_trigger_tree_ptr << std::endl;
		//main_channel_trigger_tree_ptr->Print();

		std::cerr << "calc_dumb_sig() got omicron tree at" << omicron_trigger_tree_ptr << std::endl;

		//omicron_trigger_tree_ptr->Print();
	}

	
	double Ltime, Ltstart, Ltend, Lfreq, Lfstart, Lfend, Lsnr, Lamp, Lq;
	Long64_t Lfirstentry, Lsize;

	omicron_trigger_tree_ptr->SetBranchAddress("time",       &Ltime);
	omicron_trigger_tree_ptr->SetBranchAddress("tstart",     &Ltstart);
	omicron_trigger_tree_ptr->SetBranchAddress("tend",       &Ltend);
	omicron_trigger_tree_ptr->SetBranchAddress("frequency",  &Lfreq);
	omicron_trigger_tree_ptr->SetBranchAddress("fstart",     &Lfstart);
	omicron_trigger_tree_ptr->SetBranchAddress("fend",       &Lfend);
	omicron_trigger_tree_ptr->SetBranchAddress("snr",        &Lsnr);
	omicron_trigger_tree_ptr->SetBranchAddress("amplitude",  &Lamp);
	omicron_trigger_tree_ptr->SetBranchAddress("q",          &Lq);
	omicron_trigger_tree_ptr->SetBranchAddress("firstentry", &Lfirstentry);
	omicron_trigger_tree_ptr->SetBranchAddress("size",       &Lsize);

	int num_omicron_triggers = omicron_trigger_tree_ptr->GetEntries();

	int num_coinc_triggers = 0;

//	TString cbc = "cbc";
//	if( main_channel == &cbc )
       const char *main_channel_char = (const char*)main_channel;
        if( strcmp(main_channel_char, "cbc") == 0)
	{
		Double_t start_time, end_time;
		Float_t snr, chisq, chisqdof, newsnr, snr_sq, mass1, mass2, mtotal, mchirp, eta, ttotal;
		
		
		main_channel_trigger_tree_ptr->SetBranchAddress("start_time",&start_time);
		main_channel_trigger_tree_ptr->SetBranchAddress("end_time",&end_time);
	  	main_channel_trigger_tree_ptr->SetBranchAddress("snr",&snr);
 	 	main_channel_trigger_tree_ptr->SetBranchAddress("chisq",&chisq);
 	 	main_channel_trigger_tree_ptr->SetBranchAddress("chisqdof",&chisqdof);
  		main_channel_trigger_tree_ptr->SetBranchAddress("newsnr",&newsnr);
  		main_channel_trigger_tree_ptr->SetBranchAddress("snr_sq",&snr_sq);
  		main_channel_trigger_tree_ptr->SetBranchAddress("mass1",&mass1);
  		main_channel_trigger_tree_ptr->SetBranchAddress("mass2",&mass2);
  		main_channel_trigger_tree_ptr->SetBranchAddress("mtotal",&mtotal);
  		main_channel_trigger_tree_ptr->SetBranchAddress("mchirp",&mchirp);
  		main_channel_trigger_tree_ptr->SetBranchAddress("eta",&eta);
  		main_channel_trigger_tree_ptr->SetBranchAddress("ttotal",&ttotal);
		
		int num_main_channel_triggers = main_channel_trigger_tree_ptr->GetEntries();

		if( num_omicron_triggers == 0 ) {
			if( verbose ) std::cout << "no omicron triggers, returning zero significance" << std::endl;
		       return 0;
		}

		for( Int_t c = 0; c<num_main_channel_triggers; ++c ) {
			main_channel_trigger_tree_ptr->GetEntry(c);

			for(Int_t o=0; o<num_omicron_triggers; ++o) {
				omicron_trigger_tree_ptr->GetEntry(o);

				if( Ltend > start_time && Ltstart < end_time ) {
					num_coinc_triggers++;
				}
			}
		}
	}

//	TString cwb = "cwb";
//	if( main_channel == &cwb );
        if( strcmp(main_channel_char, "cwb") == 0)
	{
		Double_t Ctime, Cfrequency, Cduration, Cstart, Cstop, Csnr, Crho, Cnetcc, Cneted, Clikelihood, Cecor, CECOR, Cifo, Crate, Cphi, Ctheta, Cpsi, Cnull, Chrss, Cnoise, Clow, Chigh, Cbandwidth, Cstrain, Crun;
		Long64_t Csize;

		main_channel_trigger_tree_ptr->SetBranchAddress("time",&Ctime);
		main_channel_trigger_tree_ptr->SetBranchAddress("frequency",&Cfrequency);
  		main_channel_trigger_tree_ptr->SetBranchAddress("duration",&Cduration);
 		main_channel_trigger_tree_ptr->SetBranchAddress("start",&Cstart);
 		main_channel_trigger_tree_ptr->SetBranchAddress("stop",&Cstop);
		main_channel_trigger_tree_ptr->SetBranchAddress("snr",&Csnr);
		main_channel_trigger_tree_ptr->SetBranchAddress("rho",&Crho);
  		main_channel_trigger_tree_ptr->SetBranchAddress("netcc",&Cnetcc);
  		main_channel_trigger_tree_ptr->SetBranchAddress("neted", &Cneted);
  		main_channel_trigger_tree_ptr->SetBranchAddress("likelihood", &Clikelihood);
  		main_channel_trigger_tree_ptr->SetBranchAddress("ecor", &Cecor);
  		main_channel_trigger_tree_ptr->SetBranchAddress("ECOR", &CECOR);
  		main_channel_trigger_tree_ptr->SetBranchAddress("ifo", &Cifo);
  		main_channel_trigger_tree_ptr->SetBranchAddress("rate", &Crate);
  		main_channel_trigger_tree_ptr->SetBranchAddress("phi", &Cphi);
  		main_channel_trigger_tree_ptr->SetBranchAddress("theta", &Ctheta);
  		main_channel_trigger_tree_ptr->SetBranchAddress("psi", &Cpsi);
  		main_channel_trigger_tree_ptr->SetBranchAddress("null", &Cnull);
  		main_channel_trigger_tree_ptr->SetBranchAddress("hrss", &Chrss);
  		main_channel_trigger_tree_ptr->SetBranchAddress("noise", &Cnoise);
  		main_channel_trigger_tree_ptr->SetBranchAddress("low", &Clow);
  		main_channel_trigger_tree_ptr->SetBranchAddress("high", &Chigh);
  		main_channel_trigger_tree_ptr->SetBranchAddress("bandwidth", &Cbandwidth);
  		main_channel_trigger_tree_ptr->SetBranchAddress("strain", &Cstrain);
  		main_channel_trigger_tree_ptr->SetBranchAddress("run", &Crun);
  		main_channel_trigger_tree_ptr->SetBranchAddress("size", &Csize);

		int num_main_channel_triggers = main_channel_trigger_tree_ptr->GetEntries();

		if( num_omicron_triggers == 0 ) {
			if( verbose ) std::cout << "no omicron triggers, returning zero significance" << std::endl;
			return 0;
		}

		for( Int_t c=0; c<num_main_channel_triggers; ++c) {
			main_channel_trigger_tree_ptr->GetEntry(c);

			for(Int_t o=0; o<num_omicron_triggers; ++o) {
				omicron_trigger_tree_ptr->GetEntry(o);

				if( Ltend > Cstart && Ltstart < Cstop ) {
					num_coinc_triggers++;
				}
			}
		}
	}

	float dumb_significance_value = (float)num_coinc_triggers/(float)num_omicron_triggers;

	if( verbose ) std::cout << "dumb significance =" << num_coinc_triggers << "/" << num_omicron_triggers << "=" << dumb_significance_value << std::endl;

	return dumb_significance_value;
}



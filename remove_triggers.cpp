#include "cbc_eveto.h"


//
//
//Algorithm to remove triggers from main channels(CBC/CWB)
//
//
TTree* eveto::remove_main_channel_triggers(
		TTree* main_channel_trigger_tree_in_ptr, //input
		//TTree* omicron_trigger_tree_veto_ptr, //output
		TTree* main_channel_trigger_tree_out_ptr, //output
		TString* main_channel,
		//TTree* cbc_segs_tree_ptr, 
		//TTree* omicron_segs_tree_ptr,
		bool verbose)
{

	if( verbose ) {
		std::cerr << "calc_dumb_sig() got main channel" << main_channel << "tree at" << main_channel_trigger_tree_in_ptr << std::endl;
		//main_channel_trigger_tree_in_ptr->Print();

		std::cerr << "calc_dumb_sig() got omicron tree at" << omicron_trigger_tree_veto_ptr << std::endl;
		//omicron_trigger_tree_ptr->Print();
	}
/*
	Double_t Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
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
	TTree* main_channel_trigger_tree_out_ptr = new TTree("main-channel-triggers","main-channel-triggers");
*/

	//
	//
	//For the CBC option
	//
	//
	TString cbc = "cbc";
	if( main_channel == &cbc )
	{
		Double_t start_time, end_time;
		Float_t snr, chisq, chisqdof, newsnr, snr_sq, mass1, mass2, mtotal, mchirp, eta, ttotal;


		main_channel_trigger_tree_in_ptr->SetBranchAddress("start_time",&start_time);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("end_time",&end_time);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("snr",&snr);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("chisq",&chisq);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("chisqdof",&chisqdof);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("newsnr",&newsnr);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("snr_sq",&snr_sq);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("mass1",&mass1);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("mass2",&mass2);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("mtotal",&mtotal);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("mchirp",&mchirp);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("eta",&eta);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("ttotal",&ttotal);

		int num_main_channel_triggers = main_channel_trigger_tree_in_ptr->GetEntries();

		//Create a new tree that will store the output data
		//TTree* main_channel_trigger_tree_out_ptr = new TTree("cbc-triggers","cbc-triggers");

		// Create the branches for the tree
		main_channel_trigger_tree_out_ptr->Branch( "start_time" , &start_time, "start_time/D" );
		main_channel_trigger_tree_out_ptr->Branch( "end_time" , &end_time, "end_time/D" );
		main_channel_trigger_tree_out_ptr->Branch( "snr" , &snr, "snr/F" );
		main_channel_trigger_tree_out_ptr->Branch( "chisq" , &chisq, "chisq/F" );
		main_channel_trigger_tree_out_ptr->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
		main_channel_trigger_tree_out_ptr->Branch( "newsnr" , &newsnr, "newsnr/F" );
		main_channel_trigger_tree_out_ptr->Branch( "snr_sq" , &snr_sq, "snr_sq/F" );
		main_channel_trigger_tree_out_ptr->Branch( "mass1", &mass1, "mass1/F" );
		main_channel_trigger_tree_out_ptr->Branch( "mass2", &mass2, "mass2/F" );
		main_channel_trigger_tree_out_ptr->Branch( "mtotal", &mtotal, "mtotal/F" );
		main_channel_trigger_tree_out_ptr->Branch( "mchirp", &mchirp, "mchirp/F" );
		main_channel_trigger_tree_out_ptr->Branch( "eta", &eta, "eta/F" );
		main_channel_trigger_tree_out_ptr->Branch( "ttotal", &ttotal, "ttotal/F" );

			Double_t Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
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
	TTree* main_channel_trigger_tree_out_ptr = new TTree("main-channel-triggers","main-channel-triggers");



		Int_t o;
		for (Int_t c=0; c<num_main_channel_triggers; ++c) {
			main_channel_trigger_tree_in_ptr->GetEntry(c);

			for(o=0; o<=num_omicron_triggers; ++o) {
				if ( o < num_omicron_triggers ) {
					omicron_trigger_tree_veto_ptr->GetEntry(o);

					if( Ctend > start_time && Ctstart < end_time) {

						if (verbose) std::cout << "CBC [" << std::setprecision(15) << start_time << "," << end_time << ") overlaps veto [" << Ctstart << "," << Ctend << "). Vetoing" << std::endl;
						break;
					}
				}
			}

			if ( o == num_omicron_triggers ) main_channel_trigger_tree_out_ptr->Fill();
		}
	}

	//
	//
	//For the CWB option
	//
	//

	TString cwb = "cwb";
	if (main_channel == &cwb)
	{
		Double_t Ctime, Cfrequency, Cduration, Cstart, Cstop, Csnr, Crho, Cnetcc, Cneted, Clikelihood, Cecor, CECOR, Cifo, Crate, Cphi, Ctheta, Cpsi, Cnull, Chrss, Cnoise, Clow, Chigh, Cbandwidth, Cstrain, Crun, Csize;

		main_channel_trigger_tree_in_ptr->SetBranchAddress("time",&Ctime);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("frequency",&Cfrequency);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("duration",&Cduration);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("start",&Cstart);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("stop",&Cstop);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("snr",&Csnr);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("rho",&Crho);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("netcc",&Cnetcc);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("neted", &Cneted);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("likelihood", &Clikelihood);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("ecor", &Cecor);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("ECOR", &CECOR);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("ifo", &Cifo);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("rate", &Crate);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("phi", &Cphi);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("theta", &Ctheta);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("psi", &Cpsi);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("null", &Cnull);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("hrss", &Chrss);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("noise", &Cnoise);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("low", &Clow);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("high", &Chigh);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("bandwidth", &Cbandwidth);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("strain", &Cstrain);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("run", &Crun);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("size", &Csize);

		int num_cwb_triggers = main_channel_trigger_tree_in_ptr->GetEntries();

		// Create a new tree that will store the output data
		//TTree* main_channel_trigger_tree_out_ptr = new TTree("cwb-triggers","cwb-triggers");

		main_channel_trigger_tree_out_ptr->Branch("time",       &time,      "time/D");
		main_channel_trigger_tree_out_ptr->Branch("frequency",  &Cfrequency, "frequency/D");
		main_channel_trigger_tree_out_ptr->Branch("duration",   &Cduration,  "duration/D");
		main_channel_trigger_tree_out_ptr->Branch("start",      &Cstart,     "start/D");
		main_channel_trigger_tree_out_ptr->Branch("stop",       &Cstop,      "stop/D");
		main_channel_trigger_tree_out_ptr->Branch("snr",        &Csnr,       "snr/D");
		main_channel_trigger_tree_out_ptr->Branch("rho",        &Crho,       "rho/D");
		main_channel_trigger_tree_out_ptr->Branch("netcc",      &Cnetcc,     "netcc/D");
		main_channel_trigger_tree_out_ptr->Branch("neted",      &Cneted,     "neted/D");
		main_channel_trigger_tree_out_ptr->Branch("likelihood", &Clikelihood,"likelihood/D");
		main_channel_trigger_tree_out_ptr->Branch("ecor",       &Cecor,      "ecor/D");
		main_channel_trigger_tree_out_ptr->Branch("ECOR",       &CECOR,      "ECOR/D");
		main_channel_trigger_tree_out_ptr->Branch("ifo",        &Cifo,       "ifo/D");
		main_channel_trigger_tree_out_ptr->Branch("rate",       &Crate,      "rate/D");
		main_channel_trigger_tree_out_ptr->Branch("phi",        &Cphi,       "phi/D");
		main_channel_trigger_tree_out_ptr->Branch("theta",      &Ctheta,     "theta/D");
		main_channel_trigger_tree_out_ptr->Branch("psi",        &Cpsi,       "psi/D");
		main_channel_trigger_tree_out_ptr->Branch("null",       &Cnull,      "null/D");
		main_channel_trigger_tree_out_ptr->Branch("hrss",       &Chrss,      "hrss/D");
		main_channel_trigger_tree_out_ptr->Branch("noise",      &Cnoise,     "noise/D");
		main_channel_trigger_tree_out_ptr->Branch("low",        &Clow,       "low/D");
		main_channel_trigger_tree_out_ptr->Branch("high",       &Chigh,      "high/D");
		main_channel_trigger_tree_out_ptr->Branch("bandwidth",  &Cbandwidth, "bandwidth/D");
		main_channel_trigger_tree_out_ptr->Branch("strain",     &Cstrain,    "strain/D");
		main_channel_trigger_tree_out_ptr->Branch("run",        &Crun,       "run/D");
		main_channel_trigger_tree_out_ptr->Branch("size",       &Csize,      "size/D");

			Double_t Ctime, Ctstart, Ctend, Cfreq, Cfstart, Cfend, Csnr, Camp, Cq;
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
	TTree* main_channel_trigger_tree_out_ptr = new TTree("main-channel-triggers","main-channel-triggers");



		Cstop=0.0;
		Int_t o; 

  		for (Int_t c=0; c<num_cwb_triggers; ++c) {
    			main_channel_trigger_tree_in_ptr->GetEntry(c);

    			for(o=0; o<=num_omicron_triggers; ++o) {
      				if ( o < num_omicron_triggers ) {
        				omicron_trigger_tree_veto_ptr->GetEntry(o);
        				
					if( Ctend > Cstart && Ctstart < Cstop) {
          				
						if (verbose) std::cout << std::setprecision(15) << "CWB [" << Cstart << "," << stop << ") overlaps veto [" << Ctstart << "," << Ctend << "). Vetoing" << std::endl;
          break;
        				}
      				}
    			}

    			if ( o == num_omicron_triggers ) main_channel_trigger_tree_out_ptr->Fill();
  		}

  	}


  	if (verbose) {
    			std::cout << "On input " << main_channel_trigger_tree_in_ptr->GetName() << " had " << main_channel_trigger_tree_in_ptr->GetEntries() << " triggers" << std::endl;
    			
			std::cout << "Used " << omicron_trigger_tree_veto_ptr->GetName() << " veto triggers from " << omicron_trigger_tree_veto_ptr->GetEntries() << " triggers" << std::endl;
    
			std::cout << "On output " << main_channel_trigger_tree_out_ptr->GetName() << " had " << main_channel_trigger_tree_out_ptr->GetEntries() << " triggers" << std::endl;
  
		}
		return 0;
		//return main_channel_trigger_tree_out_ptr;
	
}

TTree* eveto::remove_omicron_triggers(
	TTree* omicron_trigger_tree_in_ptr, //input
	TTree* omicron_trigger_tree_veto_ptr, //input
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
	//return 0;
  	return omicron_trigger_tree_out_ptr;
}


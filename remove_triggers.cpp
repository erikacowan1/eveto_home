#include cbc_eveto.h


//
//
//Algorithm to remove triggers from main channels(CBC/CWB)
//
//
TTree* eveto::remove_main_channel_triggers(
	TTree* main_channel_trigger_tree_in_ptr,
	TTree* omicron_trigger_tree_ptr,
	TString* main_channel,
	//TTree* cbc_segs_tree_ptr, 
	//TTree* omicron_segs_tree_ptr,
	bool verbose)
{

	if( verbose ) {
		std::cerr << "calc_dumb_sig() got main channel" << main_channel << "tree at" << main_channel_trigger_tree_in_ptr << std::endl;
		//main_channel_trigger_tree_in_ptr->Print();

		std::cerr << "calc_dumb_sig() got omicron tree at" << omicron_trigger_tree_ptr << std::endl;
		//omicron_trigger_tree_ptr->Print();
	}

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
		TTree* main_channel_trigger_tree_out_ptr = new TTree("cbc-triggers","cbc-triggers");

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

		
  		Int_t o;

  		for (Int_t c=0; c<num_main_channel_triggers; ++c) {
    			main_channel_trigger_tree_in_ptr->GetEntry(c);

    			for(o=0; o<=num_omicron_triggers; ++o) {
      				if ( o < num_omicron_triggers ) {
        				omicron_trigger_tree_veto_ptr->GetEntry(o);
        				
					if( Ctend > start_time && Ctstart < end_time) {
          				
						if (verbose) std::cout << std::setprecision(15) << "CBC [" << start_time << "," << end_time << ") overlaps veto [" << Ctstart << "," << Ctend << "). Vetoing" << std::endl;
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
	 	Double_t time, frequency, duration, start, stop, snr, rho, netcc, neted, likelihood, ecor, ECOR, ifo, rate, phi, theta, psi,null, hrss, noise, low, high, bandwidth, strain, run, size;

		main_channel_trigger_tree_in_ptr->SetBranchAddress("time",&time);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("frequency",&frequency);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("duration",&duration);
 		main_channel_trigger_tree_in_ptr->SetBranchAddress("start",&start);
 		main_channel_trigger_tree_in_ptr->SetBranchAddress("stop",&stop);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("snr",&snr);
		main_channel_trigger_tree_in_ptr->SetBranchAddress("rho",&rho);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("netcc",&netcc);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("neted", &neted);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("likelihood", &likelihood);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("ecor", &ecor);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("ECOR", &ECOR);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("ifo", &ifo);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("rate", &rate);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("phi", &phi);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("theta", &theta);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("psi", &psi);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("null", &null);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("hrss", &hrss);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("noise", &noise);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("low", &low);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("high", &high);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("bandwidth", &bandwidth);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("strain", &strain);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("run", &run);
  		main_channel_trigger_tree_in_ptr->SetBranchAddress("size", &size);

		int num_cwb_triggers = main_channel_trigger_tree_in_ptr->GetEntries();

		// Create a new tree that will store the output data
  		TTree* main_channel_trigger_tree_out_ptr = new TTree("cwb-triggers","cwb-triggers");

  		main_channel_trigger_tree_out_ptr->Branch("time",       &time,      "time/D");
  		main_channel_trigger_tree_out_ptr->Branch("frequency",  &frequency, "frequency/D");
  		main_channel_trigger_tree_out_ptr->Branch("duration",   &duration,  "duration/D");
  		main_channel_trigger_tree_out_ptr->Branch("start",      &start,     "start/D");
  		main_channel_trigger_tree_out_ptr->Branch("stop",       &stop,      "stop/D");
  		main_channel_trigger_tree_out_ptr->Branch("snr",        &snr,       "snr/D");
 		main_channel_trigger_tree_out_ptr->Branch("rho",        &rho,       "rho/D");
  		main_channel_trigger_tree_out_ptr->Branch("netcc",      &netcc,     "netcc/D");
  		main_channel_trigger_tree_out_ptr->Branch("neted",      &neted,     "neted/D");
  		main_channel_trigger_tree_out_ptr->Branch("likelihood", &likelihood,"likelihood/D");
  		main_channel_trigger_tree_out_ptr->Branch("ecor",       &ecor,      "ecor/D");
  		main_channel_trigger_tree_out_ptr->Branch("ECOR",       &ECOR,      "ECOR/D");
  		main_channel_trigger_tree_out_ptr->Branch("ifo",        &ifo,       "ifo/D");
  		main_channel_trigger_tree_out_ptr->Branch("rate",       &rate,      "rate/D");
  		main_channel_trigger_tree_out_ptr->Branch("phi",        &phi,       "phi/D");
  		main_channel_trigger_tree_out_ptr->Branch("theta",      &theta,     "theta/D");
  		main_channel_trigger_tree_out_ptr->Branch("psi",        &psi,       "psi/D");
  		main_channel_trigger_tree_out_ptr->Branch("null",       &null,      "null/D");
  		main_channel_trigger_tree_out_ptr->Branch("hrss",       &hrss,      "hrss/D");
  		main_channel_trigger_tree_out_ptr->Branch("noise",      &noise,     "noise/D");
  		main_channel_trigger_tree_out_ptr->Branch("low",        &low,       "low/D");
  		main_channel_trigger_tree_out_ptr->Branch("high",       &high,      "high/D");
  		main_channel_trigger_tree_out_ptr->Branch("bandwidth",  &bandwidth, "bandwidth/D");
  		main_channel_trigger_tree_out_ptr->Branch("strain",     &strain,    "strain/D");
  		main_channel_trigger_tree_out_ptr->Branch("run",        &run,       "run/D");
  		main_channel_trigger_tree_out_ptr->Branch("size",       &size,      "size/D");

  		stop=0.0;
		Int_t o; 

  		for (Int_t c=0; c<num_cwb_triggers; ++c) {
    			main_channel_trigger_tree_in_ptr->GetEntry(c);

    			for(o=0; o<=num_omicron_triggers; ++o) {
      				if ( o < num_omicron_triggers ) {
        				omicron_trigger_tree_veto_ptr->GetEntry(o);
        				
					if( Ctend > start && Ctstart < stop) {
          				
						if (verbose) std::cout << std::setprecision(15) << "CWB [" << start << "," << stop << ") overlaps veto [" << Ctstart << "," << Ctend << "). Vetoing" << std::endl;
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
		return main_channel_trigger_tree_out_ptr;
	
}

TTree* eveto::remove_omicron_triggers(
	TTree* omicron_trigger_tree_in_ptr,
	TTree* omicron_trigger_tree_out_ptr,
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


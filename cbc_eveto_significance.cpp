//
//
//Code to calculate real significance
//cbc_eveto_significance.cpp
//
//


int cbc_eveto_significance(
	int sig_threshold,
	TTree* main_channel_trigger_tree_ptr,
	TTree* omicron_trigger_tree_ptr,
	TString* main_channel,
	float time_window, //declare, for now, in eveto_main.cpp
	bool verbose ) 
{

        if( verbose ) {
		std::cerr << "cbc_eveto_significance() got " << main_channel->Data() " tree at " << main_channel_trigger_tree_ptr << std::endl;
		std::cerr << "cbc_eveto_significance() got omicron tree at" << omicron_trigger_tree_ptr << std::endl;
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
	float time_window_2 = time_window/2;
        int num_coinc_triggers = 0;
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
		Long64_t aux_end_time, aux_start_time, main_channel_start_time, main_channel_end_time;
		if( num_omicron_triggers == 0 ) {
                        if( verbose ) std::cout << "no omicron triggers, returning zero significance" << std::endl;
                       return 0;
                }

                for( Int_t c = 0; c<num_main_channel_triggers; ++c ) {
                        main_channel_trigger_tree_ptr->GetEntry(c);

			if( c = 0 ) {
				main_channel_start_time = start_time;
				break;
			}
			
			if( c = num_main_channel_triggers ) {
				main_channel_end_time = start_time;
				break;
			}

                        for(Int_t o=0; o<num_omicron_triggers; ++o) {
                                omicron_trigger_tree_ptr->GetEntry(o);
				
				if( o = 0 ) {
					aux_start_time = Ltime;
					break;
				}

				if( o=num_main_channel_triggers) {
					aux_end_time=Ltime;
					break; 
				}

				if( start_time < (Ltime + time_window_2) && start_time > Ltime - time_window_2 ) {
					num_coinc_triggers++;
				}
			}
		}

		//calculate significance
		float mu;

		//calculating total time analyzed: to be changed later. 
        	Long65_t analyze_start, analyze_end, T_tot; //total time analyzed
		
		analyze_start = aux_start_time - time_window_2;
		analyze_end = aux_end_time + time_window_2;

		
			

#include "cbc_eveto.h"
#include "TLeaf.h"
//#include <boost/math/special_functions/gamma.hpp>

//
//
//Code to calculate real significance
//cbc_eveto_significance.cpp
//
//


int eveto:: cbc_eveto_significance(
	int sig_threshold,
	TTree* main_channel_trigger_tree_ptr,
	TTree* omicron_trigger_tree_ptr,
	TString* main_channel,
	float time_window, //declare, for now, in eveto_main.cpp
	bool verbose ) 
{

        if( verbose ) {
		std::cerr << "cbc_eveto_significance() got " << main_channel->Data() <<" tree at " << main_channel_trigger_tree_ptr << std::endl;
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
	//	Double_t aux_end_time, aux_start_time, main_channel_start_time, main_channel_end_time;
		if( num_omicron_triggers == 0 ) {
                        if( verbose ) std::cout << "no omicron triggers, returning zero significance" << std::endl;
                       return 0;
                }


                for( Int_t c = 0; c<num_main_channel_triggers; ++c ) {
                        main_channel_trigger_tree_ptr->GetEntry(c);
			/*TLeaf *start_time = main_channel_trigger_tree_ptr->GetLeaf("start_time");
			if( c == 0 ) {
				start_time->GetBranch()->GetEntry(c);
				main_channel_start_time = start_time->GetValue();
				break;
			}
			
			if( c == num_main_channel_triggers ) {
				start_time->GetBranch()->GetEntry(c);
				main_channel_end_time = start_time->GetValue();
				break;
			}
			*/
                        for(Int_t o=0; o<num_omicron_triggers; ++o) {
                                omicron_trigger_tree_ptr->GetEntry(o);
			/*	//TLeaf *Ltime = main_channel_trigger_tree_ptr->GetLeaf("Ltime");	
				if( o == 0 ) {
					TLeaf *Ltime = main_channel_trigger_tree_ptr->GetLeaf("Ltime");
					Ltime->GetBranch()->GetEntry(o);
					aux_start_time = Ltime->GetValue();
					break;
				}

				if( o == num_main_channel_triggers) {
					TLeaf *Ltime = main_channel_trigger_tree_ptr->GetLeaf("Ltime");
					Ltime->GetBranch()->GetEntry(o);
					aux_end_time=Ltime->GetValue();
					break; 
				}
			*/
				if( start_time < (Ltime + time_window_2) && start_time > Ltime - time_window_2 ) {
					num_coinc_triggers++;
				}
			}
		}
	////////////////////////////////////////////////
	///////////// CWB OPTION   /////////////////////
	////////////////////////////////////////////////
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



	
		//Long64_t aux_end_time, aux_start_time, main_channel_start_time, main_channel_end_time;
                if( num_omicron_triggers == 0 ) {
                        if( verbose ) std::cout << "no omicron triggers, returning zero significance" << std::endl;
                       return 0;
                }


                for( Int_t c = 0; c<num_main_channel_triggers; ++c ) {
                        main_channel_trigger_tree_ptr->GetEntry(c);
		/*	TLeaf *start_time = main_channel_trigger_tree_ptr->GetLeaf("start_time");
                        if( c == 0 ) {
				start_time->GetBranch()->GetEntry(c);
                                main_channel_start_time = start_time->GetValue();
                                break;
                        }

                        if( c == num_main_channel_triggers ) {
                                start_time->GetBranch()->GetEntry(c);
				main_channel_end_time = start_time->GetValue();
                                break;
                        }
*/
                        for(Int_t o=0; o<num_omicron_triggers; ++o) {
                                omicron_trigger_tree_ptr->GetEntry(o);
/*				TLeaf *Ltime = main_channel_trigger_tree_ptr->GetLeaf("Ltime");
                                if( o == 0 ) {
					Ltime->GetBranch()->GetEntry(c);
                                        aux_start_time = Ltime->GetValue();
                                        break;
                                }

                                if( o == num_main_channel_triggers) {
					Ltime->GetBranch()->GetEntry(c);
                                        aux_end_time=Ltime->GetValue();
                                        break;
                                }
*/
                                if( Cstart < (Ltime + time_window_2) && Cstart > Ltime - time_window_2 ) {
                                        num_coinc_triggers++;
                                }
                        }
                }
	}

		Float_t mu;
                Long64_t sig, sigold;

                //calculating total time analyzed: to be changed later. 
                //Long64_t analyze_start, analyze_end, T_tot; //total time analyzed
                //analyze_start = aux_start_time - time_window_2; //leftover code, not necessary
                //analyze_end = aux_end_time + time_window_2;

                Long64_t T_tot = num_omicron_triggers*time_window; //total time analyzed

                mu = (time_window*num_main_channel_triggers*num_omicron_triggers)/T_tot;

                if( num_coinc_triggers > 0 ) {
                      //  sig = gamma_p(mu, num_coinc_triggers);
                        sigold = -num_coinc_triggers*log10(mu) + mu*log10(exp(1)) + lgammal(num_coinc_triggers+1)/log(10);
                        //return sigold;
                        std::cerr << "sig = " << sig << ". If close enough to infinity, print out sigold instead, which yields " << sigold << ". \n" << std::endl;
                }

                else {
                        sig = 0;
                        std::cerr << "No coincident triggers, and therefore no significance. Something is wrong." << std::endl;
                }
		return sigold;
}

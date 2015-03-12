#include "cbc_eveto.h"

//function that does a simple significance calculation
int eveto::calc_dumb_sig(
		TTree* cbc_trigger_tree_ptr,
		TTree* omicron_trigger_tree_ptr,
		float dumb_time_seg,
		bool verbose )
{

	float num_omicron_triggers;
	float num_coinc_triggers = 0;
	float num_omicron_triggers = omicron_trigger_tree_ptr->GetEntries();
	end_time

	double Ctime, Omtime;
	if(cbc_trigger_tree_ptr->SetBranchAddress("time",&Ctime)<0){
		std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
		return false;
	}

 	cbc_trigger_tree_ptr->Branch("time",       &Ctime,      "time/D");
	
		if(omicron_trigger_tree_ptr->SetBranchAddress("time",&Ctime)<0){
		std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
		return false;
	}

 	 omicron_trigger_tree_ptr->Branch("time",       &Omtime,      "time/D");

	for (Long64_t c=0; c<cbc_trigger_tree_ptr->GetEntries(c); ++c) {
		cbc_trigger_tree_ptr->GetEntry(c);

		for (Long64_t d=0; d<omicron_trigger_tree_ptr->GetEntries(d); ++d) {
			omicron_trigger_tree_ptr->GetEntry(d);
			omicron_trigger_tree_ptr->Draw(">> SigList", "d>c-dumb_time_seg&&d<c+dumb_time_seg", "");
		}
	}
	TEventList *sl= (TEventList *)gROOT->FindObject("SigList");
	return sl->Print("all")
	

		//if (omicron_trigs_round > cbc_trigs_round[c] - dumb_time_seg && omicron_trigs_round < cbc_trigs_round[c] +  dumb_time_seg) {
			num_coinc_triggers += 1;
		}
	}
	//return  num_coinc_triggers/num_omicron_triggers;


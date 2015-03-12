#include "cbc_eveto.h"

//function that does a simple significance calculation
int eveto::calc_dumb_sig(
		TTree* cbc_trigger_tree_ptr,
		TTree* omicron_trigger_tree_ptr,
		float dumb_time_seg,
		bool verbose )
{

	int num_coinc_triggers = 0;
	int num_omicron_triggers = omicron_trigger_tree_ptr->GetEntries();

	double Ctime;

	if(cbc_trigger_tree_ptr->SetBranchAddress("time",&Ctime)<0){
		std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
		return false;
	}

	for (Long64_t c=0; c<cbc_trigger_tree_ptr->GetEntries(c); ++c) {
		cbc_trigger_tree_ptr->GetEntry(c);

		omicron_trigger_tree_ptr->Draw(">> CoincOmegaList", "end_time>Ctime-dumb_time_seg"&&"end_time<Ctime+dumb_time_seg", "");
	}
	TEventList *CoincOmegaList_ptr= (TEventList *)gROOT->FindObject("CoincOmegaList");

	num_coinc_triggers += CoincOmegaList_ptr->GetN();

	return (float)num_coinc_triggers/(float)num_omicron_triggers;
}


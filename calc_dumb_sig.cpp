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
	int num_cbc_triggers = cbc_trigger_tree_ptr->GetEntries();	

	double Ctime;

	if(cbc_trigger_tree_ptr->SetBranchAddress("time",&Ctime)<0){
		std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
		return false;
	}

	for (Int_t c=0; c<num_cbc_triggers; ++c) {
		cbc_trigger_tree_ptr->GetEntry(c);
		
		//Float_t	t1 = Ctime-dumb_time_seg;
		//Float_t t2 = Ctime+dumb_time_seg;
		TCut tc1 = "end_time>Ctime-dumb_time_seg ";
		TCut tc2 = "end_time<Ctime+dumb_time_seg ";

		omicron_trigger_tree_ptr->Draw(">> CoincOmegaList", tc1 && tc2, "");
	}
	TEventList *CoincOmegaList_ptr= (TEventList *)gROOT->FindObject("CoincOmegaList");

	num_coinc_triggers += CoincOmegaList_ptr->GetN();

	return (float)num_coinc_triggers/(float)num_omicron_triggers;
}


#include "cbc_eveto.h"

//function that does a simple significance calculation
int eveto::calc_dumb_sig(
		TTree* cbc_trigger_tree_ptr,
		TTree* omicron_trigger_tree_ptr,
		float dumb_time_seg,
		bool verbose )
{


	int num_coinc_triggers = 0;
	double end_time;

	if(cbc_trigger_tree_ptr->SetBranchAddress("end_time",&end_time)<0){
		std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
		return false;
	}

 
	int num_cbc_triggers = cbc_trigger_tree_ptr->GetEntries();	

	for (Int_t c=0; c<num_cbc_triggers; ++c) {
		cbc_trigger_tree_ptr->GetEntry(c);
	
		//Float_t	t1 = Ctime-dumb_time_seg;
		//Float_t t2 = Ctime+dumb_time_seg;
	//	TCut tc1 = "end_time>(time-dumb_time_seg) ";
	//	TCut tc2 = "end_time<(time+dumb_time_seg) ";
		char *time_window_string = new char[256];
		snprintf( time_window_string, 256, "(tend > %f) && (tend < %f)", 
			end_time - dumb_time_seg, end_time + dumb_time_seg );


		std::cerr << "before Print()" << std::endl;
		omicron_trigger_tree_ptr->Print();
		std::cerr << "after Print()" << std::endl;

		std::cerr << time_window_string << std::endl;
		std::cerr << "before Draw()" << std::endl;
		omicron_trigger_tree_ptr->Draw(">>CoincOmegaList", time_window_string, "" );

		std::cerr << "after Draw()" << std::endl;

	}
//	TEventList *CoincOmegaList_ptr= (TEventList *)gROOT->FindObject("CoincOmegaList");

//	num_coinc_triggers += CoincOmegaList_ptr->GetN();
	int num_omicron_triggers = omicron_trigger_tree_ptr->GetEntries();

	return (float)num_coinc_triggers/(float)num_omicron_triggers;
}


#include "cbc_eveto.h"

//Function that chains .root files together for CWB triggers

int eveto::read_cwb_triggers(
	TChain** cwb_tchain_tree,
	TString* cwb_trigger_path,
	bool verbose )
{
	Int_t retcode;
	if ( verbose ) std::cout << "reading CWB triggers for" << cwb_trigger_path << std::endl;

	char *veto_file_pattern = new char[512];
	snprintf( veto_file_pattern, 512, "%s/wave_111*.root", cwb_trigger_path->Data());

	retcode = cwb_tchain_tree->Add(veto_file_pattern);

	if ( retcode <= 0 ) {
    	std::cerr << "could not read any triggers from " << veto_file_pattern << std::endl;
        return false;
	}

	TChain* cwb_tchain_tree = (TTree*)file->Get("waveburst");
	return 0;
}

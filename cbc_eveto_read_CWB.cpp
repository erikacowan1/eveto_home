#include "cbc_eveto.h"

//Function that chains .root files together for CWB triggers

int eveto::read_cwb_triggers(
	TTree* cwb_tchain_tree,
	TString* cwb_trigger_path,
	bool verbose )
{
	if ( verbose ) std::cout << "reading CWB triggers for" << cwb_trigger_path << std::endl;

	snprintf( veto_file_pattern, 512, "%s/wave_111*.root", cwb_trigger_path->Data());

	TChain* cwb_tchain_tree = new TChain("cwb_tchain_tree");
	retcode = cwb_tchain_tree->Add(veto_file_pattern);

	if ( retcode <= 0 ) {
    	std::cerr << "could not read any triggers from " << veto_file_pattern << std::endl;
        return false;
	}

}

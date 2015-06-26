#include "cbc_eveto.h"

int eveto::cbc_eveto_main( 
		Long_t gps_start_time, 
		Long_t gps_end_time, 
		TString* detector, 
		TString* safe_channel_file,
		TString* cbc_trigger_database, 
		Float_t cbc_snr_threshold,
		TString* omicron_trigger_path, 
		Double_t omicron_snr_threshold, 
		Double_t omicron_cluster_window,
		TString* output_directory, 
		Float_t sig_threshold,
		Float_t dumb_veto_window,
		Int_t max_rounds,
		bool verbose
		)
{
	int retcode = 0;

	//
	// Read in the veto channel data.
	//
	// For each channel, we get a tree containing the clustered omicron triggers
	// and a tree containing the segments for which vetoes are generated. These
	// are stored in two arrays, each entry in the array is a tree for an input
	// channel.
	//


	// Create a tree containing the list of safe veto channels to be processed
	TTree* safe_channels = new TTree( "safe_channel_list", "List of safe channels to process" );
	Long64_t num_safe_channels = safe_channels->ReadFile( safe_channel_file->Data(), "channel/C" );
	if ( verbose ) {
		std::cout << "Read " << num_safe_channels << " safe channel names" << std::endl;
		safe_channels->Print();
	}

	TTree* clustered_veto_trigger_tree[num_safe_channels];
	TTree* veto_segment_tree[num_safe_channels];

	// Read in the omicron triggers for the interval that we want to process
	retcode = eveto::read_omicron_triggers(
			clustered_veto_trigger_tree, // output
			veto_segment_tree, // output
			safe_channels, // input
			omicron_trigger_path, // input
			omicron_snr_threshold, // input
			omicron_cluster_window, // input
			gps_start_time, 
			gps_end_time,
			omicron_cluster_window,
			omicron_snr_threshold,
			verbose );

	if ( retcode ) {
		std::cerr << "error reading omicron triggers" << std::endl;
		return 1;
	}


	// Read in the CWB triggers for the interval that we want to process
	/*retcode = eveto::read_cwb_triggers(
			TTree* clustered_veto_trigger_tree[],
      			TTree* veto_segment_tree[],
      			TTree* safe_channels,
     			TString* cwb_trigger_path,
      			Double_t cwb_snr_threshold,
      			Double_t cwb_cluster_window,
      			Long64_t cwb_start_time,
      			Long64_t cwb_end_time,
     			Double_t cluster_time_window,
     			Double_t cluster_snr_threshold,
      			bool verbose ) ;

	if ( retcode ) {
		std::cerr << "error reading CWB triggers" << std::endl;
		return 1;
	}

	*/


	//
	// Read in cbc triggers from database.
	//

	TTree* cbc_trigger_tree;

	retcode = eveto::read_cbc_triggers(
			&cbc_trigger_tree, // output
			cbc_trigger_database, // input
			gps_start_time, // input
			gps_end_time, // input
			detector, // input
			cbc_snr_threshold, // input
			verbose );

	if ( retcode ) {
		std::cerr << "error reading cbc triggers" << std::endl;
		return 1;
	}

	//Veto Algorithm
	//currently the segments are stored in omicron_triggers.root/segments. For now
	//assume that all segments are science segments. Grab segments from segment tree
	//and perform veto algorithm. 

	//int num_safe_channels = safe_channels->GetEntries();
	float max_sig = sig_threshold;
	int i; //counts over safe channels
	float sig[num_safe_channels]; //array to store significance of each channel
	int max_sig_index = -1; // index of the winning channel!
	int r = 1; //round counter


	//define TTrees
	TTree* cbc_trigs_round[max_rounds + 1];
	TTree* omicron_trigs_round[max_rounds + 1][num_safe_channels];
	//TTree* cbc_segs_round[max_rounds + 1][num_safe_channels]; //doesn't currently exist
	//TTree* omicron_segs_round[max_rounds + 1][num_safe_channels];


	//initialize arrays
	cbc_trigs_round[0] = cbc_trigger_tree;
	std::cerr << "Initialized CBC treel for round 0: " << cbc_trigs_round[0] << std::endl;
	//cbc_segs_round[0] = cbc_segs_tree;

	std::cerr << "Number of safe channels = " << num_safe_channels << std::endl;
	for (i=0; i<num_safe_channels; ++i) {
		omicron_trigs_round[0][i] = clustered_veto_trigger_tree[i]; //check name
		if (verbose) std::cout << "Initialized veto channel for round 0: " << omicron_trigs_round[0][i]->GetName() << std::endl;
	}


	// Begin the loop over rounds and break if all channels go below the max
	// significance threshold or we exceed the maximum number of rounds
	//float max_sig = sig_threshold;
	//int r = 1; //round counter
	while (max_sig >= sig_threshold && r <= max_rounds ) {
		if (verbose) std::cerr << "==== Processing round " << r << " of " << max_rounds << " ====" << std::endl;
		max_sig_index = -1;

		// loop over veto channels, caculate significance of channel,
		// find the channel with the highest significance, and store index
		for (i=0; i<num_safe_channels; ++i) {

			if (omicron_trigs_round[r-1][i] != NULL) {

				if ( verbose ) std::cerr << "calculating dumb significance for veto tree " << omicron_trigs_round[r-1][i]->GetName() << "(" << omicron_trigs_round[r-1][i] << ") against cbc triggers (" << cbc_trigs_round[r-1] << ")" << std::endl;

				sig[i] = eveto::calc_dumb_sig(cbc_trigs_round[r-1], omicron_trigs_round[r-1][i], dumb_veto_window, verbose);
				if ( verbose ) std::cerr << "Significance for " << omicron_trigs_round[r-1][i]->GetName() << " = " << sig[i] << std::endl;
			} else {
				sig[i] = 0;
			}

			if(sig[i] >= max_sig) {
				max_sig = sig[i];
				max_sig_index = i;
			}
		}

		if ( max_sig_index == -1 )
		{
			std::cerr << "There are no winners!" << std::endl;
			break;
		}

		if ( verbose ) std::cerr << "Winning channel was " << omicron_trigs_round[r-1][max_sig_index]->GetName() << std::endl;

		cbc_trigs_round[r] = eveto::remove_cbc_triggers(cbc_trigs_round[r-1], omicron_trigs_round[r-1][max_sig_index], verbose);

		for (i=0; i<num_safe_channels; ++i) {
			if ( (i != max_sig_index) && (omicron_trigs_round[r-1][i] != NULL) ) {
				omicron_trigs_round[r][i] = eveto::remove_omicron_triggers(omicron_trigs_round[r-1][i], omicron_trigs_round[r-1][max_sig_index], verbose);
			}
			else {
				omicron_trigs_round[r][i] = NULL;
			}
		}

#if 0
		cbc_trigs_round[r] = remove_triggers(cbc_trigs_round[r-1], omicron_trigs_round[r-1][max_sig_index],cbc_segs_round[r-1], omicron_segs_round[r-1][max_sig_index]);

		if (i != max_sig_index) {	
			omicron_trigs_round[r][i] = remove_triggers(omicron_trig_round[r-1][i], omicron_trigs_round[r-1][max_sig_index], omicron_segs_round[r-1][i], omicron_segs_round[r-1][max_sig_index]); //should segments have their own remove triggers function?

		}

		else{
			i = NULL;
		}
#endif

		if (verbose) std::cerr << "Finished round " << r << std::endl;
		if (verbose) std::cerr << "Maximum significance was " << max_sig << std::endl;
		r += 1;
	
	
	for (i=0; i<num_safe_channels; ++i) {
		safe_channels->GetEntry(i);
		char *name_safe_channel_tree = new char[256];
		snprintf(name_safe_channel_tree, 256, "VETOTRIGS:%s", safe_channels);

		retcode = eveto::cbc_eveto_analysis( cbc_trigs_round[r], omicron_trigs_round[r][i], verbose); //want the newly vetoed trigger trees for cbc and omicron

		if ( retcode ) {
			std::cerr << "error plotting new cbc triggers, in cbc_eveto_analysis" << std::endl;
			return 1;
		}
	}


	}
        std::cerr << "Eveto is finished" << std::endl;
        return 0;
}

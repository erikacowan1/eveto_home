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
	Long64_t num_veto_channels = safe_channels->ReadFile( safe_channel_file->Data(), "channel/C" );
	if ( verbose ) {
		std::cout << "Read " << num_veto_channels << " safe channel names" << std::endl;
		safe_channels->Print();
	}

	TTree* clustered_veto_trigger_tree[num_veto_channels];
	TTree* veto_segment_tree[num_veto_channels];

	// Read in the omicron triggers for the interval that we want to process
	retcode = eveto::read_omicron_triggers(
			clustered_veto_trigger_tree, // output
			veto_segment_tree, // output
			safe_channels, // input
			omicron_trigger_path, // input
			omicron_snr_threshold, // input
			omicron_cluster_window, // input
			verbose );

	if ( retcode ) {
		std::cerr << "error reading omicron triggers" << std::endl;
		return 1;
	}

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

	//
	//Veto Algorithm
	//currently the segments are stored in omicron_triggers.root/segments. For now
	//assume that all segments are science segments. Grab segments from segment tree
	//and perform veto algorithm. 

	float_t stop_sig_thresh; //to be put in for command line parsing
	float_t max_sig = 0;

	//import safe channel list and increment with i
	for ( Long64_t i = 0; i < safe_channels->GetEntries(); i++ )
	{
		if ( verbose ) {
			clustered_veto_trigger_tree[i]->Print();
			veto_segment_tree[i]->Print();
		}

		//increment number of omicron triggers in TTree with j
		int num_line_om_trig_tree = clustered_veto_trigger_tree[i]->GetEntries(); //RENAME for clarity

		for(Long64_t j=0; j<num_line_om_trig_tree; ++j){
			clustered_veto_trigger_tree[i]->GetEntry(j);


			//increment cbc triggers in TTree with k
			int num_line_cbc_trig_tree = cbc_trigs_tree[i]->GetEntries();    

			for(Long64_t k=0; k<num_line_cbc_trig_tree; ++k){
				cbc_trigs_tree[i]->GetEntry(k);


				//increment cbc_segs [currently no TTree containing cbc segments]
				

				//increment omicron segments in TTree with l
				int num_line_om_seg_tree = veto_segment_tree[i]->GetEntries(); //RENAME for clarity

				for(Long64_t l=0; l<num_line_om_seg_tree; ++l){
					veto_segment_tree[i]->GetEntry(l);

					//Calculate dumb significance of each auxilliary channel w.r.t cbc_triggers
				        float_t	sig[i] = calc_dumb_sig(cbc_trigs_tree[i], omicron_trig_tree[i]);

					//Pick winning channel with the greatest significance
					if (sig[i] >= max_sig) {
						max_sig = sig[i];
						max_sig_index = i;
						
					}
					
					//exit if max_sig < stop_sig_thresh
					else {
						std::cout << dumb_sig[i] << "is less than" << max_sig << std::endl;
						exit 0;
					}

					//Take winning channel, grab times of trigger, remove those times from all other channels (cbc, omicron)
					Given the winning channel i
					Triggers to veto are stored in an array (of TTrees?) created calc_dum_sig, in the array trigs_2_veto[index].
					use TCut function to remove times (in reality, to remove segments) from all other segment channels. 
					Do NOT modify the trigger database, only narrow the segment windows
					Rinse and Repeat.  
					

				}
			}
		}
	}

}

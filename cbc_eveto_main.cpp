#include "cbc_eveto.h"

int eveto::cbc_eveto_main(
	Long_t gps_start_time,
	Long_t gps_end_time,
	TString* detector, 
	TString* main_channel,
        TString* safe_channel_file,
        TString* cbc_trigger_database, 
        Float_t cbc_snr_threshold,
        TString* omicron_trigger_path, 
        Double_t omicron_snr_threshold, 
        Double_t omicron_cluster_window,
        TString* cwb_trigger_path,
        Double_t cwb_snr_threshold,
        Double_t cluster_time_window,
        Double_t cluster_snr_threshold, 
        Float_t sig_threshold,
        Float_t dumb_veto_window,      
        Int_t max_rounds,
        TString* output_directory, 
        bool verbose )
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

	TTree* cwb_trigger_tree = new TTree("waveburst","waveburst");
	TChain* input_cwb_chain = new TChain( "waveburst", "input_cwb_chain");
	//TChain* veto_trigger_chain = new TChain( "waveburst", "cwb_unclustered_tree" );
	

	//
	//
	// Read in the CWB triggers for the interval that we want to process
	//
	//
	
	TString cwb = "cwb";
	if (main_channel == &cwb)
	{

	retcode = eveto::read_cwb_triggers(
		cwb_trigger_tree,
		input_cwb_chain,
		cwb_trigger_path,
		gps_start_time,
		gps_end_time,
		verbose ) ;

	if ( retcode ) {
		std::cerr << "error reading CWB triggers" << std::endl;
		return 1;
	}
	
	}


	//
	//
	// Read in cbc triggers from database.
	//
	//

	TTree* cbc_trigger_tree;
	TString cbc = "cbc";
	if (main_channel == &cbc)
	{


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
	}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

	//TEMP CWB Veto Algorithm
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
	TTree* main_channel_trigs_round[max_rounds + 1];
	TTree* omicron_trigs_round[max_rounds + 1][num_safe_channels];
	//TTree* cbc_segs_round[max_rounds + 1][num_safe_channels]; //doesn't currently exist
	//TTree* omicron_segs_round[max_rounds + 1][num_safe_channels]; //doesn't currently exist
	

	// For CBC option, intializing array
	
	if ( detector == &cbc ) {
		main_channel_trigs_round[0] = cbc_trigger_tree;
	}


	// For CWB option, initializing array
	
	if ( detector == &cwb ) {
		main_channel_trigs_round[0] = cwb_trigger_tree;
	}

	std::cerr << "Initialized Main Channel" << main_channel << "TTree for round 0:" << main_channel_trigs_round[0] << std::endl;

	std::cerr << "Number of safe channels = " << num_safe_channels << std::endl;

	for (i = 0; i < num_safe_channels; ++i) {
		omicron_trigs_round[0][i] = clustered_veto_trigger_tree[i];

		if ( verbose ) std::cout<<"Initialized veto channel for round 0:" << omicron_trigs_round[0][i]->GetName() << std::endl;
	}


	while (max_sig >= sig_threshold && r <= max_rounds) {
		if ( verbose ) std::cerr << "====Processing round" << r << "of" << max_rounds << "====" << std::endl;
	max_sig_index = -1;

	for(i=0; i < num_safe_channels; ++i) {

		if(omicron_trigs_round[r-1][i] !=NULL) {

			if( verbose ) std::cerr << "calculating dumb significance for veto tree" << omicron_trigs_round[r-1][i]->GetName() << "(" << omicron_trigs_round[r-1][i] << ") against main channel" << main_channel << "triggers (" << main_channel_trigs_round[0] << std:: endl; 


			
			sig[i] = eveto::calc_dumb_sig(main_channel_trigs_round[r-1], omicron_trigs_round[r-1][i], main_channel, dumb_veto_window, verbose); 

			if( verbose ) std::cerr << "Significance for" << omicron_trigs_round[r-1][i]->GetName() << "=" << sig[i] << std::endl;
		} else {
			sig[i] = 0;
		}

		if( sig[i] >= max_sig ) {
			max_sig = sig[i];
			max_sig_index = i;
		}
	}

	if( max_sig_index == -1)
	{
		std::cerr << "There are no winners!" << std:: endl;
		break;
	}

	if( verbose ) std::cerr << "Winning channel was" << omicron_trigs_round[r-1][max_sig_index]->GetName() << std::endl;
/*
	TTree* omicron_trigger_tree_veto;
	retcode = eveto::remove_main_channel_triggers(
			main_channel_trigs_round[r-1], 
			omicron_trigger_tree_veto,
			omicron_trigs_round[r-1][max_sig_index],
			main_channel,
			verbose);

	main_channel_trigs_round[r] = main_channel_trigger_tree_out_ptr; */
	main_channel_trigs_round[r] = eveto::remove_main_channel_triggers(main_channel_trigs_round[r-1], omicron_trigs_round[r-1][max_sig_index], main_channel, verbose);

	for( i=0; i<num_safe_channels; ++i) {
		if( (i !=max_sig_index) && (omicron_trigs_round[r-1][i] !=NULL) ) {
			//omicron_trigs_round[r][i] = remove_omicron_triggers(omicron_trigs_round[r-1][i], omicron_trigger_tree_veto, omicron_trigs_round[r-1][max_sig_index], verbose);
			omicron_trigs_round[r][i] = remove_omicron_triggers(omicron_trigs_round[r-1][i], omicron_trigger_tree_veto, verbose);

		}
		else {
			omicron_trigs_round[r][i] = NULL;
		}
	}
	if( verbose ) std::cerr << "Finished round" << r << std::endl;
	if( verbose ) std::cerr << "Maximum significance was" << max_sig << std::endl;
	return 0; 

	}
	return 0;
}

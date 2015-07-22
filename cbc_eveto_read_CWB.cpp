#include "cbc_eveto.h"

bool read_cwb_channel(
	TChain* veto_trigger_chain,
	TString* cwb_trigger_path,
	bool verbose)
{
	Int_t retcode;

	char *veto_file_pattern = new char[512];
	snprintf( veto_file_pattern, 512, "%s/wave_111*.root", cwb_trigger_path->Data());
	if( verbose ) std::cout<<"Adding CWB ROOT files from"<<veto_file_pattern<<std::endl;

	//read triggers from file to TChain
	retcode = veto_trigger_chain->Add(veto_file_pattern);

	if( retcode <= 0 ) {
		std::cerr<<"could not read any trigers from"<<veto_file_pattern<<std::endl;
		return false;
	}

	return true;
}

int eveto::read_cwb_triggers(
	TTree* cwb_unclustered_tree,
	TChain* input_cwb_chain,
	TString* cwb_trigger_path,
	Long64_t cwb_start_time,
	Long64_t cwb_end_time,
	bool verbose)
{
		TChain* veto_trigger_chain = new TChain( "waveburst", "cwb_unclustered_tree" );
		bool read_retcode = read_cwb_channel( veto_trigger_chain, cwb_trigger_path, verbose);
		
		if ( ! read_retcode ){
			std::cerr<<"error reading cwb triggers"<<std::endl;
			return 1;
		}
		

	if( verbose ) std::cout<<"cwb_tree_maker is starting to import"<<input_cwb_chain->GetEntries()<<"unclustered triggers"<<std::endl;

	 Double_t Ttime, Tfrequency, Tduration, Tstart, Tstop, Tsnr, Trho, Tnetcc, Tneted, Tlikelihood, Tecor, TECOR, Tifo, Trate, Tphi, Ttheta, Tpsi,Tnull, Thrss, Tnoise, Tlow, Thigh, Tbandwidth, Tstrain, Trun, Tsize;

	if(input_cwb_chain->SetBranchAddress("time",&Ttime)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing time branch"<<std::endl ;
    		return false;
  	}
	if(input_cwb_chain->SetBranchAddress("frequency",&Tfrequency)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing frequency branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("duration",&Tduration)<0){
   		std::cerr <<"ReadTriggers::GetInputTree: missing duration branch"<<std::endl ;
   		return false;
  	}
 	if(input_cwb_chain->SetBranchAddress("start",&Tstart)<0){
 		std::cerr <<"ReadTriggers::GetInputTree: missing start branch"<<std::endl ;
 	   	return false;
	}
 	if(input_cwb_chain->SetBranchAddress("stop",&Tstop)<0){
		std::cerr <<"ReadTriggers::GetInputTree: missing stop branch"<<std::endl ;
		return false;
	}
	if(input_cwb_chain->SetBranchAddress("snr",&Tsnr)<0){
 		std::cerr <<"ReadTriggers::GetInputTree: missing snr branch"<<std::endl ;
		return false;
	}
	if(input_cwb_chain->SetBranchAddress("rho",&Trho)<0){
		std::cerr <<"ReadTriggers::GetInputTree: missing rho branch"<<std::endl ;
		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("netcc",&Tnetcc)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing netcc branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("neted", &Tneted)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing neted branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("likelihood", &Tlikelihood)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing likelihood branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("ecor", &Tecor)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing ecor branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("ECOR", &TECOR)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing ECOR branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("ifo", &Tifo)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing ifo branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("rate", &Trate)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing rate branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("phi", &Tphi)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing phi branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("theta", &Ttheta)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing theta branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("psi", &Tpsi)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing psi branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("null", &Tnull)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing null branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("hrss", &Thrss)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing hrss branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("noise", &Tnoise)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing noise branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("low", &Tlow)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing low branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("high", &Thigh)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing high branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("bandwidth", &Tbandwidth)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing bandwidth branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("strain", &Tstrain)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing amplitude branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("run", &Trun)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing run branch"<<std::endl ;
    		return false;
  	}
  	if(input_cwb_chain->SetBranchAddress("size", &Tsize)<0){
    		std::cerr <<"ReadTriggers::GetInputTree: missing size branch"<<std::endl ;
    		return false;
  	}

  	Double_t Ctime, Cfrequency, Cduration, Cstart, Cstop, Csnr, Crho, Cnetcc, Cneted, Clikelihood, Cecor, CECOR, Cifo, Crate, Cphi, Ctheta, Cpsi, Cnull, Chrss, Cnoise, Clow, Chigh, Cbandwidth, Cstrain, Crun, Csize;

  	cwb_unclustered_tree->Branch("time",       &Ctime,      "time/D");
  	cwb_unclustered_tree->Branch("frequency",  &Cfrequency, "frequency/D");
  	cwb_unclustered_tree->Branch("duration",   &Cduration,  "duration/D");
  	cwb_unclustered_tree->Branch("start",      &Cstart,     "start/D");
  	cwb_unclustered_tree->Branch("stop",       &Cstop,      "stop/D");
  	cwb_unclustered_tree->Branch("snr",        &Csnr,       "snr/D");
 	cwb_unclustered_tree->Branch("rho",        &Crho,       "rho/D");
  	cwb_unclustered_tree->Branch("netcc",      &Cnetcc,     "netcc/D");
  	cwb_unclustered_tree->Branch("neted",      &Cneted,     "neted/D");
  	cwb_unclustered_tree->Branch("likelihood", &Clikelihood,"likelihood/D");
  	cwb_unclustered_tree->Branch("ecor",       &Cecor,      "ecor/D");
  	cwb_unclustered_tree->Branch("ECOR",       &CECOR,      "ECOR/D");
  	cwb_unclustered_tree->Branch("ifo",        &Cifo,       "ifo/D");
  	cwb_unclustered_tree->Branch("rate",       &Crate,      "rate/D");
  	cwb_unclustered_tree->Branch("phi",        &Cphi,       "phi/D");
  	cwb_unclustered_tree->Branch("theta",      &Ctheta,     "theta/D");
  	cwb_unclustered_tree->Branch("psi",        &Cpsi,       "psi/D");
  	cwb_unclustered_tree->Branch("null",       &Cnull,      "null/D");
  	cwb_unclustered_tree->Branch("hrss",       &Chrss,      "hrss/D");
  	cwb_unclustered_tree->Branch("noise",      &Cnoise,     "noise/D");
  	cwb_unclustered_tree->Branch("low",        &Clow,       "low/D");
  	cwb_unclustered_tree->Branch("high",       &Chigh,      "high/D");
  	cwb_unclustered_tree->Branch("bandwidth",  &Cbandwidth, "bandwidth/D");
  	cwb_unclustered_tree->Branch("strain",     &Cstrain,    "strain/D");
  	cwb_unclustered_tree->Branch("run",        &Crun,       "run/D");
  	cwb_unclustered_tree->Branch("size",       &Csize,      "size/D");

  	Cstop=0.0;

	for( Long64_t t=0; t<input_cwb_chain->GetEntries(); t++)
	{
		input_cwb_chain->GetEntry(t);

		if( Tstop < cwb_start_time || Tstart > cwb_end_time ) 
		{
			cwb_unclustered_tree->Fill();
		}

		if( verbose ) std::cout<<"cwb_tree_maker filled" << cwb_unclustered_tree->GetEntries()<<"triggers in cwb_unclustered tree"<<std::endl;
		return true;
	}
	return 0;
}

#include "cbc_eveto.h"

 // Create the cbc trigger tree and store the pointer in a local variable
  TTree *cbc_trigs_tree = *cbc_trigger_tree_ptr = new TTree("cbc-triggers","cbc-triggers");

  // Create storage for the leaves
  Double_t start_time, end_time;
  Float_t snr, chisq, chisqdof, newsnr, snr_sq, mass1, mass2, mtotal, mchirp, eta, ttotal;

  // Create the branches for the tree
  cbc_trigs_tree->Branch( "start_time" , &start_time, "start_time/D" );
  cbc_trigs_tree->Branch( "end_time" , &end_time, "end_time/D" );
  cbc_trigs_tree->Branch( "snr" , &snr, "snr/F" );
  cbc_trigs_tree->Branch( "chisq" , &chisq, "chisq/F" );
  cbc_trigs_tree->Branch( "chisqdof" , &chisqdof, "chisqdof/F" );
  cbc_trigs_tree->Branch( "newsnr" , &newsnr, "newsnr/F" );
  cbc_trigs_tree->Branch( "snr_sq" , &snr_sq, "snr_sq/F" );
  cbc_trigs_tree->Branch( "mass1", &mass1, "mass1/F" );
  cbc_trigs_tree->Branch( "mass2", &mass2, "mass2/F" );
  cbc_trigs_tree->Branch( "mtotal", &mtotal, "mtotal/F" );
  cbc_trigs_tree->Branch( "mchirp", &mchirp, "mchirp/F" );
  cbc_trigs_tree->Branch( "eta", &eta, "eta/F" );
  cbc_trigs_tree->Branch( "ttotal", &ttotal, "ttotal/F" );


  TString cbc_database_query( "select end_time, end_time_ns, ttotal, snr, chisq, chisq_dof, mass1, mass2, mtotal, mchirp, eta from sngl_inspiral where " );
  cbc_database_query += "ifo = '";
  cbc_database_query += detector->Data();
  cbc_database_query += "'";
  cbc_database_query += " and end_time >= ";
  cbc_database_query += gps_start_time;
  cbc_database_query += " and end_time < ";
  cbc_database_query += gps_end_time;
  cbc_database_query += " and snr > ";
  cbc_database_query += cbc_snr_threshold;
  cbc_database_query += ";";

  if ( verbose ) std::cout << "Querying cbc triggers with: " << cbc_database_query << std::endl;


  // Connect to the specified database
  TString database_url( "sqlite://" );
  database_url += cbc_trigger_database->Data();
  TSQLServer* serv = TSQLServer::Connect( database_url, "", "" );
  if ( serv == NULL ) {
    std::cerr << "error opening database " << database_url << std::endl;
    return 1;
  }
  // Initialize the SQL statement to get the triggers
  TSQLStatement* stmt = serv->Statement(cbc_database_query, 50000);
  if ( stmt == NULL ) {
    std::cerr << "error creating database statement " << cbc_database_query << std::endl;
    return 1;
  }

  // Process the database query 
  if ( stmt->Process() ) {
    // Store the result of the statement in the buffer 
    stmt->StoreResult();    

// process the data returned in the buffer 
    while ( stmt->NextResultRow() ) {
      end_time = stmt->GetDouble(0) + stmt->GetDouble(1) / 1e9;
      start_time = end_time - stmt->GetDouble(2);

      snr = stmt->GetDouble(3);
      snr_sq = snr*snr;
      chisq = stmt->GetDouble(4);
      chisqdof = 2.0 * stmt->GetDouble(5) - 2.0;

      mass1 = stmt->GetDouble(6);
      mass2 = stmt->GetDouble(7);
      mtotal = stmt->GetDouble(8);
      mchirp = stmt->GetDouble(9);
      eta = stmt->GetDouble(10);

      // compute the new SNR 
      if ( chisq > chisqdof ) {
        newsnr = snr * pow(((1.0 + pow((chisq/chisqdof), 3.0))/2.0), (-1.0 /6.0));
      }
      else {
        newsnr = snr;
      }

      // store the data in the tree and the histograms;    
      cbc_trigs_tree->Fill();

   }
  }
  else {
    std::cerr << "error processing sql statement" << std::endl;
    return 1;
  }

  // Close the database 
  delete serv;
      TCanvas* c1 = new TCanvas();
      c1->SetLogy();
      TFile * f =TFile::Open("cbc_eveto.root"); 
      TH1F *h1 = new TH1F("h1", "CBC", 70, 5.5, 9);
      cbc_trigs_tree->Draw("newsnr>>h1");
  if (verbose) std::cout << "Read " << cbc_trigs_tree->GetEntries() << " cbc triggers" << std::endl;
 return 0;
}

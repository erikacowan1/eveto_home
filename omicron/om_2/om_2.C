#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TTree.h>
#include <TSQLServer.h>
#include <TSQLStatement.h>
#include <stdio.h>
#include <string.h>
#include <THStack.h>
#include <TBrowser.h>
#include <TFile.h>
#include <TChain.h>
#include <TBranch.h>



int main( int argc, char* argv[] ) {
//char path[512];
//char channel[512];
//snprintf( path, 512, argv[1]);
//snprintf( channel,  512, argv[2]);
char* snr;
char* time;
char* frequency;
char* q;
char* tstart;
char* tend;
char* fstart;
char* fend;
char* amplitude;

TChain chain("triggers");
chain.Add("/home/dbrown/public_html/detchar/omicron/L1/triggers/L0:PEM-BSC1_ACCX/*.root");

chain.SetBranchAddress("snr", &snr);
chain.SetBranchAddress("time" , &time);
chain.SetBranchAddress("frequency" , &frequency);
chain.SetBranchAddress("q" , &q);
chain.SetBranchAddress("tstart" , &tstart);
chain.SetBranchAddress("tend" , &tend);
chain.SetBranchAddress("fstart" , &fstart);
chain.SetBranchAddress("fend" , fend);
chain.SetBranchAddress("amplitude" , &amplitude);

Int_t nevent = chain.GetEntries();
TH1F *h1 = new TH1F("h1", "Blah", 100, 0, 100);
//TH1F *h2 = new TH1F("h2", " ", 100, 0, 100);
for (Int_t i=0;i<nevent;i++) { 
chain.GetEvent(i); 
triggers->Draw("snr");

 }

}

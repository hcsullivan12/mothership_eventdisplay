#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

const std::vector<unsigned> febChannelsVec = {12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
const std::vector<unsigned> sipmIDVec      = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
//const std::vector<float>    sipmGains      = {57.72,60.21,59.33,59.16,60.03,58.42,59.77,58.08,58.91,62.64,50.51,48.67,37.07,37.78,25,18.46};
const std::vector<float>    sipmGains      = {60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60};
const std::vector<float>    sipmPed        = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
const unsigned ledTriggers = 5000;

std::vector<std::vector<Double_t>> param;
std::string theOutputPath;
std::string theTreePath;
std::string theEvdPath;

//------------------------------------------------------------------------
void doAnalyze(std::string treepath, std::string outputpath, std::string evdpath)
{
  std::cout << "Running doAnalyze...\n";

  theTreePath   = treepath;
  theOutputPath = outputpath;
  theEvdPath    = evdpath;

  UShort_t chg[32];
  TFile theFile(theTreePath.c_str(), "READ");
  TTree* theTree = (TTree*)theFile.Get("mppc");
  if (!theTree) { std::cout << "WARNING: Couldn't find mppc tree\n"; return; }
  theTree->SetBranchAddress("chg", chg);
  unsigned nentries = theTree->GetEntries();

  // Get the ADC integrals
  std::vector<float> theCounts(febChannelsVec.size(), 0.);
  for (Long64_t jentry=0; jentry<nentries; jentry++) 
  {
    theTree->GetEntry(jentry);
   
    size_t counter(0);
    for (const auto& febId : febChannelsVec)
    {
      theCounts[counter] += chg[febId];
      counter++;
    }
  }

  // Normalize to one trigger
  // And N = (1/G) * integral
  size_t counter(0);
  for (auto& i : theCounts) 
  {
    i /= ledTriggers;
    i /= sipmGains[counter];
    counter++;
  }

  // Update the file for evd
  std::cout << "Updating daq file for event display...\n";
  std::fstream outfile(theEvdPath, ios::in | ios::out | ios::trunc);
  if(outfile.is_open())
  {
    for (const auto &c : theCounts)
    {
      outfile << c << " ";
    }
  }
  outfile.close();

  gROOT->cd("Rint:/");
  theTree->Reset();
}

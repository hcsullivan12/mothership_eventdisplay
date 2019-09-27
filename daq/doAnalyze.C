#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

const std::vector<unsigned> febChannelsVec = { 14, 15, 16, 17, 18, 19, 20, 21 };
const std::vector<unsigned> sipmIDVec      = {  2,  4,  5,  6,  7,  1,  8,  3 };
const std::vector<float>    sipmGains      = {  65.7, 66.4, 68.6, 65.7, 63.9, 64.0, 57.9, 59.0};
const std::vector<float>    sipmPed        = {  44.3, 41.4, 56.0, 31.5, 57.9, 50.8, 50.9, 63.5};
const unsigned ledTriggers = 60000;

std::vector<std::vector<Double_t>> param;
std::vector<TH1F> histVec;
std::string theOutputPath;
std::string theTreePath;

void InitializeHistos();
void Ana();

/**********************/
void doAnalyze(std::string inputpath, std::string outputpath)
{
  theTreePath   = inputpath;
  theOutputPath = outputpath;
  // Initialize histograms
  InitializeHistos();

  UShort_t chg[32];
  TFile* theFile = new TFile(theTreePath.c_str(), "READ");
  TTree* theTree = (TTree*)theFile->Get("mppc");
  theTree->SetBranchAddress("chg", chg);
  unsigned nentries = theTree->GetEntries();

  for (Long64_t jentry=0; jentry<nentries; jentry++) 
  {
    theTree->GetEntry(jentry);
   
    // Fill the histos
    unsigned counter(0);
    for (const auto& febID : febChannelsVec)
    {
      histVec[counter].Fill(chg[febID]);
      counter++;
    }
  }

  // Plotting/fitting
  Ana();
  gApplication->Terminate(0);
}

/**********************/
void InitializeHistos()
{
  for (const auto& s : sipmIDVec)
  {
    std::string name = "SiPM"+std::to_string(s)+"_hist";
    histVec.push_back( TH1F(name.c_str(), name.c_str(), 200, 0, 2000) );
  }
}

/**********************/
void Ana()
{
  //gStyle->SetErrorX(0);
  gStyle->SetOptFit(1);

  unsigned counter(0);
  for (auto& h : histVec)
  {
    float mean = h.GetMean();
    float sig  = h.GetStdDev();

    TF1 fit("fit", "gaus", mean - sig, mean + sig);
    fit.SetParameters(1000, mean, sig);
    h.Fit(&fit, "RQ");
    std::vector<Double_t> temp = {fit.GetParameter(0), fit.GetParameter(1), fit.GetParameter(2)};
    param.push_back(temp);

    //h.SetMarkerStyle(8);
    //h.SetMarkerSize(1);
    //h.Draw("pe1");

    counter++;
  }

  // find area within 1 sigma of mean
  counter = 0;
  std::vector<unsigned> theCounts;
  for (const auto& p : param)
  {
    TF1 gaus("fit", "gaus");
    gaus.SetParameters(p[0]/ledTriggers, 0., p[2]);

    Double_t theArea = gaus.Integral(-2*p[2], 2*p[2]);

    // How many photons does the mean correspond to?
    unsigned n = p[1]/sipmGains[counter];
    //cout << std::round(theArea*n) << endl;
    theCounts.push_back(theArea*n);
    counter++;
  }

  // Write to our file
  std::ofstream outfile(theOutputPath.c_str());
  // Output dummy line
  outfile << "0 0\n";
  for (const auto& c : theCounts) outfile << c << " ";
}

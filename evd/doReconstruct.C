/**
 * @file RecoHelper.C
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Interface to reconstruction algorithm for event display.
 * @date 07-04-2019
 * 
 */

#include "Reconstructor.h"
R__LOAD_LIBRARY(mthship_evd/libReconstructor.so)
#include "Pixel.h"

/**
 * @brief 
 * 
 */
struct RecoHelper
{
  typedef std::shared_ptr<std::vector<majutil::Pixel>> pixelTablePtr_t;
  typedef std::map<size_t, size_t> counts_t;

  // Some useful variables
  std::string     theMethod;
  std::string     thePixelPath;
  std::string     theOpRefPath;
  double          theDiskRadius;
  double          theGamma;
  double          thePixelSpacing;
  size_t          theDoPenalized;
  size_t          thePenalizedIter;
  size_t          theUnpenalizedIter;
  pixelTablePtr_t thePixelVec = nullptr;
  counts_t        theData;
};

// Protos
void LoadPixelization(RecoHelper& recoHelper);
void LoadOpRefTable(RecoHelper& recoHelper);
void Reconstruct(const RecoHelper& recoHelper);

/**
 * @brief Method to load pixelization scheme.
 * 
 */
void LoadPixelization(RecoHelper& recoHelper)
{
  if (!recoHelper.thePixelVec) recoHelper.thePixelVec = std::make_shared<std::vector<majutil::Pixel>>();
  recoHelper.thePixelVec->clear();

  // Make pixels for each position
  std::ifstream f(recoHelper.thePixelPath.c_str());
  if (!f.is_open())
  { 
    std::cerr << "PixelTable::Initialize() Error! Cannot open pixelization file! << "<<recoHelper.thePixelPath<<"\n";
    exit(1);
  }
  
  std::cout << "Reading pixelization file...\n";

  // Table must be:
  //
  //   pixelID x y
  // 

  // First read top line 
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);
  if(string1 != "pixelID" || 
     string2 != "x"       || 
     string3 != "y")
  {
    std::cout << "PixelTable::Initialize() Error! ReferenceTable must have "
              << "\'pixelID mppcID probability\' as header.\n";
    exit(1);
  }

  // For computing the size
  unsigned thePixelCount(0);
  float aPixelPos(0);
  float min = std::numeric_limits<float>::max(); 
  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned pixelID = std::stoi(string1);
    float    x       = std::stof(string2);
    float    y       = std::stof(string3);
    thePixelCount++;
    if (thePixelCount == 1) aPixelPos = x; 
    else min = std::abs(aPixelPos-x) < min && std::abs(aPixelPos-x) > 0 ? std::abs(aPixelPos-x) : min;
        
    // Get r and theta just in case we need it
    float r     = std::sqrt(x*x + y*y);
    float thetaDeg(0);
    if (r > 0.01) thetaDeg = std::asin(std::abs(y/r))*180/M_PI;
    // Handle theta convention
    if (x <  0 && y >= 0) thetaDeg = 180 - thetaDeg;
    if (x <  0 && y <  0) thetaDeg = 180 + thetaDeg;
    if (x >= 0 && y <  0) thetaDeg = 360 - thetaDeg; 
 
    recoHelper.thePixelVec->emplace_back(pixelID, x, y, r, thetaDeg);
    recoHelper.thePixelVec->back().SetSize(recoHelper.thePixelSpacing);
  }

  f.close();

  // Sort 
  std::sort( recoHelper.thePixelVec->begin(), recoHelper.thePixelVec->end(), [](const majutil::Pixel& left, const majutil::Pixel& right) { return left.ID() < right.ID(); } );

  std::cout << "Initialized " << recoHelper.thePixelVec->size() << " " << min << "x" << min << "cm2 pixels...\n";
}

/**
 * @brief Method to load lookup table.
 * 
 */
void LoadOpRefTable(RecoHelper& recoHelper)
{
  // Make sure pixels have been initialized
  assert(recoHelper.thePixelVec->size() != 0 && "Pixels have not been initialized!");

  // Read in reference table
  std::ifstream f(recoHelper.theOpRefPath.c_str());
  if (!f.is_open())
  { 
    std::cout << "PixelTable::LoadReferenceTable() Error! Cannot open reference table file!\n";
    exit(1);
  }
  std::cout << "Reading reference table file...\n";
 
  // Table must be:
  //
  //    pixelID mppcID probability
  //
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);

  if (string1 != "pixelID" || 
      string2 != "mppcID"  || 
      string3 != "probability")
  { 
    std::cout << "PixelTable::LoadReferenceTable() Error! ReferenceTable must have "
              << "\'pixelID mppcID probability\' as header.\n";
    exit(1);
  } 
  
  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned pixelID = std::stoi(string1);
    unsigned mppcID  = std::stof(string2);
    float    prob    = std::stof(string3);

    // This assumes the pixels have been ordered
    recoHelper.thePixelVec->at(pixelID-1).AddReference(mppcID, prob);
  }
  f.close();
}

/**
 * @brief Method to start reconstruction algorithm.
 * 
 */
void Reconstruct(const RecoHelper& recoHelper)
{
  // Initialize the reconstructor
  majreco::Reconstructor theReconstructor(recoHelper.theData, recoHelper.thePixelVec, recoHelper.theDiskRadius);
  if (recoHelper.theMethod == "emml")
  {
    theReconstructor.DoEmMl(recoHelper.theGamma,
                            recoHelper.theUnpenalizedIter,
                            recoHelper.thePenalizedIter,
                            recoHelper.theDoPenalized); 
  }
  else theReconstructor.DoChi2();

  theReconstructor.Dump();
  // Write the reconstructed image
  TFile f("recoanatree.root", "RECREATE");
  theReconstructor.MLImage()->Write();
  theReconstructor.Chi2Image()->Write();
  
  // Write the expected data
  auto expdata = theReconstructor.ExpectedCounts();
  TH1I h("expdata", "expdata", expdata.size(), 0.5, expdata.size()+0.5);
  for (const auto& d : expdata) h.SetBinContent(d.first, d.second);
  h.Write();
  f.Close();
  cout << "Finished!" << endl;
}

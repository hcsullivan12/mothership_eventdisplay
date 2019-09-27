/**
 * @file Reconstructor.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief The SiPM wheel reconstruction algorithm.
 * @date 07-04-2019
 * 
 */

#ifndef MAJRECO_RECONSTRUCTOR_H
#define MAJRECO_RECONSTRUCTOR_H

#include "Pixel.h"

#include <map>
#include <list>
#include <memory>

#include "TH2.h"
#include "TF2.h"

namespace majreco
{

class Reconstructor 
{

public:

  // Pixel structure for minimum chi2
  struct Chi2Pixel {
    float chi2;
    std::vector<float> vertex;
    size_t id;
  };


  /**
   * @brief Construct a new Reconstructor object
   * 
   * @param data Map from detector ID to detector counts
   * @param pixelVec Vector of pixels
   * @param diskRadius Radius of the disk
   */
  Reconstructor(const std::map<size_t, size_t>&              data,
                std::shared_ptr<std::vector<majutil::Pixel>> pixelVec,
                const float&                                 diskRadius);
  ~Reconstructor();


  void Clean() { if(!fMLHist) delete fMLHist; if(!fMLGauss) delete fMLGauss; if(!fChi2Hist) delete fChi2Hist;}


  /**
   * @brief Entry point to reconstruction algorithm.
   * 
   * @param gamma Regularization parameter
   * @param upStop Iteration number to stop unpenalized method
   * @param pStop Iteration number to stop penalized method
   * @param doPenalized Option to run penalized method
   */
  void DoEmMl(const float&  gamma, 
              const size_t& pStop,
              const size_t& upStop,
              const bool&   doPenalized);    

  /**
   * @brief Reconstructs mean position based on Chi2 minimization.
   * 
   */
  void DoChi2();              

  /**
   * @brief Dump configuration and reconstruction results.
   * 
   */
  void Dump();

  const double   ML()    const { return fMLLogLikelihood; }
  const float    X()     const { return fEstimateX; }
  const float    Y()     const { return fEstimateY; }
  const size_t   TotalLight() const { return fEstimateTotalLight; }
  TH2F*          MLImage() { return fMLHist; }
  TH2F*          Chi2Image() { return fChi2Hist; }
  const std::map<size_t,size_t> ExpectedCounts();
    
private:
  
  /**
   * @brief Simple factorial function.
   *
   */
  inline int doFactorial(const size_t& n) 
  { 
    if (n <= 1) return 1;
    return n * doFactorial(n-1);
  }

  /**
   * @brief Do unpenalized version of reconstruction.
   * 
   */
  void DoUnpenalized();

  /**
   * @brief Do penalized version of reconstruction. Same as 
   *        unpenalized except uses different formula to update
   *        estimates.
   * 
   */
  void DoPenalized();

  /**
   * @brief Initializes the pixel intensities using a uniform distribution.
   * 
   */
  void InitPixelList();

  /**
   * @brief Calculates the unpenalized pixel values for the image.
   * 
   */
  void UnpenalizedEstimate();

  /**
   * @brief Calculates the penalized pixel values for the image.
   * 
   */
  void PenalizedEstimate();

  /**
   * @brief Calulate log likelihood using current estimates.
   * @todo Clean up this area.
   * 
   */
  float CalculateLL();
  float CalculateMean(const size_t& sipmID);

  /**
   * @brief Calculates the sum in the denominator of the Money Formula.
   * 
   */
  float DenominatorSum(const size_t& sipmID);

  /**
   * @brief Entry point for applying the Moneyu Formula.
   * 
   * @param pixelID The pixel ID to update.
   * @param theEst The current estimate.
   * @param referenceTable The lookup table for this pixel.
   * @return float The updated pixel intensity.
   */
  float MoneyFormula(const float&              theEst,
                     const std::vector<float>& referenceTable);

  /**
   * @brief Method to check for convergence. Currently returns false.
   * @todo Finish writing.
   * 
   * @return true If converged.
   * @return false If not.
   */
  bool CheckConvergence();

  /**
   * @brief Resets the pixel estimates with the updated estimates.
   * 
   */
  void Reset();

  /**
   * @brief Updates the ML histogram with current estimates. Fits 
   *        distribution using a 2D gaussian function to find the
   *        mean x and y coordinates.
   * 
   */
  void UpdateHistogram();

  /**
   * @brief Updates the priors used in the penalized reconstruction.
   * 
   */
  void InitializePriors();
   
  TH2F*                        fMLHist;                ///< The reconstructed image 
  TH2F*                        fChi2Hist;              ///< The Chi2 image
  TF2*                         fMLGauss;               ///< Gaussian fit to point source
  double                       fMLLogLikelihood;       ///< Log likelihood for the MLE
  size_t                       fEstimateTotalLight;    ///< MLE for total light
  float                        fEstimateX;             ///< MLE for x (cm)
  float                        fEstimateY;             ///< MLE for y (cm)
  float                        fDiskRadius;            ///< Disk radius 
  std::shared_ptr<std::vector<majutil::Pixel>> fPixelVec;  ///< List of pixels
  std::vector<float>           fDenomSums;                 ///< Map of sipm to denominator sum
  std::map<size_t, size_t>     fData;                      ///< Measured counts (sipm, np.e.)
  std::vector<float>           fLogLikehs;             ///< Container for logL at each iteration 
  std::vector<float>           fPriors;                ///< Container for priors used in penalized reconstruction
  float                        fGamma;                 ///< Strength parameter for penalty function
  size_t                       fUnpenalizedIterStop;   ///< Iteration number to hault unpenalized reconstruction
  size_t                       fPenalizedIterStop;     ///< Iteration number to hault penalized reconstruction
  Chi2Pixel                    fChi2Pixel;
  std::string                  fMethod;                ///< Most recent reconstruction method
};
}

#endif

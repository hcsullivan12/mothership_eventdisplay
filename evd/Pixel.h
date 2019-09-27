/**
 * @file Pixel.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Simple pixel structure.
 * @date 07-04-2019
 *
 */

#include <vector>

#ifndef PIXEL_H
#define PIXEL_H

namespace majutil
{

class Pixel
{

using AReferenceTable = std::vector<float>;

public:
  Pixel(const unsigned& id,
        const float& x,
        const float& y,
        const float& r,
        const float& theta)
    : fID(id),
      fX(x),
      fY(y),
      fR(r),
      fTheta(theta)
   {
     fReferenceTable.clear();
   };
  ~Pixel(){};

  float       X()     const { return fX; };
  float       Y()     const { return fY; };
  float       R()     const { return fR; };
  float       Theta() const { return fTheta; };
  float       Size()  const { return fSize; };
  unsigned    ID()    const { return fID; };
  float       Intensity() const { return fIntensity; };
  AReferenceTable const& ReferenceTable() const { return fReferenceTable; };

  void SetSize(const float& s) { fSize = s; };
  void SetIntensity(const float& i) { fIntensity = i; };
  void AddReference(const unsigned& mppcID, const float& prob)
  {
    if (mppcID > fReferenceTable.size())
    {
      fReferenceTable.resize(mppcID);
    }
    fReferenceTable[mppcID-1] = prob;
  };

private:

  float  fX;      ///< x position that this pixel is centered on
  float  fY;      ///< y position that this pixel is centered on
  float  fR;      ///< radius from center for this pixel
  float  fTheta;  ///< angle with respect to sipm 1 (in degrees)
  float  fSize;   ///< size of pixel 
  float  fIntensity;    ///< if reconstructing, this is the reconstructed intensity
  unsigned fID;         ///< id number
  AReferenceTable fReferenceTable; ///< stores mppc to probability map
};
}

#endif

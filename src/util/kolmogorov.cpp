/*! \file kolmogorov.cpp
* COPY: Adapted from TMath library KolmogorovProb function:
* http://root.cern.ch/root/html/src/TMath.cxx.html#RDBIQ
* Authors: Rene Brun, Anna Kreshuk, Eddy Offermann, Fons Rademakers   29/07/95
* Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.
* All rights reserved.
* For the licensing terms see $ROOTSYS/LICENSE.
* For the list of contributors see $ROOTSYS/README/CREDITS.
*/

#include "kolmogorov.hpp"
#include "common/Integer.hpp"

namespace Biips
{
  namespace TMath
  {
    typedef Scalar Double_t;
    typedef Int Int_t;

    inline Double_t Abs(Double_t d)
    { return std::fabs(d); }

    inline Int_t Nint(Double_t x)
    { return roundInteger(x); }

    inline Double_t Exp(Double_t x)
    { return std::exp(x); }

    inline Int_t Max(Int_t a, Int_t b)
    { return std::max(a,b); }

    // COPY: ************ from here ********************
    // Authors: Rene Brun, Anna Kreshuk, Eddy Offermann, Fons Rademakers   29/07/95

    /*************************************************************************
    * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
    * All rights reserved.                                                  *
    *                                                                       *
    * For the licensing terms see $ROOTSYS/LICENSE.                         *
    * For the list of contributors see $ROOTSYS/README/CREDITS.             *
    *************************************************************************/

    Double_t KolmogorovProb(Double_t z)
    {
      // Calculates the Kolmogorov distribution function,
      //Begin_Html
      /*
   <img src="gif/kolmogorov.gif">
       */
      //End_Html
      // which gives the probability that Kolmogorov's test statistic will exceed
      // the value z assuming the null hypothesis. This gives a very powerful
      // test for comparing two one-dimensional distributions.
      // see, for example, Eadie et al, "statistocal Methods in Experimental
      // Physics', pp 269-270).
      //
      // This function returns the confidence level for the null hypothesis, where:
      //   z = dn*sqrt(n), and
      //   dn  is the maximum deviation between a hypothetical distribution
      //       function and an experimental distribution with
      //   n    events
      //
      // NOTE: To compare two experimental distributions with m and n events,
      //       use z = sqrt(m*n/(m+n))*dn
      //
      // Accuracy: The function is far too accurate for any imaginable application.
      //           Probabilities less than 10^-15 are returned as zero.
      //           However, remember that the formula is only valid for "large" n.
      // Theta function inversion formula is used for z <= 1
      //
      // This function was translated by Rene Brun from PROBKL in CERNLIB.

      Double_t fj[4] = {-2,-8,-18,-32}, r[4];
      const Double_t w = 2.50662827;
      // c1 - -pi**2/8, c2 = 9*c1, c3 = 25*c1
      const Double_t c1 = -1.2337005501361697;
      const Double_t c2 = -11.103304951225528;
      const Double_t c3 = -30.842513753404244;

      Double_t u = TMath::Abs(z);
      Double_t p;
      if (u < 0.2) {
        p = 1;
      } else if (u < 0.755) {
        Double_t v = 1./(u*u);
        p = 1 - w*(TMath::Exp(c1*v) + TMath::Exp(c2*v) + TMath::Exp(c3*v))/u;
      } else if (u < 6.8116) {
        r[1] = 0;
        r[2] = 0;
        r[3] = 0;
        Double_t v = u*u;
        Int_t maxj = TMath::Max(1,TMath::Nint(3./u));
        for (Int_t j=0; j<maxj;j++) {
          r[j] = TMath::Exp(fj[j]*v);
        }
        p = 2*(r[0] - r[1] +r[2] - r[3]);
      } else {
        p = 0;
      }
      return p;
    }
    // COPY: ************ to here ********************
  }

  Scalar ksProb(Scalar z)
  {
    return TMath::KolmogorovProb(z);
  }

}

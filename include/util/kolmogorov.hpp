#ifndef BIIPS_KOLMOGOROV_HPP_
#define BIIPS_KOLMOGOROV_HPP_

#include "common/Types.hpp"

namespace Biips
{

  template<typename InputIterator>
  Scalar ksTwoSamplesStat(InputIterator firstX, InputIterator lastX, InputIterator firstY, InputIterator lastY)
  {
    Size n_x = std::distance(firstX, lastX);
    Size n_y = std::distance(firstY, lastY);
    Size n = n_x+n_y;

    std::sort(firstX, lastX);
    std::sort(firstY, lastY);

    Scalar cumul_x = 0;
    Scalar cumul = 0;
    Scalar max = 0.0;
    Scalar val;

    while(firstX != lastX)
    {
      while(firstY != lastY)
      {
        if(*firstX < *firstY)
        {
          cumul = cumul+1;
          cumul_x = cumul_x+1;

          val = std::fabs(cumul_x/n_x - cumul/n);
          if (val>max)
            max = val;

          break;
        }

        cumul = cumul+1;

        val = std::fabs(cumul_x/n_x - cumul/n);
        if (val>max)
          max = val;

        ++firstY;
      }

      if(firstY == lastY)
      {
        cumul = cumul+1;
        cumul_x = cumul_x+1;

        val = std::fabs(cumul_x/n_x - cumul/n);
        if (val>max)
          max = val;
      }

      ++firstX;
    }
    while(firstY != lastY)
    {
      cumul = cumul+1;

      val = std::fabs(1 - cumul/n);
      if (val>max)
        max = val;

      ++firstY;
    }

    return Scalar(n)/n_y * max;
  }

  Scalar ksProb(Scalar z);
}
#endif /* BIIPS_KOLMOGOROV_HPP_ */

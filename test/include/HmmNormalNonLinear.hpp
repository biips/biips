#ifndef BIIPS_HMMNORMALNONLINEAR_HPP_
#define BIIPS_HMMNORMALNONLINEAR_HPP_

#include "ModelTest.hpp"

namespace Biips
{

  class HmmNormalNonLinear: public ModelTest
  {
  protected:
    Bool precFlag_;

    static const String NAME_;

    void initAccumulators(Size nParticles, Size numBins, std::map<String,
        MultiArray::Array> & statsValuesMap);

    virtual void initFilterAccumulators(Size nParticles, Size numBins);
    virtual void initSmoothAccumulators(Size nParticles, Size numBins);

    void accumulate(Size t,
                    std::map<String, MultiArray::Array> & statsValuesMap,
                    const String & title);

    virtual void filterAccumulate(Size iter);
    virtual void smoothAccumulate(Size iter);

  public:
    typedef ModelTest BaseType;

    HmmNormalNonLinear(int argc, char** argv, Size verbose = 1, Size showMode =
        0, Bool precFlag = false, std::ostream & os = std::cout);

    virtual void PrintIntro();

    //    virtual void InputModelParam(std::istream & is = std::cin);

    virtual void BuildModelGraph();

  };

}

#endif /* BIIPS_HMMNORMALNONLINEAR_HPP_ */

//                                               -*- C++ -*-
/*! \file HmmNormalNonLinear.hpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/

#ifndef BIIPS_HMMNORMALNONLINEAR_HPP_
#define BIIPS_HMMNORMALNONLINEAR_HPP_

#include "ModelTest.hpp"

namespace Biips
{

  class HmmNormalNonLinear : public ModelTest
  {
  protected:
    Bool precFlag_;

    static const String NAME_;

    void initAccumulators(std::map<String, DataType::Array> & statsValuesMap);

    virtual void initFilterAccumulators();
    virtual void initSmoothAccumulators();

    void accumulate(Size t, std::map<String, DataType::Array> & statsValuesMap, const String & title);

    virtual void filterAccumulate(Size t);
    virtual void smoothAccumulate(Size t);

  public:
    typedef ModelTest BaseType;

    HmmNormalNonLinear(int argc, char** argv, Size verbose = 1, Size showMode = 0, Bool precFlag = false, std::ostream & os = std::cout);

    virtual void PrintIntro() const;

//    virtual void InputModelParam(std::istream & is = std::cin);

    virtual void BuildModelGraph();

    virtual void PlotResults(const String & plotFileName = "") const;

  };

}

#endif /* BIIPS_HMMNORMALNONLINEAR_HPP_ */

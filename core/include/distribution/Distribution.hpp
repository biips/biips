//                                               -*- C++ -*-
/*! \file Distribution.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DISTRIBUTION_HPP_
#define BIIPS_DISTRIBUTION_HPP_

#include "common/Common.hpp"
#include "common/Table.hpp"

namespace Biips
{

  const String UNDEF_DIST = "undef";

  class Distribution
  {
  protected:
    const String name_;
    const Size nParam_;

    Distribution(const String & name, Size nParam) :
      name_(name), nParam_(nParam)
    {
    }

    virtual Bool
        checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const = 0;
    virtual Bool checkParamValues(const NumArray::Array & paramDims) const = 0;
    virtual DimArray
        dim(const Types<DimArray::Ptr>::Array & paramDims) const = 0;
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const = 0;
    virtual Scalar logDensity(const NumArray & x,
                              const NumArray::Array & paramValues,
                              const NumArray::Pair & boundValues) const = 0;
    virtual void unboundedSupport(ValArray & lower,
                                  ValArray & upper,
                                  const NumArray::Array & paramValues) const = 0;

  public:
    typedef Distribution SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    String const & Name() const
    {
      return name_;
    }
    virtual String Alias() const
    {
      return "";
    }
    Size NParam() const
    {
      return nParam_;
    }

    Bool CheckNParam(Size n) const
    {
      return n == nParam_;
    }

    Bool CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    Bool CheckParamValues(const NumArray::Array & paramValues) const;

    virtual Bool
        CheckDensityParamValues(const NumArray & x,
                                const NumArray::Array & paramValues) const;

    DimArray Dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    void Sample(ValArray & values,
                const NumArray::Array & paramValues,
                const NumArray::Pair & boundValues,
                Rng & rng) const;

    // TODO use PDFType argument to optimize calculations : cf JAGS Distribution.h
    Scalar LogDensity(const NumArray & x,
                      const NumArray::Array & paramValues,
                      const NumArray::Pair & boundValues) const;

    void UnboundedSupport(ValArray & lower,
                          ValArray & upper,
                          const NumArray::Array & paramValues) const;

    // is unbounded support fixed
    virtual Bool IsSupportFixed(const Flags & fixmask) const = 0;

    virtual Bool CanBound() const
    {
      return false;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return false;
    }
    virtual Bool CheckParamDiscrete(const Flags & mask) const
    {
      return true;
    }
    virtual Bool IsLocationParameter(Size i) const
    {
      return false;
    }
    virtual Bool IsScaleParameter(Size i) const
    {
      return false;
    }
    virtual Size DegreeOfFreedom(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return Dim(paramDims).Length();
    }

    virtual ~Distribution()
    {
    }
  };

  typedef Table<Distribution> DistributionTable;
} /* namespace Biips */

#endif /* BIIPS_DISTRIBUTION_HPP_ */

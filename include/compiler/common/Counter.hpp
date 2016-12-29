/*! \file Counter.hpp
 * COPY: Adapted from JAGS Counter class
 */

#ifndef BIIPS_COUNTER_HPP_
#define BIIPS_COUNTER_HPP_

#include "common/IndexRangeIterator.hpp"

namespace Biips
{

  /*!
   * @short Mutable index that traverses a BUGS-language "for" loop
   *
   * The Counter class is used to represent counters in "for" loops in the
   * BUGS language. A Counter is a scalar RangeIterator.
   *
   * <pre>
   * for (i in 1:N) {
   * }
   * </pre>
   */
  class Counter : public IndexRangeIterator
  {
  public:
    typedef IndexRangeIterator BaseType;

    /*!
     * Create a counter
     * @param range Scalar range. i.e., Range having ndim(false) == 1
     */
    Counter(const IndexRange & range);
  };

}

#endif /* BIIPS_COUNTER_HPP_ */

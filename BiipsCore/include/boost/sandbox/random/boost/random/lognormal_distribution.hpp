// Copyright Jens Maurer 2000-2001
// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_LOGNORMAL_DISTRIBUTION_HPP
#define BOOST_RANDOM_LOGNORMAL_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/random/normal_distribution.hpp>

namespace boost {
    // A rewrite of the original lognormal dsitribution. This removes the work
    // done to maintain an adjusted mean and sigma and simply exponentiates
    // values of a normal distribution. This implementation completely removes
    // the artificial constraints on the mean (couldn't be 0). Also, it's a
    // bit cleaner now.
    template<typename RealType = double>
    class lognormal_distribution
    {
        typedef normal_distribution<RealType> base_type;
    public:
        typedef RealType result_type;

        explicit lognormal_distribution(result_type mean = result_type(1),
                                         result_type sigma = result_type(1))
            : m_norm(mean, sigma)
        {
            BOOST_ASSERT(sigma > 0);
        }

        inline result_type mean() const
        { return m_norm.mean(); }

        inline result_type sigma() const
        { return m_norm.sigma(); }

        result_type reset()
        { m_norm.reset(); }

        template<class Engine>
        result_type operator()(Engine& eng)
        {
            using std::exp;

            // The previous implementation scales and translates by some
            // kind of a factor. Why don't we just exponentiate a normal
            // variate with the given mean and sigma.
            //
            // return exp(_norm(eng) * _nsigma + _nmean);

            return exp(m_norm(eng));
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator<<(std::basic_ostream<CharT, Traits>& os, const lognormal_distribution& dist)
        {
            os << dist.m_normal;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT, Traits>&
        operator>>(std::basic_istream<CharT, Traits>& is, lognormal_distribution& dist)
        {
            is >> dist.m_normal;
            return is;
        }
#endif

    private:
        base_type m_norm;
    };

} // namespace boost

#endif // BOOST_RANDOM_LOGNORMAL_DISTRIBUTION_HPP

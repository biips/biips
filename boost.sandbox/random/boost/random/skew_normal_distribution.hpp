// Copyright Edward M. Morrison 2007
// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_SKEW_NORMAL_DISTRIBUTION_HPP
#define BOOST_RANDOM_SKEW_NORMAL_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/random/normal_distribution.hpp>

namespace boost
{
    // Skew normal distribution based on descriptions of Adelchi Azzalini given
    // here. http://azzalini.stat.unipd.it/SN.
    //
    // This distribution is constructed over a standard normal distribution object
    // that provides a pair of random numbers for skewing. The inputs are as
    // follows:
    //  - mean - The center of the distribution.
    //  - sigma - The standard deviation of the distribution.
    //  - alpha - Shape parameter. Negative values skew to the left, positive
    //            to the right. Note that a shape of 0 is just a normal
    //            distribution.
    //
    // Note that the alpha parameter is used to compute a delta value that
    // controls the correlation between two random numbers. This value is
    // cached inside the distribution to avoid redundant computation.
    template <typename RealType = double>
    class skew_normal_distribution
    {
        typedef normal_distribution<RealType> base_type;
    public:
        typedef RealType result_type;

        skew_normal_distribution(const result_type& mean = result_type(0),
                                 const result_type& sigma = result_type(1),
                                 const result_type& alpha = result_type(0))
            : m_mean(mean)
            , m_sigma(sigma)
            , m_alpha(alpha)
            , m_delta(make_delta(alpha))
            , m_norm()
        {
            BOOST_ASSERT(m_sigma >= result_type(0));
            BOOST_ASSERT(m_delta >= result_type(-1) && m_delta <= result_type(1));
        }

        result_type mean() const
        { return m_mean; }

        result_type sigma() const
        { return m_sigma; }

        result_type delta() const
        { return m_delta; }

        result_type alpha() const
        { return m_alpha; }


        void reset()
        { m_norm.reset(); }

        // Note that the engine must generate uniform random numbers in the
        // range of (0, 1).
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            using std::sqrt;

            // Generate a couple of base random numbers.
            result_type u0 = m_norm(eng);
            result_type v = m_norm(eng);

            // Compute u1 such that u0, u1 have a correlation of delta.
            result_type one = result_type(1);
            result_type u1 =  u0 * m_delta + v * sqrt(one - m_delta * m_delta);
            result_type z = (u0 >= 0 ) ? u1 : -u1;

            // Scale and tranlate the result so that it conforms to the mean
            // and standard deviation of the distribution.
            return m_mean + m_sigma * z;
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT,Traits>&
        operator <<(std::basic_ostream<CharT,Traits>& os, const skew_normal_distribution& dist)
        {
            os << dist.m_mean << " " << dist.m_sigma << " "
               << dist.m_delta << " " << dist.m_delta;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT,Traits>&
        operator >>(std::basic_istream<CharT,Traits>& is, skew_normal_distribution& dist)
        {
            is >> std::ws >> dist.m_mean >> std::ws >> dist.m_sigma
               >> std::ws >> dist.m_alpha >> std::ws >> dist.m_delta;
            return is;
        }
#endif

    private:
        result_type make_delta(result_type alpha)
        {
            using std::sqrt;
            result_type one(1);
            return alpha / sqrt(one + alpha * alpha);
        }

    private:
        result_type m_mean;
        result_type m_sigma;
        result_type m_alpha;
        result_type m_delta;
        base_type m_norm;
    };

} // namespace boost

#endif


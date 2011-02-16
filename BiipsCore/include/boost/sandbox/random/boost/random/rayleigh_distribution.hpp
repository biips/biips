// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_RAYLEIGH_DISTRIBUTION_HPP
#define BOOST_RANDOM_RAYLEIGH_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/random/normal_distribution.hpp>

namespace boost {

    // They Rayleigh distribution generates random numbers distributed along
    // a Rayleigh distribution with the parameter sigma. The implentation of
    // this generator is taken from Wikipedia. Specifically R ~ Rayleigh(sigma)
    // if R = sqrt(X^2 + Y^2) where X, Y ~ N(0, sigma^2).
    //
    // Implementation-wise, we actually use two different distribution objects
    // to represent the random vectors upon which the distribution is based.
    // I'm not sure if we actually need two since they're both have the same
    // distribution. Just something to check.
    template <typename ResultType = double>
    class rayleigh_distribution
    {
        typedef normal_distribution<ResultType> base_type;
    public:
        typedef ResultType result_type;

        // Default sigma is 1 - which looks like the maximum of the curve
        // defined by the distribution should also be 1.
        explicit rayleigh_distribution(const result_type& sigma = 1)
            : m_sigma(sigma)
            , m_x(0, sigma)
            , m_y(0, sigma)
        {
            assert(sigma > result_type(0));
        }

        rayleigh_distribution(const rayleigh_distribution& dist)
            : m_sigma(dist.m_sigma)
            , m_x(0, dist.m_sigma * dist.m_sigma)
            , m_y(0, dist.m_sigma * dist.m_sigma)
        { }

        result_type sigma() const
        { return m_sigma; }

        void reset()
        {
            m_x.reset();
            m_y.reset();
        }

        // Note that Engine must generate uniformly distributed random numbers
        // in the range (0, 1).
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            using std::sqrt;

            // For this algorithm, draw a number and square it. We can't use
            // two random values for each x and y since we can forseeably (and
            // it will happen) end up with negative numbers under the root.
            // Using only one random number will prevent that.
            result_type x = m_x(eng);
            result_type y = m_y(eng);
            return sqrt(x * x + y * y);
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT,Traits>&
        operator<<(std::basic_ostream<CharT,Traits>& os, const rayleigh_distribution& wd)
        {
            os << wd._sigma;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT,Traits>&
        operator>>(std::basic_istream<CharT,Traits>& is, rayleigh_distribution& wd)
        {
            is >> std::ws >> wd._sigma;
            return is;
        }
#endif

    private:
        result_type m_sigma;
        base_type m_x;
        base_type m_y;
    };

} // namespace boost

#endif


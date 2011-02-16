// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_WEIBULL_DISTRIBUTION_HPP
#define BOOST_RANDOM_WEIBULL_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>

namespace boost {

    // A Weibull distribution for random number generation. The implementation
    // is based off the description found in Wikipedia with alpha as lambda and
    // beta as k.
    //
    // Note that the weibull distribution as a special case of the exponential
    // distribution s.t. Y ~ Weibull(a, b) if X ~ Exponential(b^-a) and Y = X^-a.
    // This implementation is not built as such.
    //
    // Note that MathWorld uses alpha/beta rather than lambda and k.
    template <typename ResultType = double>
    class weibull_distribution
    {
    public:
        typedef ResultType result_type;

        explicit weibull_distribution(const result_type& alpha = result_type(1),
                                      const result_type& beta = result_type(1))
            : m_alpha(alpha)
            , m_beta(beta)
        {
            BOOST_ASSERT(alpha > result_type(0));
            BOOST_ASSERT(beta > result_type(0));
        }

        result_type alpha() const
        { return m_alpha; }

        result_type beta() const
        { return m_beta; }


        void reset()
        { }

        // Note that Engine must generate uniformly distributed random numbers
        // in the range (0, 1).
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            using std::log;
            using std::pow;

            // Generate a random number in [0, 1) that is _not_ 0. Taking the
            // log of 0 is bad!
            result_type x;
            for(x = eng(); x == 0; x = eng()) ;

            result_type one = result_type(1);
            result_type y = m_alpha * (pow(-log(x), one / m_beta));

            return y;
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT,Traits>&
        operator<<(std::basic_ostream<CharT,Traits>& os, const weibull_distribution& dist)
        {
            os << dist.m_alpha << " " << dist.m_beta;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT,Traits>&
        operator>>(std::basic_istream<CharT,Traits>& is, weibull_distribution& dist)
        {
            is >> std::ws >> dist.m_alpha >> std::ws >> dist.m_beta;
            return is;
        }
#endif

    private:
        result_type m_alpha;
        result_type m_beta;
    };

} // namespace boost

#endif


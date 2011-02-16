// Copyright Jens Maurer 2000-2001
// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_GAMMA_DISTRIBUTION_HPP
#define BOOST_RANDOM_GAMMA_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>

namespace boost {

    // A rewrite of the old gamma distribution, this adds support for the
    // two-parameter version described almost everywhere else (Wikipedia,
    // MathWorld, etc). The first parameter is named alpha (or k), the second
    // is named theta (or beta).
    //
    // The gamma distribution represents the summation of alpha exponentially
    // distributed random variables,each with the parameter theta.
    template<class RealType = double>
    class gamma_distribution
    {
    public:
        typedef RealType result_type;

        explicit gamma_distribution(const result_type& alpha = result_type(1),
                                    const result_type& theta = result_type(1))
            : m_alpha(alpha)
            , m_theta(theta)
        {
            BOOST_ASSERT(m_alpha > result_type(0));
            BOOST_ASSERT(m_theta > result_type(0));
        }

        result_type alpha() const
        { return m_alpha; }

        result_type theta() const
        { return m_theta; }

        void reset()
        { }

        // Note that eng must provide values in the range [0, 1).
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            using std::sqrt;
            using std::pow;
            using std::log;
            using std::exp;
            using std::tan;

            if(m_alpha == result_type(1)) {
                // Pick a number, not 0, and return -log(u) * theta
                result_type u;
                for(u = eng(); u == 0; u = eng()) ;
                return -log(u) * m_theta;
            }
            else if(m_alpha > result_type(1)) {
                // This part of the algorithm appears to come from Cheng'1977,
                // "The generation of Gamma variables with non-integral shape
                // parameters", in Applied Statistices, 26(1), pp. 71-74.
                // At least that's what the Python code sites :)

                // Preliminaries
                result_type a = sqrt(result_type(2) * m_alpha - result_type(1));
                result_type b = m_alpha - log(result_type(4));
                result_type c = m_alpha + a;

                while(1) {
                    // Pick two numbers s.t. 0 != u1 != 1. Note that the Python
                    // implementation constrains these with a precision of 1e-7
                    // away from the values.
                    result_type u1 = eng();
                    if(u1 == 0 || u1 == 1) {
                        continue;
                    }
                    result_type u2 = result_type(1) - eng();

                    // A bunch of computing. I don't know what this does.
                    result_type v = log(u1 / (result_type(1) - u1)) / a;
                    result_type x = m_alpha * exp(v);
                    result_type z = u1 * u1 * u2;
                    result_type r = b + c * v - x;
                    result_type magic = result_type(1) + log(result_type(4.5));

                    if((r + magic - result_type(4.5) * z >= 0.0) ||
                       (r >= log(z)))
                    {
                        return x * m_theta;
                    }
                }
            }
            else {
                // For lack of a constant...
                result_type e = exp(1);

                // For alpha < 0, we use the ALGORITHM GS of Statistical Computing
                // by Kennedy and Gentle. I didn't read this either, but did basically
                // reimplement the Python version.
                result_type x;
                while(1) {
                    result_type u = eng();
                    result_type b = (m_alpha + e) / e;
                    result_type p = b * u;
                    x = p < result_type(1)
                            ? pow(p, result_type(1) / m_alpha)
                            : -log((b - p) / m_alpha);
                    result_type u1 = eng();
                    if(p > result_type(1)) {
                        if(u1 <= pow(x, m_alpha - result_type(1))) {
                            break;
                        }
                    }
                    else if(u1 <= exp(-x)) {
                        break;
                    }
                }
                return x * m_theta;
            }
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator<<(std::basic_ostream<CharT, Traits>& os, const gamma_distribution& dist)
        {
            os << dist.m_alpha << " " << dist.m_theta;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT,Traits>&
        operator>>(std::basic_istream<CharT,Traits>& is, gamma_distribution& dist)
        {
            is >> std::ws >> dist.m_alpha << std::ws >> dist.m_theta;
            return is;
        }
#endif

        result_type m_alpha;
        result_type m_theta;
    };

} // namespace boost

#endif // BOOST_RANDOM_GAMMA_DISTRIBUTION_HPP

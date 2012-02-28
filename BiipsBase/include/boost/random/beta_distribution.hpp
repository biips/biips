// Copyright Jeremy Bruestle 2007
// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_BETA_DISTRIBUTION_HPP
#define BOOST_RANDOM_BETA_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>

namespace boost {

    // The Beta Distribution for random number generation as implemented by
    // Jeremy Bruestle.
    //
    // This implementation is taken from the paper Cheng, "Generating Beta
    // Variates with Nonintegral Shape Parameters", Communications of
    // the ACM, 21(4), 1978.
    template<class RealType = double>
    class beta_distribution
    {
    public:
        typedef RealType input_type;
        typedef RealType result_type;

        beta_distribution(const result_type& a = result_type(1),
                        const result_type& b = result_type(1))
            : _a(a)
            , _b(b)
            , _alpha()
            , _beta()
            , _theta()
        {
            BOOST_ASSERT(a > result_type(0));
            BOOST_ASSERT(b > result_type(0));
            init();
        }

        inline result_type a() const
        { return _a; }

        inline result_type b() const
        { return _b; }

        inline void reset()
        { }

        template<class Engine>
        result_type operator ()(Engine& eng)
        {
            using std::log;
            using std::exp;

            // Try until we get a working case: expected number attempts varies
            // based on a + b worst case number of expected attempts is 4.
            result_type u1, u2, v, w, x;
            do {
                u1 = eng();
                u2 = eng();
                v = _beta * log (u1 / (result_type(1) - u1));
                w = _a * exp(v);
                x = _alpha * log(_alpha / (_b + w)) + _theta * v - log(result_type(4));
            } while(x < log(u1 * u1 * u2));

            return w / (_b + w);
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator<<(std::basic_ostream<CharT, Traits>& os, const beta_distribution& dist)
        {
            os << dist.m_a << dist.m_b;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT, Traits>&
        operator>>(std::basic_istream<CharT,Traits>& is, beta_distribution& dist)
        {
            is >> std::ws >> dist.m_a >> std::ws >> dist.m_b;
            dist.init();
            return is;
        }
#endif

    private:
        void init()
        {
            using std::exp;
            using std::log;
            using std::sqrt;
            using std::max;

            _alpha = _a + _b;
            if (_a <= result_type(1) || _b <= result_type(1)) {
                _beta = max(result_type(1) / _a, result_type(1) / _b);
            }
            else {
                _beta = sqrt((_alpha - result_type(2)) / (result_type(2) * _a *_b - _alpha));
            }
            _theta = _a + result_type(1) / _beta;
        }

        result_type _a;
        result_type _b;
        result_type _alpha;
        result_type _beta;
        result_type _theta;
    };

} // namespace boost

#endif // BOOST_RANDOM_BETA_DISTRIBUTION_HPP

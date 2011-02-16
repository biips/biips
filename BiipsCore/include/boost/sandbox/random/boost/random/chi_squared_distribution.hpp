// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_CHI_SQUARED_DISTRIBUTION_HPP
#define BOOST_RANDOM_CHI_SQUARED_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/random/gamma_distribution.hpp>

namespace boost {

    // The Chi-Squared distribution...
    //
    // This implementation is drawn from the fact that the chi-squared is a
    // special case of the gamma distribution with alpha being the degrees of
    // freedom and theta, 2.
    template<class RealType = double>
    class chi_squared_distribution
    {
        typedef gamma_distribution<RealType> base_type;
    public:
        typedef RealType result_type;

        explicit chi_squared_distribution(const result_type& df = result_type(1))
            : m_gamma(df / result_type(2), result_type(2))
        {
            BOOST_ASSERT(degrees_of_freedom() > 0);
        }

        inline result_type degrees_of_freedom() const
        { return m_gamma.alpha() * result_type(2); }

        void reset()
        { m_gamma.reset(); }

        // Note that eng must provide values in the range [0, 1).
        template<class Engine>
        result_type operator ()(Engine& eng)
        {
            return m_gamma(eng);
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator <<(std::basic_ostream<CharT, Traits>& os, const chi_squared_distribution& dist)
        {
            os << dist.m_gamma;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT, Traits>&
        operator >>(std::basic_istream<CharT,Traits>& is, chi_squared_distribution& dist)
        {
            is >> std::ws >> dist.m_gamma;
            return is;
        }
#endif

    private:

        result_type m_df;
        base_type m_gamma;
    };

} // namespace boost

#endif // BOOST_RANDOM_GAMMA_DISTRIBUTION_HPP

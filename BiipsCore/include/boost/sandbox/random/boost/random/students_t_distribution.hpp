// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_STUDENTS_T_DISTRIBUTION_HPP
#define BOOST_RANDOM_STUDENTS_T_DISTRIBUTION_HPP

#include <cmath>
#include <iostream>

#include <boost/assert.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/chi_squared_distribution.hpp>

namespace boost {

    // The Student's T distribution...
    //
    // The implementation of this distribution is taken from Wikipedia, which
    // describes the ratio of two random variates (a Normal and Chi-Square)
    // being t-distributed over v diegrees of freedom. The normal distribution
    // is the standard normal distributuion, and the Chi-Square distribution
    // has v degrees of freedom.
    template<class RealType = double>
    class students_t_distribution
    {
        typedef normal_distribution<RealType> normal_type;
        typedef chi_squared_distribution<RealType> chi_square_type;
    public:
        typedef RealType result_type;

        explicit students_t_distribution(const result_type& df = result_type(1))
            : m_norm()
            , m_chi(df)
        {
            BOOST_ASSERT(df > 0);
        }

        inline result_type degrees_of_freedom() const
        { return m_chi.degrees_of_freedom(); }

        void reset()
        {
            m_norm.reset();
            m_chi.reset();
        }

        // Note that eng must provide values in the range (0, 1].
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            return m_norm(eng) / sqrt(m_chi(eng) / degrees_of_freedom());
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator <<(std::basic_ostream<CharT, Traits>& os, const students_t_distribution& dist)
        {
            os << dist.m_norm << " " << dist.m_chi;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT,Traits>&
        operator >>(std::basic_istream<CharT,Traits>& is, students_t_distribution& dist)
        {
            is >> std::ws >> dist.m_norm << std::ws >> dist.m_chi;
            return is;
        }
#endif

        normal_type m_norm;
        chi_square_type m_chi;
    };

} // namespace boost

#endif // BOOST_RANDOM_GAMMA_DISTRIBUTION_HPP

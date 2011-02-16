// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_F_DISTRIBUTION_HPP
#define BOOST_RANDOM_F_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/random/chi_squared_distribution.hpp>

namespace boost {

    // The F distribution (a.k.a., Snedecor's F or Fisher-Snedecor) represents
    // the ration of two chi-squared variates.
    template<class RealType = double>
    class fisher_f_distribution
    {
        typedef chi_squared_distribution<RealType> base_type;

    public:
        typedef RealType result_type;

        explicit fisher_f_distribution(const result_type& d1 = result_type(1),
                                       const result_type& d2 = result_type(1))
            : m_first(d1)
            , m_second(d2)
        {
            BOOST_ASSERT(first_degrees_of_freedom() > 0);
            BOOST_ASSERT(second_degrees_of_freedom() > 0);
        }

        inline result_type first_degrees_of_freedom() const
        { return m_first.degrees_of_freedom(); }

        inline result_type second_degrees_of_freedom() const
        { return m_second.degrees_of_freedom(); }

        void reset()
        {
            m_first.reset();
            m_second.reset();
        }

        // Note that eng must provide values in the range (0, 1].
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            result_type top = m_first(eng) / first_degrees_of_freedom();
            result_type bot = m_second(eng) / second_degrees_of_freedom();
            return top / bot;
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator <<(std::basic_ostream<CharT, Traits>& os, const fisher_f_distribution& dist)
        {
            os << dist.m_first << " " << dist.m_second;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT,Traits>&
        operator >>(std::basic_istream<CharT,Traits>& is, fisher_f_distribution& dist)
        {
            is >> std::ws >> dist.m_first << std::ws >> dist.m_second;
            return is;
        }
#endif

        base_type m_first;
        base_type m_second;
    };

} // namespace boost

#endif // BOOST_RANDOM_GAMMA_DISTRIBUTION_HPP

// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_EXTREME_VALUE_I_DISTRIBUTION_HPP
#define BOOST_RANDOM_EXTREME_VALUE_I_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>

namespace boost {

    // The Gumbel distribution is one of the extreme value (I) distributions
    // along with Weibull and Frechet. This is also known as the Fisher-Tippet
    // distribution or the log-Weibull.
    //
    // This distribution actually implements a class of several different
    // distributions. The extreme value distribution deals with the median
    // of different distributions. There are three distinct flavors of extreme
    // value distributions (I, II, and III). For type I distributions, there
    // are two cases: minimum and maximum. Generally when authors refer to the
    // type I distribution, they refer to the maximum case. The different cases
    // only differ by a factor of -1 in most cases.
    //
    // Note that the type II and III are closely related to the minimum and
    // maximum cases of the type I.
    template<class RealType = double>
    class extreme_value_i_distribution
    {
    public:
        typedef RealType result_type;

        explicit extreme_value_i_distribution(const result_type& mu = result_type(0),
                                              const result_type& beta = result_type(1),
                                              const result_type& sign = result_type(1))
            : m_mu(mu)
            , m_beta(beta)
            , m_sign(sign)
        {
            BOOST_ASSERT(m_beta > result_type(0));
            BOOST_ASSERT(m_sign == -1 || m_sign == 1);
        }

        result_type mu() const
        { return m_mu; }

        result_type beta() const
        { return m_beta; }

        result_type sign() const
        { return m_sign; }

        void reset()
        { }

        // Note that eng must provide values in the range (0, 1].
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            using std::log;
            return m_mu - m_sign * m_beta * log(-log(eng()));
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator <<(std::basic_ostream<CharT, Traits>& os, const extreme_value_i_distribution& dist)
        {
            os << dist.m_mu << " " << dist.m_beta << " " << dist.m_sign;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT,Traits>&
        operator >>(std::basic_istream<CharT,Traits>& is, extreme_value_i_distribution& dist)
        {
            is >> std::ws >> dist.m_mu >> std::ws >> dist.m_beta >> std::ws >> dist.m_sign;
            return is;
        }
#endif

        result_type m_mu;
        result_type m_beta;
        result_type m_sign;
    };

} // namespace boost

#endif // BOOST_RANDOM_GAMMA_DISTRIBUTION_HPP

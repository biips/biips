// Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_PARETO_DISTRIBUTION_HPP
#define BOOST_RANDOM_PARETO_DISTRIBUTION_HPP

#include <iostream>
#include <cmath>

#include <boost/assert.hpp>

namespace boost {

    // The Pareto distribution can be used to Pareto-distributed random numbers.
    // This is basically the set of functions similar to 1/x. The Pareto
    // distribution is given in terms of a location, xi and a shape, alpha. The
    // implementation of this class comes from information gleaned from
    // Wikipedia. Note that MathWorld uses k as the location, whereas Wikipedia
    // uses k as the shape. We prefer to use k to denote location.
    //
    // Note that there is also a generalized Pareto distribution. We should
    // probably implement this in terms of that, but we don't need to. Moreover,
    // this matches the Pareto distribution given in the Math toolkit.
    template <typename ResultType = double>
    class pareto_distribution
    {
    public:
        typedef ResultType result_type;

        explicit pareto_distribution(const result_type& location = result_type(1),
                                     const result_type& shape = result_type(1))
            : m_location(location)
            , m_shape(shape)
        {
            BOOST_ASSERT(m_location > 0);
            BOOST_ASSERT(m_shape > 0);
        }

        // Returns the minimum value of the distribution.
        result_type location() const
        { return m_location; }

        // Returns the shape of the distribution.
        result_type shape() const
        { return m_shape; }

        void reset()
        { }

        // Note that Engine must generate uniformly distributed random numbers
        // in the range [0, 1).
        template<class Engine>
        result_type operator()(Engine& eng)
        {
            using std::pow;

            // This is actually taken from the Wikipedia section on sampling
            // rather than the generalized Pareto generator.
            return m_location / pow(eng(), result_type(1) / m_shape);
        }

#if !defined(BOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
        template<class CharT, class Traits>
        friend std::basic_ostream<CharT,Traits>&
        operator<<(std::basic_ostream<CharT,Traits>& os, const pareto_distribution& dist)
        {
            os << dist.m_location << " " << dist.m_shape;
            return os;
        }

        template<class CharT, class Traits>
        friend std::basic_istream<CharT, Traits>&
        operator>>(std::basic_istream<CharT, Traits>& is, pareto_distribution& dist)
        {
            is >> std::ws >> dist.m_location >> std::ws >> dist.m_shape;
            return is;
        }
#endif

    private:
        result_type m_location;
        result_type m_shape;
    };

} // namespace boost

#endif


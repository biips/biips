//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file  cholesky.hpp
 *  \brief Cholesky factorizations in the spirit of boost lu.hpp
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from lists.boost.org uBlas mailing list
 */

#ifndef BOOST_UBLAS_CHOLESKY_HPP_
#define BOOST_UBLAS_CHOLESKY_HPP_

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

namespace boost
{
  namespace numeric
  {
    namespace ublas
    {

      // COPY: Copied from Boost.uBlas mailing list
      // http://lists.boost.org/MailArchives/ublas/2005/07/0568.php

      //! Inplace Cholesky factorization
      template<class M>
      bool cholesky_factorize(M &m)
      {
        typedef M matrix_type;
        typedef typename M::size_type size_type;
        typedef typename M::value_type value_type;

        BOOST_UBLAS_CHECK (m.size1() == m.size2(), external_logic("Cholesky decomposition is only valid for a square, positive definite matrix."));

        size_type size = m.size1();
        vector<value_type> d(size);

        for (size_type i = 0; i < size; ++i)
        {
          matrix_row<M> mri(row(m, i));
          for (size_type j = i; j < size; ++j)
          {
            matrix_row<M> mrj(row(m, j));

            value_type elem = m(i, j) - inner_prod(project(mri, range(0, i)),
                                                   project(mrj, range(0, i)));

            if (i == j)
            {
              if (elem <= 0.0)
              {
                // matrix after rounding errors is not positive definite
                return false;
              }
              else
              {
                d(i) = type_traits<value_type>::type_sqrt(elem);
              }
            }
            else
            {
              m(j, i) = elem / d(i);
            }
          }
        }
        // put the diagonal back in
        for (size_type i = 0; i < size; ++i)
        {
          m(i, i) = d(i);
        }
        // decomposition succeeded
        return true;
      }

      // Cholesky substitution
      template<class M, class E>
      void cholesky_substitute(const M &m, vector_expression<E> &e)
      {
        typedef const M const_matrix_type;
        typedef vector<typename E::value_type> vector_type;

        inplace_solve(m, e, lower_tag());
        inplace_solve(trans(m), e, upper_tag());
      }
      template<class M, class E>
      void cholesky_substitute(const M &m, matrix_expression<E> &e)
      {
        typedef const M const_matrix_type;
        typedef matrix<typename E::value_type> matrix_type;

        inplace_solve(m, e, lower_tag());
        inplace_solve(trans(m), e, upper_tag());
      }
      template<class E, class M>
      void cholesky_substitute_left(vector_expression<E> &e, const M &m)
      {
        typedef const M const_matrix_type;
        typedef vector<typename E::value_type> vector_type;

        inplace_solve(trans(m), e, upper_tag());
        inplace_solve(m, e, lower_tag());
      }
      template<class E, class M>
      void cholesky_substitute_left(matrix_expression<E> &e, const M &m)
      {
        typedef const M const_matrix_type;
        typedef matrix<typename E::value_type> matrix_type;

        inplace_solve(trans(m), e, upper_tag());
        inplace_solve(m, e, lower_tag());
      }

      //! Cholesky matrix inversion
      /*!
       * Inplace inverts a positive definite matrix given its cholesky factorization
       * @param m cholesky factorization of a positive definite matrix
       */
      template<class M>
      void cholesky_invert(M &m)
      {
        typedef typename M::size_type size_type;
        typedef typename M::value_type value_type;

        size_type size = m.size1();

        // determine the inverse of the lower triangular matrix
        for (size_type i = 0; i < size; ++i)
        {
          m(i, i) = 1 / m(i, i);

          for (size_type j = i + 1; j < size; ++j)
          {
            value_type elem(0);

            for (size_type k = i; k < j; ++k)
            {
              elem -= m(j, k) * m(k, i);
            }
            m(j, i) = elem / m(j, j);
          }
        }

        // multiply the upper and lower inverses together
        m = prod(trans(triangular_adaptor<M, lower> (m)), triangular_adaptor<M,
            lower> (m));
      }
      // ENDCOPY


      //! Cholesky matrix determinant
      /*!
       * Computes the determinant of a positive definite matrix given its cholesky factorization
       * @param m cholesky factorization of a positive definite matrix
       * @return the determinant value
       */
      template<class M>
      typename M::value_type cholesky_det(const M & m)
      {
        typedef typename M::size_type size_type;
        typedef typename M::value_type value_type;

        size_type size = m.size1();
        // TODO check mat
        value_type det = 1.0;
        for (size_type i = 0; i < size; ++i)
          det *= m(i, i);
        return det;
      }

    }
  }
}

#endif /* BOOST_UBLAS_CHOLESKY_HPP_ */

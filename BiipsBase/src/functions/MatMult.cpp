//                                               -*- C++ -*-
/*! \file MatMult.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

//#include <boost/numeric/ublas/operation.hpp>

#include "functions/MatMult.hpp"


namespace Biips
{

    DataType MatMult::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & left = paramValues[0];
      const DataType & right = paramValues[1];

      DataType ans;
      if (left.IsMatrix() && right.IsVector())
      {
        Vector ans_vec = ublas::prod(Matrix(left), Vector(right));
        ans = DataType(ans_vec);
      }
      else if (left.IsVector() && right.IsMatrix())
      {
        Vector ans_vec = ublas::prod(Vector(left), Matrix(right));
        ans = DataType(ans_vec);
      }
      else if (left.IsMatrix() && right.IsMatrix())
      {
        Matrix ans_mat = ublas::prod(Matrix(left), Matrix(right));
        ans = DataType(ans_mat);
      }
      // TODO throw exception

      return ans;
    }


}

#ifndef RFUNCTION_H_
#define RFUNCTION_H_

#include <function/Function.hpp>
#include <Rcpp.h>


namespace Biips
{

    class RFunction: public Function
    {
        protected:
        const Rcpp::Function fun_dim_;
        const Rcpp::Function fun_eval_;
        const Rcpp::Function fun_check_param_;
        const Rcpp::Function fun_is_discrete_;

        virtual Bool checkParamDims(
            const Types<DimArray::Ptr>::Array & paramDims) const
        {
          return true;
        }
        virtual DimArray

        dim(const Types<DimArray::Ptr>::Array & paramDims) const;

        virtual void eval(ValArray & values, const NumArray::Array & paramValues) const;

        public:
        typedef RFunction SelfType;
        typedef Function BaseType;

        RFunction(const String & name, Size nParam, 
                  const Rcpp::Function & fun_dim,
                  const Rcpp::Function & fun_eval, 
                  const Rcpp::Function & fun_check_param,
                  const Rcpp::Function & fun_is_discrete)
            : BaseType(name, nParam), 
              fun_dim_(fun_dim), 
              fun_eval_(fun_eval),
              fun_check_param_(fun_check_param), 
              fun_is_discrete_(fun_is_discrete)
        {
          // TODO: check name is valid
          // TODO: check functions nb of arguments
        }

        virtual Bool IsDiscreteValued(const Flags & mask) const;

        virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;

        virtual ~RFunction() {}

    };

}

#endif /* RFUNCTION_H_ */

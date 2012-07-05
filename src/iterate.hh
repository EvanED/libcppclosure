// NO inclusion guards. This should be called as part of
// Boost::preprocessor's iteration.

#ifndef BOOST_PP_IS_ITERATING
#error "iterate.hh should only be included through Boost preprocessor"
#endif

#ifdef n
#error "What kind of dick defined the macro \'n\'?"
#endif

#define n BOOST_PP_ITERATION()

namespace ffi_function {
    
    template<typename FunctionType>
    std::vector<ffi_type *>
    get_arg_types() {
        typedef typename boost::function_traits<FunctionType>::arg1_type TyArg1;
        typedef typename boost::function_traits<FunctionType>::arg2_type TyArg2;
        std::vector<ffi_type *> ret;
        ret.push_back(get_ffi_type<TyArg1>::value);
        ret.push_back(get_ffi_type<TyArg2>::value);
        return ret;
    }

}


#undef n

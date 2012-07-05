// NO inclusion guards. This should be called as part of
// Boost::preprocessor's iteration.

#ifndef BOOST_PP_IS_ITERATING
#error "iterate.hh should only be included through Boost preprocessor"
#endif

#ifndef BOOST_PP_ENUM_PARAMS
#error "Include <boost/preprocessor/repetition.hpp> from the place you iterate this file"
#endif

#ifndef BOOST_PP_COMMA_IF
#error "Include <boost/preprocessor/punctuation/comma_if.hpp> from the place you iterate this file"
#endif

#ifndef BOOST_PP_LOCAL_ITERATE
#error "Include <boost/preprocessor/iteration/local.hpp> from the place you iterate this file"
#endif

#ifndef BOOST_PP_SUB
#error "Include <boost/preprocessor/arithmetic/sub.hpp> from the place you iterate this file"
#endif

#ifdef n
#error "What kind of dick defined the macro \'n\'?"
#endif

#define n BOOST_PP_ITERATION()

namespace ffi_function {
    
    template<
        typename ReturnType
        BOOST_PP_COMMA_IF(n)  // There may not be arguments, after all
        BOOST_PP_ENUM_PARAMS(n, typename TyArg)
        >
    std::vector<ffi_type *>
    get_arg_types(std::function<ReturnType (BOOST_PP_ENUM_PARAMS(n, TyArg))> const & func)
    {
        std::vector<ffi_type *> ret;
# if n > 0
#       define BOOST_PP_LOCAL_MACRO(n) ret.push_back(get_ffi_type<BOOST_PP_CAT(TyArg, n)>::value);
#       define BOOST_PP_LOCAL_LIMITS (0, BOOST_PP_SUB(n,1))
#       include BOOST_PP_LOCAL_ITERATE()
# endif
        return ret;
    }

    template<typename StdFunctionType> struct binder_struct;

    template<
        typename DeclaredReturnTy
        BOOST_PP_COMMA_IF(n)  // There may not be arguments, after all
        BOOST_PP_ENUM_PARAMS(n, typename DeclaredTyArg)
        >
    struct binder_struct<std::function<DeclaredReturnTy (BOOST_PP_ENUM_PARAMS(n, DeclaredTyArg))>>
    {
        static void go(ffi_cif * cif __attribute__((unused)),
                       void * ret,
                       void * args[], void * funcptr)
        {
            typedef std::function<DeclaredReturnTy (BOOST_PP_ENUM_PARAMS(n, DeclaredTyArg))> FunctionType;
            
            typedef typename ReferenceToPointer<DeclaredTyArg0>::type PhysicalTyArg0;
            typedef typename ReferenceToPointer<DeclaredTyArg1>::type PhysicalTyArg1;
            typedef typename ReferenceToPointer<DeclaredReturnTy>::type PhysicalReturnType;
  
            PhysicalTyArg0 * arg0 = (PhysicalTyArg0 *)args[0];
            PhysicalTyArg1 * arg1 = (PhysicalTyArg1 *)args[1];

            FunctionType * func = static_cast<FunctionType*>(funcptr);

            PhysicalReturnType * ret2 = (PhysicalReturnType *)(ret);
            
            store_return<DeclaredReturnTy>(ret2,
                                           (*func)(FormActual<DeclaredTyArg0>::form_actual(*arg0),
                                                   FormActual<DeclaredTyArg1>::form_actual(*arg1)));
        }
    };


}


#undef n

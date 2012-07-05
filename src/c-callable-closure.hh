#include "get_type.hh"

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition.hpp>

#define BOOST_PP_ITERATION_LIMITS (0,10)
#define BOOST_PP_FILENAME_1 "iterate.hh"
#include BOOST_PP_ITERATE()

int zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz;

namespace ffi_function {

template<typename Ty>
struct ReferenceToPointer {
  typedef Ty type;
};

template<typename Ty>
struct ReferenceToPointer<Ty&> {
  typedef Ty * type;
};


template<typename Ty>
struct FormActual {
  static Ty & form_actual(Ty & obj) {
    return obj;
  }
};

template<typename Ty>
struct FormActual<Ty &> {
  static Ty & form_actual(Ty * obj) {
    return *obj;
  }
};

///
/// Just stores the return value in ret_val at ret_addr
template<typename DeclaredReturnTy,
	 typename PhysicalReturnTy,
	 typename ActualReturnTy>
typename std::enable_if<!std::is_reference<DeclaredReturnTy>::value>::type
store_return(PhysicalReturnTy * ret_addr,
	     ActualReturnTy && ret_val)
{
  *ret_addr = ret_val;
}

/// This one is called if the declared return type is a
/// reference. Then we have something like 'int& foo()', 'ret_addr' is
/// actually an int**, but 'ret_val' IS STILL AN 'int&'! (This is
/// because it's straight out of the actual function that was called.)
/// So we need to store '&ret_val' at 'ret_addr' instead.
template<typename DeclaredReturnTy,
	 typename PhysicalReturnTy,
	 typename ActualReturnTy>
typename std::enable_if<std::is_reference<DeclaredReturnTy>::value>::type
store_return(PhysicalReturnTy * ret_addr,
	     ActualReturnTy && ret_val)
{
  *ret_addr = &ret_val;
}


template<typename FunctionType>
void binder(ffi_cif * cif __attribute__((unused)),
	    void * ret,
	    void * args[], void * funcptr)
{
  typedef typename FunctionType::first_argument_type DeclaredTyArg1;
  typedef typename FunctionType::second_argument_type DeclaredTyArg2;
  typedef typename FunctionType::result_type DeclaredReturnTy;
  
  typedef typename ReferenceToPointer<DeclaredTyArg1>::type PhysicalTyArg1;
  typedef typename ReferenceToPointer<DeclaredTyArg2>::type PhysicalTyArg2;
  typedef typename ReferenceToPointer<DeclaredReturnTy>::type PhysicalReturnType;
  
  PhysicalTyArg1 * arg1 = (PhysicalTyArg1 *)args[0];
  PhysicalTyArg2 * arg2 = (PhysicalTyArg2 *)args[1];

  FunctionType * func = static_cast<FunctionType*>(funcptr);

  PhysicalReturnType * ret2 = (PhysicalReturnType *)(ret);

  store_return<DeclaredReturnTy>(ret2,
				 (*func)(FormActual<DeclaredTyArg1>::form_actual(*arg1),
					 FormActual<DeclaredTyArg2>::form_actual(*arg2)));
}


template<typename FunctionType>
class CCallableClosure {
  FunctionType * c_function_pointer_;
  std::vector<ffi_type *> argument_types_;
  ffi_closure * closure_descriptor_;
  ffi_cif interface_;
  std::shared_ptr<std::function<FunctionType>> functor_ptr_;
    
public:
  CCallableClosure(std::function<FunctionType> const & functor)
    : argument_types_(ffi_function::get_arg_types<FunctionType>())
    , functor_ptr_(new std::function<FunctionType>(functor))
  {
    closure_descriptor_ = static_cast<ffi_closure*>
      (ffi_closure_alloc(sizeof(ffi_closure), 
                         reinterpret_cast<void**>(&c_function_pointer_)));
    
    if (closure_descriptor_) {
      if (ffi_prep_cif(&interface_, FFI_DEFAULT_ABI, argument_types_.size(),
                       ffi_function::get_ffi_type<typename boost::function_traits<FunctionType>::result_type>::value,
                       &argument_types_[0])
          == FFI_OK)
      {
        if (ffi_prep_closure_loc(closure_descriptor_, &interface_, 
                                 &binder<typename std::remove_reference<decltype(*functor_ptr_)>::type>,
                                 static_cast<void*>(functor_ptr_.get()),
                                 reinterpret_cast<void*>(c_function_pointer_))
            == FFI_OK)
        {
          return;
        }
      }
    }
    c_function_pointer_ = nullptr;
  }

  FunctionType *
  get_func_ptr() const {
    return c_function_pointer_;
  }

  ~CCallableClosure() {
    ffi_closure_free(closure_descriptor_);
  }
};

}

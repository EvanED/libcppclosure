#include <functional>
#include <memory>
#include <typeinfo>
#include <iostream>
#include <stdio.h>
#include <ffi.h>

#include "src/get_type.hh"

#include <boost/type_traits/function_traits.hpp>

typedef void (*binder_t)(ffi_cif *, void *, void **, void *);

class C {
  const char * x;
public:
  C() { x = "orig\n"; }
  C(C const & c) { x = "copy\n"; }
  char const * str() { return x; }
};

typedef int (*func_t)(C & s, FILE * stream);

double my_fputs(C & c, FILE * stream)
{
  fputs(c.str(), stream);
  return 17.7;
}

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


template<typename FunctionType>
void binder(ffi_cif * cif, void * ret,
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

  std::cout << "Return type: " <<typeid(PhysicalReturnType).name() << "\n";

  *ret2 = (*func)(FormActual<DeclaredTyArg1>::form_actual(*arg1),
                  FormActual<DeclaredTyArg2>::form_actual(*arg2));

  std::cout << "return was " << *ret2 << "\n";
}


typedef int (*fputs_t)(C&, FILE*);


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


int main()
{
  std::function<double (C&, FILE*)> my_fputs_wrapper = my_fputs;
  CCallableClosure<double (C&, FILE*)> bound_puts(my_fputs_wrapper);

  C c;
  std::cout << bound_puts.get_func_ptr()(c, stdout) << "\n";

  

  //ffi_closure_free(closure);
  return 0;
}

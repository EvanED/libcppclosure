#include <functional>
#include <memory>
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

int my_fputs(C & c, FILE * stream)
{
  return fputs(c.str(), stream);
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
  
  typedef typename ReferenceToPointer<DeclaredTyArg1>::type PhysicalTyArg1;
  typedef typename ReferenceToPointer<DeclaredTyArg2>::type PhysicalTyArg2;
  
  PhysicalTyArg1 * arg1 = (PhysicalTyArg1 *)args[0];
  PhysicalTyArg2 * arg2 = (PhysicalTyArg2 *)args[1];

  FunctionType * func = static_cast<FunctionType*>(funcptr);

  unsigned int * ret2 = static_cast<unsigned*>(ret);

  *ret2 = (*func)(FormActual<DeclaredTyArg1>::form_actual(*arg1),
                  FormActual<DeclaredTyArg2>::form_actual(*arg2));
}


typedef int (*fputs_t)(C&, FILE*);


class CCallableClosure {
  fputs_t bound_puts;
  std::vector<ffi_type *> args;
  ffi_closure * closure;
public:
  CCallableClosure(std::shared_ptr<std::function<int (C&, FILE*)>> const & my_fputs_wrapper)
    : args(ffi_function::get_arg_types<C&, FILE*>())
  {
    ffi_cif cif;

    closure = static_cast<ffi_closure*>
      (ffi_closure_alloc(sizeof(ffi_closure), 
                         reinterpret_cast<void**>(&bound_puts)));
    
    if (closure) {
      if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, args.size(),
                       &ffi_type_uint, &args[0])
          == FFI_OK)
      {
        if (ffi_prep_closure_loc(closure, &cif, 
                                 &binder<std::remove_reference<decltype(*my_fputs_wrapper)>::type>,
                                 static_cast<void*>(my_fputs_wrapper.get()),
                                 reinterpret_cast<void*>(bound_puts))
            == FFI_OK)
        {
          return;
        }
      }
    }
    bound_puts = nullptr;
  }

  fputs_t
  get_func_ptr() const {
    return bound_puts;
  }

  ~CCallableClosure() {
    ffi_closure_free(closure);
  }
};


int main()
{
  std::shared_ptr<std::function<int (C&, FILE*)>>
    my_fputs_wrapper(new std::function<int (C&, FILE*)>(my_fputs));
  CCallableClosure bound_puts(my_fputs_wrapper);
  int rc;

  C c;
  rc = bound_puts.get_func_ptr()(c, stdout);

  //ffi_closure_free(closure);
  return 0;
}

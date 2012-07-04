#include <stdio.h>
#include <ffi.h>

#include "src/get_type.hh"

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


template<typename DeclaredTyArg1, typename DeclaredTyArg2>
void binder(ffi_cif * cif, void * ret,
	    void * args[], void * stream)
{
  typedef typename ReferenceToPointer<DeclaredTyArg1>::type PhysicalTyArg1;
  typedef typename ReferenceToPointer<DeclaredTyArg2>::type PhysicalTyArg2;
  
  PhysicalTyArg1 * arg1 = (PhysicalTyArg1 *)args[0];
  PhysicalTyArg2 * arg2 = (PhysicalTyArg2 *)args[1];

  (void) stream;

  unsigned int * ret2 = static_cast<unsigned*>(ret);

  *ret2 = my_fputs(FormActual<DeclaredTyArg1>::form_actual(*arg1),
                   FormActual<DeclaredTyArg2>::form_actual(*arg2));
}


int main()
{
  ffi_cif cif;
  std::vector<ffi_type *> args = ffi_function::get_arg_types<C&, FILE*>();
  ffi_closure * closure;

  int (*bound_puts)(C &, FILE*);
  int rc;
  
  closure = static_cast<ffi_closure*>
    (ffi_closure_alloc(sizeof(ffi_closure), 
		       reinterpret_cast<void**>(&bound_puts)));

  if (closure) {
    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, args.size(),
		     &ffi_type_uint, &args[0])
	== FFI_OK)
    {
      if (ffi_prep_closure_loc(closure, &cif, 
			       &binder<C&, FILE*>,
			       static_cast<void*>(stdout),
			       reinterpret_cast<void*>(bound_puts))
	  == FFI_OK)
      {
	C c;
	rc = bound_puts(c, stdout);
      }
    }
  }

  ffi_closure_free(closure);
  return 0;
}

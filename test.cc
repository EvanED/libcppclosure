#include <stdio.h>
#include <ffi.h>

typedef void (*binder_t)(ffi_cif *, void *, void **, void *);

class C {
  const char * x;
public:
  C() { x = "orig\n"; }
  C(C const & c) { x = "copy\n"; }
  char const * str() { return x; }
};

typedef int (*func_t)(C * s, FILE * stream);

int my_fputs(C * c, FILE * stream)
{
  return fputs(c->str(), stream);
}


void puts_binding(ffi_cif * cif, void * ret,
		  void * args[], void * stream)
{
  func_t* func = (func_t*)args[0];
  C ** s = (C**)args[1];
  FILE * stream2 = static_cast<FILE*>(stream);
  unsigned int * ret2 = static_cast<unsigned*>(ret);

  *ret2 = (*func)(*s, stream2);
}


int main()
{
  ffi_cif cif;
  const int nargs = 2;
  ffi_type * args[nargs];
  ffi_closure * closure;

  int (*bound_puts)(func_t, C *);
  int rc;
  
  closure = static_cast<ffi_closure*>
    (ffi_closure_alloc(sizeof(ffi_closure), 
		       reinterpret_cast<void**>(&bound_puts)));

  if (closure) {
    args[0] = &ffi_type_pointer;
    args[1] = &ffi_type_pointer;

    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, &ffi_type_uint, args)
	== FFI_OK)
    {
      if (ffi_prep_closure_loc(closure, &cif, 
			       puts_binding,
			       static_cast<void*>(stdout),
			       reinterpret_cast<void*>(bound_puts))
	  == FFI_OK)
      {
	C c;
	rc = bound_puts(my_fputs, &c);
      }
    }
  }

  ffi_closure_free(closure);
  return 0;
}

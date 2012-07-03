#include <stdio.h>
#include <ffi.h>

typedef int (*func_t)(char const * s, FILE * stream);

void puts_binding(ffi_cif * cif, unsigned int * ret,
		  void * args[], FILE * stream)
{
  func_t* func = (func_t*)args[0];
  char ** s = (char**)args[1];

  *ret = (*func)(*s, stream);
}


int main()
{
  ffi_cif cif;
  const int nargs = 2;
  ffi_type * args[nargs];
  ffi_closure * closure;

  int (*bound_puts)(func_t, char *);
  int rc;
  
  closure = ffi_closure_alloc(sizeof(ffi_closure), &bound_puts);

  if (closure) {
    args[0] = &ffi_type_pointer;
    args[1] = &ffi_type_pointer;

    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, &ffi_type_uint, args)
	== FFI_OK)
    {
      if (ffi_prep_closure_loc(closure, &cif, puts_binding,
			       stdout, bound_puts)
	  == FFI_OK)
      {
	rc = bound_puts(fputs, "Hello world!\n");
      }
    }
  }

  ffi_closure_free(closure);
  return 0;
}

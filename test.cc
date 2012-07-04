#include <functional>
#include <memory>
#include <typeinfo>
#include <iostream>
#include <stdio.h>
#include <ffi.h>

#include "src/c-callable-closure.hh"

class C {
  const char * x;
public:
  C() { x = "orig\n"; }
  C(C const & c) { (void)c; x = "copy\n"; }
  char const * str() { return x; }
};

double my_fputs(C & c, FILE * stream)
{
  fputs(c.str(), stream);
  return 17.7;
}

int main()
{
  std::function<double (C&, FILE*)> my_fputs_wrapper = my_fputs;
  ffi_function::CCallableClosure<double (C&, FILE*)> bound_puts(my_fputs_wrapper);

  C c;
  std::cout << bound_puts.get_func_ptr()(c, stdout) << "\n";

  

  //ffi_closure_free(closure);
  return 0;
}

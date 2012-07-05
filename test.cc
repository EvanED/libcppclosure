#include <functional>
#include <memory>
#include <typeinfo>
#include <iostream>
#include <stdio.h>
#include <ffi.h>

#include "src/c-callable-closure.hh"

double my_fputs(std::string const & str, double d)
{
  puts(str.c_str());
  return d;
}

int g;

int & get_g(int x, int y) {
  (void) x, (void) y;
  return g;
}

int main()
{
  std::function<double (std::string const &, double)> my_fputs_wrapper = my_fputs;
  ffi_function::CCallableClosure<double (std::string const &, double)>
    bound_puts(my_fputs_wrapper);

  std::cout << bound_puts.get_func_ptr()("Hello world!", 17.5) << "\n";

#if 0
  std::function<int & (int, int)> return_ref_test = get_g;
  ffi_function::CCallableClosure<int & (int, int)>
    return_ref_test2(return_ref_test);

  std::cout << &(return_ref_test2.get_func_ptr()(1,1)) << "\n";
  std::cout << &g << "\n";
#endif

  return 0;
}

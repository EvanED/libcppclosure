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

int main()
{
  std::function<double (std::string const &, double)> my_fputs_wrapper = my_fputs;
  ffi_function::CCallableClosure<double (std::string const &, double)>
    bound_puts(my_fputs_wrapper);

  std::cout << bound_puts.get_func_ptr()("Hello world!", 17.5) << "\n";

  return 0;
}

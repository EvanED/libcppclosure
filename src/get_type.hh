#include <vector>

#include <ffi.h>

namespace ffi_function
{
    template<typename Ty>
    struct get_ffi_type;

    template<>
    struct get_ffi_type<void> {
	static ffi_type * value;
    };

    template<>
    struct get_ffi_type<char> {
	static ffi_type * value;
    };
    template<>
    struct get_ffi_type<unsigned char> {
	static ffi_type * value;
    };
    template<>
    struct get_ffi_type<signed char> {
	static ffi_type * value;
    };

    template<>
    struct get_ffi_type<unsigned short> {
	static ffi_type * value;
    };
    template<>
    struct get_ffi_type<signed short> {
	static ffi_type * value;
    };

    template<>
    struct get_ffi_type<unsigned int> {
	static ffi_type * value;
    };
    template<>
    struct get_ffi_type<signed int> {
	static ffi_type * value;
    };

    template<>
    struct get_ffi_type<unsigned long> {
	static ffi_type * value;
    };
    template<>
    struct get_ffi_type<signed long> {
	static ffi_type * value;
    };

    template<>
    struct get_ffi_type<float> {
	static ffi_type * value;
    };

    template<>
    struct get_ffi_type<double> {
	static ffi_type * value;
    };

    template<>
    struct get_ffi_type<long double> {
	static ffi_type * value;
    };

    template<typename Ty>
    struct get_ffi_type<Ty *> {
	static ffi_type * value;
    };

    template<typename Ty>
    struct get_ffi_type<Ty &> {
	static ffi_type * value;
    };


    template<typename TyArg1, typename TyArg2>
    std::vector<ffi_type *>
    get_arg_types() {
	std::vector<ffi_type *> ret;
	ret.push_back(get_ffi_type<TyArg1>::value);
	ret.push_back(get_ffi_type<TyArg2>::value);
	return ret;
    }


    ffi_type * get_ffi_type<void>::value = &ffi_type_void;
    /*!!!*/ffi_type * get_ffi_type<char>::value = &ffi_type_schar;
    ffi_type * get_ffi_type<signed char>::value = &ffi_type_schar;
    ffi_type * get_ffi_type<unsigned char>::value = &ffi_type_uchar;
    ffi_type * get_ffi_type<signed short>::value = &ffi_type_sshort;
    ffi_type * get_ffi_type<unsigned short>::value = &ffi_type_ushort;
    ffi_type * get_ffi_type<signed int>::value = &ffi_type_sint;
    ffi_type * get_ffi_type<unsigned int>::value = &ffi_type_uint;
    ffi_type * get_ffi_type<signed long>::value = &ffi_type_slong;
    ffi_type * get_ffi_type<unsigned long>::value = &ffi_type_ulong;
    ffi_type * get_ffi_type<float>::value = &ffi_type_float;
    ffi_type * get_ffi_type<double>::value = &ffi_type_double;
    ffi_type * get_ffi_type<long double>::value = &ffi_type_longdouble;
    template<typename Ty> ffi_type * get_ffi_type<Ty*>::value = &ffi_type_pointer;
    template<typename Ty> ffi_type * get_ffi_type<Ty&>::value = &ffi_type_pointer;
}

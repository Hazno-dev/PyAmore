// Hazno - 2026

module;

#include <type_traits>

#define DTL_LIB_EXPAND(x) x

//Foreach impl
//From https://www.scs.stanford.edu/~dm/blog/va-opt.html

#define DTL_LIB_EXPAND0(...) DTL_LIB_EXPAND4(DTL_LIB_EXPAND4(DTL_LIB_EXPAND4(DTL_LIB_EXPAND4(__VA_ARGS__))))
#define DTL_LIB_EXPAND4(...) DTL_LIB_EXPAND3(DTL_LIB_EXPAND3(DTL_LIB_EXPAND3(DTL_LIB_EXPAND3(__VA_ARGS__))))
#define DTL_LIB_EXPAND3(...) DTL_LIB_EXPAND2(DTL_LIB_EXPAND2(DTL_LIB_EXPAND2(DTL_LIB_EXPAND2(__VA_ARGS__))))
#define DTL_LIB_EXPAND2(...) DTL_LIB_EXPAND1(DTL_LIB_EXPAND1(DTL_LIB_EXPAND1(DTL_LIB_EXPAND1(__VA_ARGS__))))
#define DTL_LIB_EXPAND1(...) __VA_ARGS__

#define DTL_LIB_FOR_EACH(macro, ...)										\
__VA_OPT__(DTL_LIB_EXPAND0(DTL_LIB_FOR_EACH_HELPER(macro, __VA_ARGS__)))

#define DTL_LIB_FOR_EACH_HELPER(macro, a1, ...)								\
macro a1														\
__VA_OPT__(, DTL_LIB_FOR_EACH_AGAIN DTL_LIB_PARENS (macro, __VA_ARGS__))

#define DTL_LIB_FOR_EACH_AGAIN() DTL_LIB_FOR_EACH_HELPER

//Unparent impl
//From https://stackoverflow.com/a/46311121

#define DTL_LIB_REALLY_UNPARENS(...) DTL_LIB_FOR_EACH(typename , __VA_ARGS__)
#define DTL_LIB_UNPARENS(args) DTL_LIB_EXPAND(DTL_LIB_REALLY_UNPARENS args)
#define LIB_UNPARENS(args) DTL_LIB_UNPARENS(args)

#define EXPORT_USING(name, type) \
    export using name = type

#define EXPORT_CONCEPT(name, types, ...) \
    export template<LIB_UNPARENS(types)> concept name = __VA_ARGS__

export module Core:Types;

EXPORT_USING(int8, signed char);
EXPORT_USING(uint8, unsigned char);
EXPORT_USING(int16, signed short);
EXPORT_USING(uint16, unsigned short);
EXPORT_USING(int32, signed int);
EXPORT_USING(uint32, unsigned int);
EXPORT_USING(int64, signed long long);
EXPORT_USING(uint64, unsigned long long);

EXPORT_USING(byte, unsigned char);

EXPORT_CONCEPT(Cpt_Ptr, (t_input), std::is_pointer_v<std::remove_const_t<t_input>>);

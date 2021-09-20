# named-parameters

## Example:
https://godbolt.org/z/o3Gnz3GTf
```c++
#include <named-parameters.h>
namespace par{
    NP_INL_CONSTEXPR np::Parameter<110, double> energy;
    NP_INL_CONSTEXPR np::Parameter<111, double> p1;
    NP_INL_CONSTEXPR np::Parameter<112, double> p2;
    NP_INL_CONSTEXPR np::Parameter<113, double> p3;
    NP_INL_CONSTEXPR np::Parameter<114, double> p;
}

template<NP_ARGUMENT... Args>
double mass2(Args ...args) { // return energy^2 - p1^2 - p2^2 - p3^2 = energy^2 - p^2
    using namespace par;
    auto sqr = [](double x) { return x*x; };

    double e2 = sqr(np::get_default(energy, 0, args...));
    if (np::contains(p, args...)) {
        e2 -= sqr(np::get(p, args...));        
    } else {
        e2 -= sqr(np::get_default(p1, 0, args...));
        e2 -= sqr(np::get_default(p2, 0, args...));
        e2 -= sqr(np::get_default(p3, 0, args...));
    }
    return e2;
}

double test_mass2(double a, double b, double c) {
    using namespace par;    
    return mass2(p = b, energy=a, p2 = c/*this is not used*/);
}

```

## Reference
```c++

NA_INL_CONSTEXPR np::Parameter< <unique_number> , <type without const and && or &> > <parameter_id>;
//NA_INL_CONSTEXPR is auto set to either to static constexpr or inline constexpr, according to __cplusplus

template<NP_ARGUMENT... Args>
// NP_ARGUMENT is atuo set to either to class or ::np::argument, accorinding to __cplusplus
void function(Args... args) {
    // get value for <parameter_id>
    // compile error if argument for <parameter_id> not in args
    // the same as np::get_default(<parameter_id>, np::nodef, args...)
    np::get(<parameter_id>, <default_value>, args...)
    
    // get value for <parameter_id>
    // return the <default_value> if argument for <parameter_id> not in args
    np::get_default(<parameter_id>, <default_value>, args...)
    
    // check if args contain the argument for <parameter_id>
    cosntexpr bool np::contains(<parameter_id>, args...)
    
}
```

## A long example:
```c++

#if __cplusplus >= 202002
#include <named-parameters.h>
#include <iostream>
#include <string_view>
#include <optional>

void foo(char const* comment,
    int a,
    std::optional<std::string_view> b,
    int c)
{
    std::cout << comment
        << a << " "
        << (b ? *b : "<a>") << " "
        << c << "\n";
}


NP_INL_CONSTEXPR np::Parameter<0, int> user_id;
NP_INL_CONSTEXPR np::Parameter<1, std::string_view> user_name;
NP_INL_CONSTEXPR np::Parameter<2, int> foo_a;

template<np::argument... Args >
inline void foo(char const* comment, Args&& ...args)
{
    foo(comment,
        np::get_default(user_id, np::nodef, args...),
        np::get_default(user_name, std::nullopt, args...),
        np::get_default(foo_a, 0, args...));
}


void test_foo()
{
    foo("1: ", user_id = 1, user_name = "a");
    foo("2: ", user_name = "a", user_id = 1);
    foo("3: ", user_id = 1, foo_a = 2);
    foo("4: ", user_id = 3, user_name = std::string("a"));
    //foo("5: ");
 
}
#endif

```

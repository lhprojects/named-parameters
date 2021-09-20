#include <iostream>

template<class T>
void print_type()
{
    bool is_ref = std::is_reference_v<T>;
    bool is_rref = std::is_rvalue_reference_v<T>;
    bool is_lref = std::is_lvalue_reference_v<T>;
    using PT = std::remove_reference_t<T>;
    bool is_const = std::is_const_v<PT>;
    if (is_const) printf("const ");
    printf("double");
    if (is_rref) printf("&&\n");
    if (is_lref) printf("&\n");
}

namespace types_forward_ns {

    static np::Parameter<0, double> a1;
    static np::Parameter<1, double> a2;
    static np::Parameter<2, double> a3;
    static np::Parameter<3, double> a4;
}

template<np::argument... Args>
inline void print_all_types(Args ...args)
{
    using namespace types_forward_ns;
    print_type<decltype((np::get(a1, args...)))>();
    print_type<decltype((np::get(a2, args...)))>();
    print_type<decltype((np::get(a3, args...)))>();
    print_type<decltype((np::get(a4, args...)))>();
}

void test_types_forward()
{
    using namespace types_forward_ns;
    double a = 1;
    print_all_types(a1 = (double&)a,
        a2 = (double&&)a,
        a3 = (const double&)a,
        a4 = (const double&&)a);
}

int main() {
  test_types_forward();
  return 0;
}

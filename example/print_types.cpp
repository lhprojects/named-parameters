#include <named-parameter.h>

#include <iostream>

template<class T>
void print_type()
{
    bool is_ref = std::is_reference<T>::value;
    bool is_rref = std::is_rvalue_reference<T>::value;
    bool is_lref = std::is_lvalue_reference<T>::value;
    using PT = typename std::remove_reference<T>::type;
    bool is_const = std::is_const<PT>::value;
    if (is_const) printf("const ");
    printf("double");
    if (is_rref) printf("&&\n");
    if (is_lref) printf("&\n");
}

namespace parameters {

    static np::Parameter<0, double> a1;
    static np::Parameter<1, const double> a2;
    static np::Parameter<2, double&> a3;
    static np::Parameter<3, const double&> a4;
    static np::Parameter<4, double&&> a5;
    static np::Parameter<5, const double&&> a6;
    static np::Parameter<6, int> a7;

    template<NP_ARGUMENT... Args>
    inline void print_all_types(Args ...args)
    {
        np::get(a1, args...);
        np::get(a2, args...);
        np::get(a3, args...);
        np::get(a4, args...);
        np::get(a5, args...);
        np::get(a6, args...);
        print_type<decltype((np::get(a1, args...)))>();
        print_type<decltype((np::get(a2, args...)))>();
        print_type<decltype((np::get(a3, args...)))>();
        print_type<decltype((np::get(a4, args...)))>();
        print_type<decltype((np::get(a5, args...)))>();
        print_type<decltype((np::get(a6, args...)))>();
    }

    void test_types_forward()
    {
        double a = 1;
        print_all_types(a1 = a,
            a2 = a,
            a3 = a,
            a4 = a,
            a5 = std::move(a),
            a6 = std::move(a));
    }
}


int main()
{
    parameters::test_types_forward();
    return 0;
}

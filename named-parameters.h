#ifndef NAMED_PARAMETERS_H
#define NAMED_PARAMETERS_H

#include <type_traits>
namespace np {
  
#if __cplusplus >= 201703
#define NP_INL_CONSTEXPR inline constexpr
#else
#define NP_INL_CONSTEXPR static constexpr
#endif

    template<int name, class T>
    struct Arg {

        Arg(T const& value) : value(value) {}
        T && value;

    };

    template<int name, class T>
    struct Parameter {

        Parameter() = default;

        Arg<name, T const &> operator=(T const& a) const
        {
            return Arg<name, T const &>(a);
        }
        Arg<name, T&> operator=(T& a) const
        {
            return Arg<name, T&>(a);
        }
        Arg<name, T&&> operator=(T&& a) const
        {
            return Arg<name, T&&>(a);
        }
    };

    template<class T>
    struct is_argument {
        static const bool value = false;
    };

    template<int I, class T>
    struct is_argument<Arg<I, T> > {
        static const bool value = true;
    };

    template<class... T>
    struct all_arguments;

    template<class U, class... T>
    struct all_arguments<U, T...> {
        static const bool value = is_argument<U>::value && all_arguments<T...>::value;
    };

    template<>
    struct all_arguments<> {
        static const bool value = true;
    };


    struct nodef_t {
    };
    
    NP_INL_CONSTEXPR nodef_t nodef;

    template<class T>
    struct remove_cvref {
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
    };

    template<class A>
    struct get_arg_idx;

    template<int I, class A>
    struct get_arg_idx<Arg<I,A> > {
        static const int value = I;
    };

    template<class A>
    struct argument_type_trait_impl;

    template<int I, class A>
    struct argument_type_trait_impl<Arg<I, A> > {
        using type = A;
        using forward_type = type&&;
    };

    template<class A>
    struct argument_type_trait {
        using pure_argument_type = typename remove_cvref<A>::type;
        using type = typename argument_type_trait_impl<pure_argument_type>::type;
        using forward_type = typename argument_type_trait_impl<pure_argument_type>::forward_type;
    };


    // not find the argument, continue to search
    template<int I, class E, class Default, class Arg0, class... Args>
    auto get_default2(std::false_type, Parameter<I, E> const& par,
        Default&& def, Arg0 &&arg0, Args&& ...args)
        -> decltype((get_default(par, def, args...)))
    {
        return get_default(par, def, args...);
    }


    // find the argument
    template<int I, class E, class Default, class Arg0, class... Args>
    auto get_default2(std::true_type, Parameter<I, E> const& par,
        Default&& def, Arg0 &&arg0, Args&& ...args)
        -> typename argument_type_trait<Arg0>::forward_type
    {
        typedef typename remove_cvref<Arg0>::type PureArg0;
        typedef typename argument_type_trait<PureArg0>::type arg0_type;
        return static_cast<arg0_type&&>(arg0.value);
    }

    template<int I, class E, class Default, class Arg0, class... Args>
    auto get_default(Parameter<I, E> const& par,
        Default&& def, Arg0 &&arg0, Args&& ...args)
        -> decltype((
            get_default2(
            std::integral_constant<bool,get_arg_idx<typename remove_cvref<Arg0>::type>::value == I>(),
            par,
            static_cast<Default&&>(def), 
            static_cast<Arg0>(arg0), 
            static_cast<Args>(args)...)
        ))
    {
        typedef typename remove_cvref<Arg0>::type PureArg0;
        constexpr int arg0_idx = get_arg_idx<PureArg0>::value;
        return get_default2(
            std::integral_constant<bool,arg0_idx == I>(),
            par,
            static_cast<Default&&>(def), 
            static_cast<Arg0>(arg0), 
            static_cast<Args>(args)...);
    }

    // reach the end of arguments
    template<int I, class E, class Default>
    auto get_default(Parameter<I, E> const& par,
        Default&& def)
        -> decltype((static_cast<Default&&>(def)))
    {
        typedef typename remove_cvref<Default>::type PureDefault;
        static_assert(!std::is_same<PureDefault, nodef_t>::value,
            "no default value set for par");
        return static_cast<Default&&>(def);
    }

    template<int I, class E, class... Args>
    auto get(Parameter<I, E> const& par,
        Args&& ...args)
        -> decltype((get_default(par, nodef, args...)))
    {
        return get_default(par, nodef, args...);
    }

    template<int I, class E, class Arg0, class... Args>
    constexpr bool contains(Parameter<I, E> const& par,
        Arg0 &&arg0, Args&& ...args)
    {
        typedef typename remove_cvref<Arg0>::type PureArg0;
        return I == get_arg_idx<PureArg0>::value ? true : contains(par, args...);
    }

    // reach the end of arguments
    template<int I, class E>
    constexpr bool contains(Parameter<I, E> const& par)
    {
        return false;
    }

#if __cplusplus >= 202002
    template<typename T>
    concept argument = is_argument<T>::value;
#define NA_ARGUMENT ::np::argument
#else
#define NA_ARGUMENT class
#endif


}

#endif // NAMED_PARAMETERS_H

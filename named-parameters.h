#ifndef NAMED_PARAMETERS_H
#define NAMED_PARAMETERS_H

#include <type_traits>
namespace np {

#if __cplusplus >= 201703
#define NP_INL_CONSTEXPR inline constexpr
#else
#define NP_INL_CONSTEXPR static constexpr
#endif
    template<class T>
    struct remove_cvref {
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
    };

    template<int name, class T>
    struct Arg {

        template<class U>
        Arg(U&& value) : value(static_cast<U&&>(value)) {
        }
        T value;

    };

    template<int name, class T>
    struct Parameter {

        template<class U, typename std::enable_if<
            std::is_convertible<U&&, T>::value,
            int
        >::type = 0>        
        Arg<name, T> operator=(U&& a) const
        {
            return { static_cast<U&&>(a) };
        }
    };

    namespace dtl {

        template<class T>
        struct iaimpl {
            static const bool value = false;
        };

        template<int I, class T>
        struct iaimpl<Arg<I, T> > {
            static const bool value = true;
        };


        template<class T>
        struct isarg {
            using unref_t = typename remove_cvref<T>::type;
            static const bool value = dtl::iaimpl<unref_t>::value;
        };

        template<class... T>
        struct alimpl;

        template<class U, class... T>
        struct alimpl<U, T...> {
            static const bool value = isarg<U>::value && alimpl<T...>::value;
        };

        // get argument idx implments
        template<>
        struct alimpl<> {
            static const bool value = true;
        };

        template<class A>
        struct gaiimpl;

        template<int I, class A>
        struct gaiimpl<Arg<I, A> > {
            static const int value = I;
        };

        // get argument idx
        template<class T>
        struct gaidx {
            using unref_t = typename remove_cvref<T>::type;
            static const int value = gaiimpl<unref_t>::value;
        };

        template<int I, class... Args>
        struct cntn;

        template<int I>
        struct cntn<I> {
            static const bool value = false;
        };

        template<int I, class Arg0, class... Args>
        struct cntn<I, Arg0, Args...> {
            static const int  arg0_idx = gaidx<Arg0>::value;
            static const bool value = (I == arg0_idx)
                || cntn<I, Args...>::value;
        };

        // check arguments indices
        template<class... T>
        struct chck;

        template<class U, class... T>
        struct chck<U, T...> {
            static const int idx0 = gaidx<U>::value;
            static const bool value =
                !cntn<idx0, T...>::value && chck<T...>::value;
        };
        template<>
        struct chck<> {
            static const bool value = true;
        };
    }

    template<class T>
    struct is_argument {
        static const bool value = dtl::isarg<T>::value;
    };

    template<class... T>
    struct all_arguments {
        static const bool value = dtl::alimpl<T...>::value;
    };

#if __cplusplus >= 202002
    template<typename T>
    concept argument = is_argument<T>::value;
#define NP_ARGUMENT ::np::argument
#else
#define NP_ARGUMENT class
#endif

    struct nodef_t {
    };
    NP_INL_CONSTEXPR nodef_t nodef;


    template<class A>
    struct get_arg_idx {
        static const int value = dtl::gaidx<A>::value;
    };

    template<int I, class... Args>
    struct contains_t {
        static const bool value = dtl::cntn<I, Args...>::value;
    };

    template<class... Args>
    struct check_t {
        static const bool value = dtl::chck<Args...>::value;
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

    template<int I, class E, NP_ARGUMENT Arg0, NP_ARGUMENT... Args>
    constexpr bool contains(Parameter<I, E> const& par,
        Arg0&& arg0, Args&& ...args)
    {
        static_assert(check_t<Args...>::value, "args the same parameter_id");
        typedef typename remove_cvref<Arg0>::type PureArg0;
        return I == get_arg_idx<PureArg0>::value ? true : contains(par, args...);
    }

    // reach the end of arguments
    template<int I, class E>
    constexpr bool contains(Parameter<I, E> const& par)
    {
        return false;
    }


    // not find the argument, continue to search
    template<int I, class E, class Default, class Arg0, NP_ARGUMENT... Args>
    auto get_default2(std::false_type, Parameter<I, E> const& par,
        Default&& def, Arg0&& arg0, Args&& ...args)
        -> decltype((
            get_default(par,
                static_cast<Default&&>(def),
                static_cast<Args&&>(args)...)
            ))
    {
        return get_default(par,
            static_cast<Default&&>(def),
            static_cast<Args&&>(args)...);
    }

    // find the argument
    template<int I, class E, class Default, class Arg0, NP_ARGUMENT... Args>
    auto get_default2(std::true_type, Parameter<I, E> const& par,
        Default&& def, Arg0&& arg0, Args&& ...args)
        -> typename argument_type_trait<Arg0>::forward_type
    {
        using forward_type = typename argument_type_trait<Arg0>::forward_type;
        return static_cast<forward_type&&>(
            static_cast<Arg0&&>(arg0).value
            );
    }

    template<int I, class E, class Default, class Arg0, NP_ARGUMENT... Args>
    auto get_default(Parameter<I, E> const& par,
        Default&& def, Arg0&& arg0, Args&& ...args)
        -> decltype((
            get_default2(
                std::integral_constant<bool, get_arg_idx<typename remove_cvref<Arg0>::type>::value == I>(),
                par,
                static_cast<Default&&>(def),
                static_cast<Arg0&&>(arg0),
                static_cast<Args&&>(args)...)
            ))
    {
        static_assert(check_t<Args...>::value, "args should contains unique parameter_ids");
        typedef typename remove_cvref<Arg0>::type PureArg0;
        constexpr int arg0_idx = get_arg_idx<PureArg0>::value;
        return get_default2(
            std::integral_constant<bool, arg0_idx == I>(),
            par,
            static_cast<Default&&>(def),
            static_cast<Arg0&&>(arg0),
            static_cast<Args&&>(args)...);
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

    template<int I, class E,
        NP_ARGUMENT... Args>
    auto get(Parameter<I, E> const& par,
        Args&& ...args)
        -> decltype((
            get_default(par,
            nodef,
            static_cast<Args&&>(args)...)
            ))
    {
        static_assert(check_t<Args...>::value, "args should contains unique parameter_ids");
        static_assert(contains_t<I, Args...>::value, "args doesn't contain par");
        return get_default(par,
            nodef,
            static_cast<Args&&>(args)...);
    }


}

#endif // NAMED_PARAMETERS_H

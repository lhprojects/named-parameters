#include <named-parameters.h>

struct NonCopyable {
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(NonCopyable const& r) = delete;
    NonCopyable& operator=(NonCopyable const& r) = delete;
};

namespace use_copyable_par {
    static np::Parameter<1, NonCopyable> a1;
};

template<NP_ARGUMENT Arg>
void use_copyable(Arg a)
{
}

void test_bar()
{
    using namespace use_copyable_par;
    NonCopyable bar_;
    use_copyable(a1 = bar_);
    use_copyable(a1 = std::move(bar_));
    NonCopyable const const_bar;
    use_copyable(a1 = const_bar);
    use_copyable(a1 = static_cast<const NonCopyable&&>(const_bar));
}

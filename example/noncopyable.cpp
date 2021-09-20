#include <named-parameters.h>

struct NonCopyable {
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(NonCopyable const& r) = delete;
    NonCopyable& operator=(NonCopyable const& r) = delete;
};

namespace use_copyable_par {
    static np::Parameter<1, NonCopyable &> a1;
    static np::Parameter<2, NonCopyable const &> a2;
    static np::Parameter<3, NonCopyable &&> a3;
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
    use_copyable(a2 = bar_);
    use_copyable(a3 = std::move(bar_));
    NonCopyable const const_bar;
    use_copyable(a2 = const_bar);
}

int main()
{
    test_bar();
    return 0;
}

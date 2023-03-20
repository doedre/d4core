#include <d4/static_factory.hpp>

class actor_base
{
public:
    using identifier_type = std::string;

    virtual ~actor_base() {}
    virtual identifier_type get_identifier() = 0;
    virtual void run() = 0;
};

class test_actor : public actor_base, public d4::static_factory_registration<test_actor>
{
public:
    using base = actor_base;

    test_actor()
    {
        D4_STATIC_REGISTER;
    }

    virtual base::identifier_type get_identifier() override
    {
        return identifier;
    }

    virtual void run() override
    {
        printf("Test actor been activated\n");
    }

    constexpr static base::identifier_type identifier = "test_actor";
};

using d4_static_actor_factory = d4::static_factory<actor_base>;

int main()
{
    const auto actor_name = test_actor::identifier;

    const auto f = d4_static_actor_factory::get_instance();
    const auto actor = f->make("test_actor");
    if (actor == nullptr)
        printf("actor %s cannot be created\n", actor_name.c_str());
    else
        actor->run();

    return 0;
}

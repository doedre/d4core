# d4 core

Generic concepts and utilities to ease the development of actor-based software.

## Static registrable components

In order to provide support for static component registration you should write a base class for it

```cpp
class component
{
public:
    // Satisfy constraints of d4::is_static_registrable_base<T>
    using identifier_type = std::string;

    // Any custom functions
    virtual void run() = 0;
};
```

Then inherit it with CRTP `d4::static_factory_registration<BaseT, T>` to register it
```cpp
class example final : public d4::static_factory_registration<component, example>
{
private:
    using base = component;

public:
    example()
    {
        // Macro to help overcome static variable initialization problems
      	D4_STATIC_REGISTER;
    }

    virtual void run() override
    {
        printf("Component has been activated\n");
    }

    constexpr static base::identifier_type identifier()
    {
        return "component";
    }
};
```

Finally, you can create this component with static factory instance

```cpp
using component_factory = d4::static_factory<component>;

const auto factory = component_factory::get_instance();
const auto comp = factory->make("example");
if (comp)
    comp->run();
```


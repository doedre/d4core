# c4 core

Generic concepts and utilities to ease the development of actor-based software.

## Static registrable components

In order to provide support for static component registration you should write a base class for it

```cpp
class component_base
{
public:
	using identifier_type = std::string;

	// Satisfy concept requirements
	virtual ~component_base() {}	// Should be abstract
	virtual identifier_type get_identifier() = 0;	// Provide basic information

	// Any custom functions
	virtual void run() = 0;
};
```

Then inherit it with CRTP `d4::static_factory_registration<T>` to register it
```cpp
class component final : public component_base, public d4::static_factory_registration<component>
{
public:
	using base = actor_base;

	component()
	{
		// Macro to help overcome static variable initialization problems
		D4_STATIC_REGISTER;
	}

	virtual base::identifier_type get_identifier() override
	{
		return identifier;
	}

	virtual void run() override
	{
		printf("Component has been activated\n");
	}

	constexpr static base::identifier_type identifier = "component";
};
```

Finally, you can create this component with static factory instance

```cpp
using custom_factory = d4::static_factory<component_base>;

const auto factory = custom_factory::get_instance();
const auto comp = factory->make("component");
if (comp)
	comp->run();
```


#include <d4/static_factory.hpp>

#include <boost/ut.hpp>

class component
{
 public:
  using identifier_type = std::string;
};

class example :
    public d4::static_factory_registration<component, example>
{
 public:
  example()
  {
    D4_STATIC_REGISTER;
  }

  static constexpr component::identifier_type identifier()
  {
    return "example";
  }
};

int main()
{
  using namespace boost::ut;
  using namespace boost::ut::bdd;

//  cfg<override> = { .tag = {"todo"}};

  feature("static_factory") = [] {
    scenario("Get registered and unregistered components") = [] {
      given("I have static factory with `example` component") = [] {
        using static_component_factory = d4::static_factory<component>;

        const auto sf = static_component_factory::get_instance();
        expect((sf != nullptr) >> fatal);

        when("I make `example` component") = [&sf] {
          const auto comp = sf->make("example");

          then("It should be created normally") = [&comp] {
            expect(comp != nullptr);
          };
        };

        when("I make `missing` component") = [&sf] {
          const auto comp = sf->make("missing");

          then("It should return nullptr") = [&comp] {
            expect(comp == nullptr);
          };
        };
      };
    };

    tag("todo") / scenario("Factory with errors throws on make()") = [] {
      given("I have static factory with two `example` components") = [] {
        using static_component_factory = d4::static_factory<>;

        const auto sf = static_component_factory::get_instance();
        expect((sf == nullptr) >> fatal);
      };
    };
  };

  return 0;
}

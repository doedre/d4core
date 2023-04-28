#include <boost/ut.hpp>

#include <d4/static_factory.hpp>

class num65 final : public d4::static_factory_registration<d4::default_static_registrable, num65>
{
 private:
  using base = d4::default_static_registrable;

 public:
  num65()
  {
    D4_STATIC_REGISTER;
  }

  static constexpr base::identifier_type identifier() noexcept
  {
    return 65;
  }
};

class number65 final : public d4::static_factory_registration<d4::default_static_registrable, number65>
{
 private:
  using base = d4::default_static_registrable;

 public:
  number65()
  {
    D4_STATIC_REGISTER;
  }

  static constexpr base::identifier_type identifier() noexcept
  {
    return 65;
  }
};

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

  static constexpr component::identifier_type identifier() noexcept
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
        expect((!sf->is_corrupted().first) >> fatal);

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

    scenario("Working with corrupted static factory") = [] {
      given("I have static factory with two `65` components") = [] {
        using static_component_factory = d4::static_factory<>;
        
        const auto sf = static_component_factory::get_instance();
        expect((sf != nullptr) >> fatal);
        expect((sf->is_corrupted().first) >> fatal);

        when("I make `65` component") = [&sf] {
          then("It should throw") = [&sf] {
            expect(throws([&sf] { sf->make(65); }));
          };
        };

        when("I make any other component") = [&sf] {
          then("It should also throw") = [&sf] {
            expect(throws([&sf] { sf->make(10); }));
          };
        };
      };
    };
  };

  return 0;
}


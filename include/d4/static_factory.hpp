#pragma once

#include <map>
#include <memory>
#include <iostream>

#include <d4/type_traits.hpp>

namespace d4
{

template<class T>
concept is_static_registrable_base = requires(T t) {
                                      typename T::identifier_type;
                                    };

template<class BaseT, class T>
concept is_static_registrable = requires(BaseT srbase, T sr) {
                                  std::is_base_of_v<BaseT, T>;
                                  typename BaseT::identifier_type;
                                  sr.identifier();
                                  T::identifier();
                                };

namespace detail
{

template<class StaticRegistrableBase>
class proxy_base
{
 public:
  virtual ~proxy_base() = default;

  virtual std::unique_ptr<StaticRegistrableBase> make() = 0;
};

template<class StaticRegistrable, class StaticRegistrableBase>
requires is_static_registrable<StaticRegistrableBase, StaticRegistrable>
class proxy final : public proxy_base<StaticRegistrableBase>
{
 public:
  virtual std::unique_ptr<StaticRegistrableBase> make() override
  {
    return std::make_unique<StaticRegistrable>();
  }
};

} // namespace detail

class default_static_registrable
{
 public:
  using identifier_type = int;
};

template<class StaticRegistrableBase = default_static_registrable>
requires is_static_registrable_base<StaticRegistrableBase>
class static_factory
{
 public:
  using identifier_type = typename StaticRegistrableBase::identifier_type;

  static_factory() = default;

  static_factory(const static_factory&) = delete;
  static_factory(static_factory&&) = delete;
  static_factory& operator=(const static_factory&) = delete;
  static_factory& operator=(static_factory&&) = delete;

  static std::shared_ptr<static_factory> get_instance()
  {
    if (m_instance == nullptr)
      m_instance = std::make_shared<static_factory>();

    return m_instance;
  }

  template<class StaticRegistrable>
  requires is_static_registrable<StaticRegistrableBase, StaticRegistrable>
  constexpr bool static_registration(const identifier_type& key)
  {
    std::cout << "Register " << key << " component\n";
    const auto [it, success] = m_proxies.insert({ key, std::make_unique<detail::proxy<StaticRegistrable, StaticRegistrableBase>>() });
    if (!success)
    {
      m_failed = true;
      m_failed_identifier = key;
    }

    return success;
  }

  std::unique_ptr<StaticRegistrableBase> make(const identifier_type& key)
  {
    if (m_failed)
      throw m_failed_identifier;

    auto p = m_proxies.find(key);
    if (p != m_proxies.end())
      return p->second->make();
    else
      return std::unique_ptr<StaticRegistrableBase>();
  }

  std::pair<bool, identifier_type> is_corrupted() const noexcept
  {
    return std::make_pair(m_failed, m_failed_identifier);
  }

 private:
  bool m_failed{false};
  static inline std::shared_ptr<static_factory> m_instance{nullptr};
  identifier_type m_failed_identifier;
  std::map<identifier_type, std::unique_ptr<detail::proxy_base<StaticRegistrableBase>>> m_proxies;
};

#define D4_STATIC_REGISTER ((void)m_registered)

template<class StaticRegistrableBase, class StaticRegistrable>
class static_factory_registration : public StaticRegistrableBase
{
 public:
  bool is_registered() 
  {
    return m_registered;
  }

 protected:
  static inline bool m_registered =
            static_factory<StaticRegistrableBase>::get_instance()
            ->template static_registration<StaticRegistrable>(StaticRegistrable::identifier());
};

}   // namespace d4

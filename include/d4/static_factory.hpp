#pragma once

#include <map>
#include <memory>

#include <d4/type_traits.hpp>

namespace d4
{

namespace detail
{
    template<typename T>
    using get_identifier_expression = decltype(std::declval<T&>().get_identifier());
}   // namespace detail

template<class T>
concept is_static_registrable_base =
        std::is_abstract_v<T> &&
        d4::is_detected<detail::get_identifier_expression, T>;

template<class T, class BaseT>
concept is_static_registrable =
        std::is_base_of_v<BaseT, T> &&
        d4::is_detected<detail::get_identifier_expression, T>;

namespace detail
{

template<class StaticRegistrableBase>
requires is_static_registrable_base<StaticRegistrableBase>
class proxy_base
{
public:
    virtual ~proxy_base() {}

    virtual std::unique_ptr<StaticRegistrableBase> make() = 0;
};

template<class StaticRegistrable, class StaticRegistrableBase>
requires is_static_registrable<StaticRegistrable, StaticRegistrableBase>
class proxy final : public proxy_base<StaticRegistrableBase>
{
public:
    virtual std::unique_ptr<StaticRegistrableBase> make() override
    {
        return std::make_unique<StaticRegistrable>();
    }
};

}   // namespace detail

template<class StaticRegistrableBase>
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
    requires is_static_registrable<StaticRegistrable, StaticRegistrableBase>
    constexpr bool static_registration(const identifier_type& key)
    {
        m_proxies[key] = std::make_unique<detail::proxy<StaticRegistrable, StaticRegistrableBase>>();
        return true;
    }

    std::unique_ptr<StaticRegistrableBase> make(const identifier_type& key)
    {
        auto p = m_proxies.find(key);
        if (p != m_proxies.end())
        {
            return p->second->make();
        }
        else
        {
            return std::unique_ptr<StaticRegistrableBase>();
        }
    }

private:
    static inline std::shared_ptr<static_factory> m_instance = nullptr;
    std::map<identifier_type, std::unique_ptr<detail::proxy_base<StaticRegistrableBase>>> m_proxies;
};

#define D4_STATIC_REGISTER ((void)m_registered)

template<class StaticRegistrable>
class static_factory_registration
{
public:
    bool is_registered() 
    {
        return m_registered;
    }

protected:
    static inline bool m_registered =
            static_factory<typename StaticRegistrable::base>::get_instance()
            ->template static_registration<StaticRegistrable>(StaticRegistrable::identifier);
};

}   // namespace d4

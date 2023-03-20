#pragma once

#include <d4/type_traits.hpp>

namespace d4
{

template<typename IdentifierT>
class default_actor_base
{
public:
    virtual ~default_actor_base() {}
};

namespace detail
{
}   // namespace detail

template<class T>
concept is_actor_base = std::is_abstract_v<T>;

template<class T, class BaseT>
concept is_actor = is_actor_base<BaseT> && std::is_base_of_v<BaseT, T>;

}


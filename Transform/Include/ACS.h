#pragma once

#include <iostream>
#include <utility>
#include <map>
#include <set>
#include <string>
#include <concepts>
#include <functional>
#include <bitset>
#include <memory>

#pragma warning(disable : 4834)

namespace ACS
{
  struct ISystem
  {
    virtual void operator () (float elapsedTime) = 0;
  };
  struct IComponent
  {
    IComponent(std::string const& actorName) {}
  };
  struct IActor
  {
    std::string mActorName{};

    IActor(std::string const& actorName)
      : mActorName{ actorName } {}
  };

  struct TypeRegistry
  {
    using Registry = std::array<void*, 64>;

    std::map<std::size_t, std::size_t> mTypeRegistry     {};
    std::unique_ptr<Registry>          mComponentRegistry{ std::unique_ptr<Registry>{} };

    template<typename C>
    inline void RegisterComponent() noexcept
    {
      static std::size_t index{};
      auto [typeIt, _] { mTypeRegistry.emplace(typeid(C).hash_code(), index++) };
    }

    template<typename C>
    inline C* GetOrAttach() noexcept
    {
      std::size_t const regIdx{ mTypeRegistry.at(typeid(C).hash_code()) };
      return reinterpret_cast<C*>(mComponentRegistry[regIdx]);
    }
  };

  inline static std::map<std::string, IActor*>       sActors           {};
  inline static std::map<std::string, ISystem*>      sSystems          {};

  inline static std::map<std::size_t, TypeRegistry*> sCompMask2Registry{};
  inline static std::map<IActor*,     TypeRegistry*> sActor2Registry   {};

  template<typename A>
  requires std::is_base_of_v<IActor, A>
  inline static A*      Create(std::string const& actorName) noexcept
  {
    auto [actorIt, _] { sActors.emplace(actorName, new A{ actorName }) };
    return reinterpret_cast<A*>(actorIt->second);
  }

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  inline static C*      GetOrAttach(std::string const& actorName) noexcept
  {
    //auto actorIt{ sActors.find(actorName) };
    //if (actorIt == sActors.end()) return nullptr;

    //auto [compIt, _] { actorIt->second->mComponents.emplace(typeid(C).name(), new C{ actorName }) };
    //return reinterpret_cast<C*>(compIt->second);

    

    return nullptr;
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static S*      Register(Args&&... arg) noexcept
  {
    auto [sysIt, _] { sSystems.emplace(typeid(S).name(), new S{ std::forward<Args>(arg)... }) };
    return reinterpret_cast<S*>(sysIt->second);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static void    Update(Args&&... arg) noexcept
  {
    for (const auto& [systemName, pSystem] : sSystems)
      (*reinterpret_cast<S*>(pSystem))(std::forward<Args>(arg)...);
  }

  template<typename T>
  struct IdentityType
  {
    using Type    = T;
    using Pointer = T*;
  };

  template<typename ... Components>
  inline static void    ForEach(std::function<void(typename IdentityType<Components>::Pointer...)> const& predicate) noexcept
  {
    std::size_t const typeHash{ (typeid(typename IdentityType<Components>::Type).hash_code() | ... | 0) };

    //using C = decltype(func);

    for (const auto& [actorName, pActor] : sActors)
    {
      std::size_t const compHash{ std::uintptr_t{ pActor } };

      //sComponents.at(compHash);

      //auto compIt{ pActor->mComponents.find(typeid(C).name()) };
      //if (compIt == pActor->mComponents.end()) continue;
      //func((reinterpret_cast<C*>(compIt->second)));
    }
  }
}
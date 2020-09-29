#pragma once

#include <iostream>
#include <utility>
#include <map>
#include <set>
#include <string>
#include <concepts>
#include <functional>

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
    std::string                        mActorName {};
    std::map<std::string, IComponent*> mComponents{};

    IActor(std::string const& actorName)
      : mActorName{ actorName } {}
  };

  inline static std::map<std::string, IActor*>  sActors{};
  inline static std::map<std::string, ISystem*> sSystems{};

  template<typename A>
  requires std::is_base_of_v<IActor, A>
  inline static A*      Create(std::string const& actorName) noexcept
  {
    auto [actorIt, _] { sActors.emplace(actorName, new A{ actorName }) };
    return reinterpret_cast<A*>(actorIt->second);
  }

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  inline static C*      Attach(std::string const& actorName) noexcept
  {
    auto actorIt{ sActors.find(actorName) };
    if (actorIt == sActors.end()) return nullptr;
    auto [compIt, _] { actorIt->second->mComponents.emplace(typeid(C).name(), new C{ actorName }) };
    return reinterpret_cast<C*>(compIt->second);
  }

  template<typename S>
  requires std::is_base_of_v<ISystem, S>
  inline static S*      Register() noexcept
  {
    auto [sysIt, _] { sSystems.emplace(typeid(S).name(), new S) };
    return reinterpret_cast<S*>(sysIt->second);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static void    Update(Args&&... arg) noexcept
  {
    for (const auto& [systemName, pSystem] : sSystems)
      (*reinterpret_cast<S*>(pSystem))(std::forward<Args>(arg)...);
  }

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  inline static void    ForEach(std::function<void(C*)> func) noexcept
  {
    for (const auto& [actorName, pActor] : sActors)
    {
      auto compIt{ pActor->mComponents.find(typeid(C).name()) };
      if (compIt == pActor->mComponents.end()) continue;
      func((reinterpret_cast<C*>(compIt->second)));
    }
  }
}
#pragma once

#include <utility>
#include <array>
#include <map>
#include <set>
#include <string>
#include <concepts>
#include <cstdint>
#include <functional>

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

  using TActor = std::tuple<IActor *, std::uint64_t, void * *>;

  inline static constexpr std::uint64_t               maxComponents{ 64 };

  inline static std::map<std::size_t, std::size_t>    sTypeRegistry{};
  inline static std::map<std::string, TActor>         sActors      {};
  inline static std::map<std::string, ISystem *>      sSystems     {};

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  inline static std::size_t Type2Index() noexcept
  {
    static std::size_t distinctTypeCount{ 1 };
    auto const typeIt{ sTypeRegistry.find(typeid(C).hash_code()) };

    if (typeIt == sTypeRegistry.end())
    {
      auto const [resultIt, _] { sTypeRegistry.emplace(typeid(C).hash_code(), distinctTypeCount << 1) };
      return resultIt->second;
    }

    return typeIt->second;
  }

  template<typename A>
  requires std::is_base_of_v<IActor, A>
  inline static A *         Create(std::string const& actorName) noexcept
  {
    auto [actorIt, _] { sActors.emplace(actorName, std::tuple<IActor *, std::uint64_t>{ new A{ actorName }, 0 }) };
    return reinterpret_cast<A*>(std::get<0>(actorIt->second));
  }

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  inline static C *         GetOrAttach(std::string const & actorName) noexcept
  {
    // opt this section away
    auto const actorIt{ sActors.find(actorName) };
    if (actorIt == sActors.end()) return nullptr;

    // dont xor use uint128 and shifts
    std::size_t const actorIndex{ std::uintptr_t(&std::get<0>(actorIt->second)) };
    std::size_t const typeIndex{ Type2Index<C>() };

    u128 const compIndex{ actorIndex, typeIndex };

    auto const compIt{ sComponents.find(compIndex) };

    Registers * pRegisters{};

    if (compIt == sComponents.end())
    {
      auto const [resultIt, _] { sComponents.emplace(compIndex, new Registers) };
      
      Registers * pRegisters = resultIt->second;
      pRegisters->mMask |= typeIndex;
      pRegisters->mppRegisters = static_cast<void * *>(std::malloc(sizeof(void *) * maxComponents));

      std::memset(pRegisters->mppRegisters, 0, sizeof(void *) * maxComponents);

      std::get<1>(actorIt->second) |= typeIndex;
    }
    else
    {
      pRegisters = compIt->second;
    }

    void * pRegister = pRegisters->mppRegisters[typeIndex];

    if (!pRegister)
    {
      pRegister = new C{ actorName };
    }

    return reinterpret_cast<C *>(pRegister);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static S *         Register(Args && ... args) noexcept
  {
    auto const [sysIt, _] { sSystems.emplace(typeid(S).name(), new S{ std::forward<Args>(args) ... }) };
    return reinterpret_cast<S *>(sysIt->second);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static void        Update(Args && ... args) noexcept
  {
    for (const auto & [systemName, pSystem] : sSystems)
      (* reinterpret_cast<S *>(pSystem))(std::forward<Args>(args) ...);
  }

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  struct Identity
  {
    using Type    = C;
    using Pointer = C *;
  };

  template<typename ... Components>
  inline static void    ForEach(std::function<void(typename Identity<Components>::Pointer...)> const & predicate) noexcept
  {
    for (auto const & [actorName, actor ] : sActors)
    {
      IActor * pActor{ std::get<0>(actor) };
      std::uint64_t compMask{ std::get<1>(actor) };

      std::size_t const actorIndex{ std::uintptr_t(pActor) };
      std::size_t const typeIndex{ (Type2Index<typename Identity<Components>::Type>() | ... | 0) };

      if (compMask != typeIndex) continue;

      u128 const compIndex{ actorIndex, typeIndex };

      // is this find rly required
      auto const compIt{ sComponents.find(compIndex) };

      if (compIt == sComponents.end()) continue;

      Registers * pRegisters{ compIt->second };

      // type index wont work for multiple type xors
      // fold expr required to apply predicate to all sub-components
      (predicate((reinterpret_cast<typename Identity<Components>::Pointer>(pRegisters->mppRegisters[Type2Index<typename Identity<Components>::Type>()]))), ...);
    }
  }

}
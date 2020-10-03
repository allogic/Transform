#pragma once

#include <utility>
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
    
  };
  struct IActor
  {
    std::string mActorName{};

    IActor(std::string const& actorName)
      : mActorName{ actorName } {}
  };

  struct Actor
  {
    IActor *      mpActor     {};
    std::uint64_t mMask       {};
    void * *      mppRegisters{};
  };
  struct Key
  {
    char          mLogicalIndex{};
    std::uint64_t mMaskBit     {};
  };

  inline static std::size_t                      sDistinctTypeCount{};
  inline static std::map<std::size_t, Key>       sTypeRegistry     {};
  inline static std::map<std::string, Actor>     sActors           {};
  inline static std::map<std::size_t, ISystem *> sSystems          {};

  // obsolete -> make constexpr hash map, maybe xor will do..
  template<typename C, bool AsLogicalIndex>
  requires std::is_base_of_v<IComponent, C>
  inline static std::size_t Type2Index() noexcept
  {
    auto const typeIt{ sTypeRegistry.find(typeid(C).hash_code()) };

    if (typeIt == sTypeRegistry.end())
    {
      Key key
      {
        .mLogicalIndex{ static_cast<char>(sDistinctTypeCount) },
        .mMaskBit     { static_cast<std::uint64_t>(1) << sDistinctTypeCount },
      };

      sDistinctTypeCount++;

      auto const [resultIt, _] { sTypeRegistry.emplace(typeid(C).hash_code(), std::move(key)) };

      if constexpr (AsLogicalIndex) return resultIt->second.mLogicalIndex;
      else                          return resultIt->second.mMaskBit;
    }

    if constexpr (AsLogicalIndex) return typeIt->second.mLogicalIndex;
    else                          return typeIt->second.mMaskBit;
  }

  template<typename ... C>
  requires (std::is_base_of_v<IComponent, C>, ...)
  inline static constexpr std::size_t Types2Mask()
  {
    return (typeid(C).hash_code() | ... | 0);
  }

  template<typename A>
  requires std::is_base_of_v<IActor, A>
  inline static A *         Create(std::string const & actorName) noexcept
  {
    Actor actor
    {
      .mpActor     { new A{ actorName } },
      .mMask       {},
      .mppRegisters{ static_cast<void * *>(std::malloc(sizeof(void *) * 64)) },
    };

    std::memset(actor.mppRegisters, 0, sizeof(void *) * 64);

    auto const [actorIt, _] { sActors.emplace(actorName, std::move(actor)) };
    return reinterpret_cast<A *>(actorIt->second.mpActor);
  }

  template<typename C, typename ... Args>
  requires std::is_base_of_v<IComponent, C>
  inline static C *         GetOrAttach(std::string const & actorName, Args && ... args) noexcept
  {
    auto const actorIt{ sActors.find(actorName) };
    if (actorIt == sActors.end()) return nullptr;

    Actor & actor{ actorIt->second };

    std::size_t const mLogicalIndex{ Type2Index<C, true>() };
    std::size_t const maskBit{ Type2Index<C, false>() };

    if (! (actor.mMask & maskBit))
    {
      actor.mMask |= maskBit;
      actor.mppRegisters[mLogicalIndex] = new C{ std::forward<Args>(args) ... };
    }

    return reinterpret_cast<C *>(actor.mppRegisters[mLogicalIndex]);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static S *         Register(Args && ... args) noexcept
  {
    auto const [sysemIt, _] { sSystems.emplace(typeid(S).hash_code(), new S{ std::forward<Args>(args) ... }) };
    return reinterpret_cast<S *>(sysemIt->second);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static void        Update(Args && ... args) noexcept
  {
    for (const auto & [hash, pSystem] : sSystems)
      (* reinterpret_cast<S *>(pSystem))(std::forward<Args>(args) ...);
  }

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  struct Identity
  {
    using Type    = C;
    using Pointer = C *;
  };

  struct Instruction
  {
    std::uint64_t mMask;
  };

  using Instructions = std::set<Instruction>;

  inline static void    Submit(Instruction const & instruction)
  {

  }

  // maybe parallelize?
  // build queuing system..
  template<typename ... Components>
  inline static void    ForEach(std::function<void(typename Identity<Components>::Pointer...)> const & predicate) noexcept
  {
    for (auto const & [name, actor] : sActors)
    {
      std::size_t const mask{ (Type2Index<typename Identity<Components>::Type, false>() | ... | 0) };

      if (actor.mMask & mask)
      {
        predicate
        (
          reinterpret_cast<typename Identity<Components>::Pointer>
          (
            actor.mppRegisters[Type2Index<typename Identity<Components>::Type, true>()]
          ) ...
        );
      }
    }
  }

}
#pragma once

#include <Debug.h>
#include <Common.h>

#include <utility>
#include <map>
#include <set>
#include <array>
#include <tuple>
#include <string>
#include <concepts>
#include <cstdint>
#include <utility>

namespace ACS
{
  template<typename T>
  struct [[nodiscard("Internal type")]] Identity
  {
    using Type     = T;
    using Pointer  = T *;
    using CPointer = T const *;
  };

  struct [[nodiscard("Internal type")]] Actor
  {
    IActor *      mpActor     {};
    std::uint64_t mMask       {};
    void * *      mppRegisters{};
  };
  struct [[nodiscard("Internal type")]] Key
  {
    char          mLogicalIndex{};
    std::uint64_t mMaskBit     {};
  };
  struct [[nodiscard("Internal type")]] Job
  {
    std::uint64_t mMask     {};
    void *        mpInstance{};

    inline bool operator () (Job const & lhs, Job const & rhs) const noexcept { return rhs.mpInstance < lhs.mpInstance; }
  };

  inline static std::uint64_t                      sDistinctTypeCount{};
  inline static std::map<std::uint64_t, Key>       sTypeRegistry     {};
  inline static std::map<std::string, Actor>       sActors           {};
  inline static std::map<std::uint64_t, ISystem *> sSystems          {};
  inline static std::set<Job, Job>                 sJobs             {};

  template<typename C, bool AsLogicalIndex>
  requires std::is_base_of_v<IComponent, C>
  inline static std::uint64_t Type2Index() noexcept
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

      auto const [resultIt, _] { sTypeRegistry.emplace(typeid(C).hash_code(), key) };

      if constexpr (AsLogicalIndex) return resultIt->second.mLogicalIndex;
      else                          return resultIt->second.mMaskBit;
    }

    if constexpr (AsLogicalIndex) return typeIt->second.mLogicalIndex;
    else                          return typeIt->second.mMaskBit;
  }

  template<typename A>
  requires std::is_base_of_v<IActor, A>
  inline static A *           CreateActor(std::string const & actorName) noexcept
  {
    Actor actor
    {
      .mpActor     { new A{ actorName } },
      .mMask       {},
      .mppRegisters{ static_cast<void * *>(std::malloc(sizeof(void *) * 64)) },
    };

    std::memset(actor.mppRegisters, 0, sizeof(void *) * 64);

    auto const [actorIt, _] { sActors.emplace(actorName, actor) };
    return reinterpret_cast<A *>(actorIt->second.mpActor);
  }

  template<typename C, typename ... Args>
  requires std::is_base_of_v<IComponent, C>
  inline static C *           GetOrAttachComponent(std::string const & actorName, Args && ... args) noexcept
  {
    auto const actorIt{ sActors.find(actorName) };
    if (actorIt == sActors.end()) return nullptr;

    Actor & actor{ actorIt->second };

    std::uint64_t const logicalIndex{ Type2Index<C, true>() };
    std::uint64_t const maskBit{ Type2Index<C, false>() };

    if (! (actor.mMask & maskBit))
    {
      actor.mMask |= maskBit;
      actor.mppRegisters[logicalIndex] = new C{ std::forward<Args>(args) ... };
    }

    return reinterpret_cast<C *>(actor.mppRegisters[logicalIndex]);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static S *           RegisterSystem(Args && ... args) noexcept
  {
    auto const [sysemIt, _] { sSystems.emplace(typeid(S).hash_code(), new S{ std::forward<Args>(args) ... }) };
    return reinterpret_cast<S *>(sysemIt->second);
  }

  // maybe parallelize?
  template<typename ... Systems>
  requires (std::is_base_of_v<ISystem, Systems> && ...)
  inline static void          UpdateSystems(float elapsedTime) noexcept
  {
    for (const auto& [hash, pSystem] : sSystems)
    {
      (
        ((*reinterpret_cast<typename Identity<Systems>::Pointer>(pSystem))(elapsedTime)) // call
      , ...);
    }
  }

  template<typename S, typename ... Components>
  requires std::is_base_of_v<ISystem, S> && (std::is_base_of_v<IComponent, Components> && ...)
  inline static void          SubmitJob(S * pInstance) noexcept
  {
    Job job
    {
      (Type2Index<typename Identity<Components>::Type, false>() | ... | 0),
      pInstance,
    };

    sJobs.insert(job);
  }

  // maybe parallelize?
  template<typename ... Systems>
  requires (std::is_base_of_v<ISystem, Systems> && ...)
  inline static void          DispatchSystems() noexcept
  {
    for (auto const & [name, actor] : sActors)
    {
      // only apply jobs which match system mask
      for (auto const & job : sJobs)
      {
        if (job.mMask == actor.mMask)
        {
          // decode components from registers!
          (
            ((* reinterpret_cast<typename Identity<Systems>::Pointer>(job.mpInstance))(reinterpret_cast<typename Identity<Systems>::Pointer>(job.mpInstance))) // call
          , ...);
        }
      }
    }
  }
}
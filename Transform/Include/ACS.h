#pragma once

#include <Debug.h>
#include <Common.h>

#define ACS_EXCLUDE
#include <Components.h>
#undef ACS_EXCLUDE

#include <utility>
#include <map>
#include <set>
#include <array>
#include <tuple>
#include <string>
#include <concepts>
#include <cstdint>
#include <utility>
#include <bitset>

#include <olcPixelGameEngine.h>

// properly implement std::uint_t

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
    IActor *     mpActor     {};
    std::uint8_t mMask       {};
    void * *     mppRegisters{};
  };
  struct [[nodiscard("Internal type")]] Key
  {
    std::uint8_t mLogicalIndex{};
    std::uint8_t mMaskBit     {};
  };
  struct [[nodiscard("Internal type")]] Job
  {
    std::uint8_t mMask     {};
    void *       mpInstance{};
    std::uint8_t mNumArgs  {};

    inline bool operator () (Job const & lhs, Job const & rhs) const noexcept
    {
      // extend comparison
      return rhs.mpInstance < lhs.mpInstance;
    }
  };

  inline static std::uint8_t                       sDistinctTypeCount{};
  inline static std::map<std::uint64_t, Key>       sTypeRegistry     {};
  inline static std::map<std::string, Actor>       sActors           {};
  inline static std::map<std::uint64_t, ISystem *> sSystems          {};
  inline static std::set<Job, Job>                 sJobs             {};

  template<typename C, bool AsLogicalIndex>
  requires std::is_base_of_v<IComponent, C>
  inline static std::uint8_t  Type2Index() noexcept
  {
    auto const typeIt{ sTypeRegistry.find(typeid(C).hash_code()) };

    if (typeIt == sTypeRegistry.end())
    {
      Key key
      {
        .mLogicalIndex{ sDistinctTypeCount },
        .mMaskBit     { static_cast<std::uint8_t>(1 << sDistinctTypeCount) },
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

    std::uint8_t const logicalIndex{ Type2Index<C, true>() };
    std::uint8_t const maskBit{ Type2Index<C, false>() };

    if (! (actor.mMask & maskBit))
    {
      actor.mMask |= maskBit;
      actor.mppRegisters[logicalIndex] = new C{ std::forward<Args>(args) ... };
    }

    return reinterpret_cast<C *>(actor.mppRegisters[logicalIndex]);
  }

  template<typename ... Systems>
  requires (std::is_base_of_v<ISystem, Systems> && ...)
  inline static void          RegisterSystems(olc::PixelGameEngine * pEngine) noexcept
  {
    (
      (sSystems.emplace(typeid(typename Identity<Systems>::Type).hash_code(), new typename Identity<Systems>::Type{ pEngine }))
    , ...);
  }

  // maybe parallelize?
  template<typename ... Systems>
  requires (std::is_base_of_v<ISystem, Systems> && ...)
  inline static void          UpdateSystems(float elapsedTime) noexcept
  {
    for (const auto & [hash, pSystem] : sSystems)
    {
      (
        ((* reinterpret_cast<typename Identity<Systems>::Pointer>(pSystem))(elapsedTime))
      , ...);
    }
  }

  template<typename S, typename ... Components>
  requires std::is_base_of_v<ISystem, S> && (std::is_base_of_v<IComponent, Components> && ...)
  inline static void          SubmitJob(S * pInstance) noexcept
  {
    Job job
    {
      .mMask     { (Type2Index<typename Identity<Components>::Type, false>() | ... | 0u) },
      .mpInstance{ pInstance },
      .mNumArgs  { sizeof ... (Components) },
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
      for (auto const & job : sJobs)
      {
        (
          (
            (job.mMask == actor.mMask)
              ? (* reinterpret_cast<typename Identity<Systems>::Pointer>(job.mpInstance))(reinterpret_cast<typename Identity<Systems>::Pointer>(job.mpInstance), actor.mppRegisters)
              : void()
          )
        , ...);

        // fold here.. ?

        // build args tuple
        //for (std::uint8_t i{ 1 }; i < job.mNumArgs; i++)
        //{
        //  if (job.mMask & (1 << i)) Component0 * pTransform{ reinterpret_cast<Component0 *>(actor.mppRegisters[i]) };
        //}
        //
        //Component2 * pDecal{ reinterpret_cast<Component2 *>(actor.mppRegisters[0]) };
        //Component5 * pBlockDynamic{ reinterpret_cast<Component5 *>(actor.mppRegisters[1]) };

        // decode components from registers!

        // static approach
        //if (job.mMask & Type2Index<BlockResource, false>())
        //{
        //  BlockResource * pBlockResource{ reinterpret_cast<BlockResource *>(actor.mpRegisters[Type2Index<BlockResource, true>()]) };
        //}
        //
        //if (job.mMask & Type2Index<Decal, false>())
        //{
        //  Decal * pDecal{ reinterpret_cast<Decal *>(actor.mpRegisters[Type2Index<Decal, true>()]) };
        //}

        // dynamic approach
        //for (std::uint8_t i{ 128 }; i > 0; i >>= 1)
        //{
        //  std::cout << std::bitset<8>(i) << std::endl;
        //
        //  if (job.mMask & i) // found active component
        //  {
        //
        //  }
        //}
      }
    }
  }
}
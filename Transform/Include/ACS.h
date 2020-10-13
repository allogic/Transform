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

namespace ACS
{
  template<typename T>
  struct [[nodiscard("Internal type")]] Identity
  {
    using Type     = T;
    using Pointer  = T *;
    using CPointer = T const *;
  };

  struct [[nodiscard("Internal type")]] AMR
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
    std::uint8_t           mMask     {};
    std::type_info const * mpArgsInfo{};
    void *                 mpSystem  {};
    std::uint8_t           mNumArgs  {};

    inline bool operator () (Job const & lhs, Job const & rhs) const noexcept
    {
      // extend comparison
      return rhs.mpSystem < lhs.mpSystem;
    }
  };

  inline static std::uint8_t                                    sDistinctComponentCount{};
  inline static std::map<std::uint64_t, Key>                    sComponentRegistry     {};
  inline static std::map<std::uint64_t, std::type_info const *> sComponentTupleRegistry{};
  inline static std::map<std::string, AMR>                      sAmrs                  {};
  inline static std::map<std::uint64_t, ISystem *>              sSystems               {};
  inline static std::set<Job, Job>                              sJobs                  {};

  template<typename Component, bool AsLogicalIndex>
  requires std::is_base_of_v<IComponent, Component>
  inline static std::uint8_t  Component2Index() noexcept
  {
    auto const compIt{ sComponentRegistry.find(typeid(Component).hash_code()) };

    if (compIt == sComponentRegistry.end())
    {
      Key key
      {
        .mLogicalIndex{ sDistinctComponentCount },
        .mMaskBit     { static_cast<std::uint8_t>(1 << sDistinctComponentCount) },
      };

      sDistinctComponentCount++;

      auto const [resultIt, _] { sComponentRegistry.emplace(typeid(Component).hash_code(), key) };

      if constexpr (AsLogicalIndex) return resultIt->second.mLogicalIndex;
      else                          return resultIt->second.mMaskBit;
    }

    if constexpr (AsLogicalIndex) return compIt->second.mLogicalIndex;
    else                          return compIt->second.mMaskBit;
  }

  template<typename ... Components>
  requires (std::is_base_of_v<IComponent, typename Identity<Components>::Type> && ...)
  inline static std::type_info const * ComponentTuple2Index() noexcept
  {
    auto const compTupleIt{ sComponentTupleRegistry.find(typeid(std::tuple<typename Identity<Components>::Type> ...).hash_code()) };

    if (compTupleIt == sComponentTupleRegistry.end())
    {
      auto const [resultIt, _]
      {
        sComponentTupleRegistry.emplace(
          typeid(std::tuple<typename Identity<Components>::Type> ...).hash_code(),
          & typeid(std::tuple<typename Identity<Components>::Type> ...))
      };

      return resultIt->second;
    }

    return compTupleIt->second;
  }

  // implement mem pool
  template<typename Actor>
  requires std::is_base_of_v<IActor, Actor>
  inline static Actor *       GetOrCreateActor(std::string const & actorName) noexcept
  {
    AMR amr
    {
      .mpActor     { new Actor{ actorName } },
      .mMask       {},
      .mppRegisters{ static_cast<void * *>(std::malloc(sizeof(void *) * 64)) },
    };

    std::memset(amr.mppRegisters, 0, sizeof(void *) * 64);

    auto const [amrIt, _] { sAmrs.emplace(actorName, amr) };
    return reinterpret_cast<Actor *>(amrIt->second.mpActor);
  }

  template<typename Component, typename ... Args>
  requires std::is_base_of_v<IComponent, Component>
  inline static Component *   GetOrCreateComponent(std::string const & actorName, Args && ... args) noexcept
  {
    auto const amrIt{ sAmrs.find(actorName) };
    if (amrIt == sAmrs.end()) return nullptr;

    AMR & amr{ amrIt->second };

    std::uint8_t const logicalIndex{ Component2Index<Component, true>() };
    std::uint8_t const maskBit{ Component2Index<Component, false>() };

    if (! (amr.mMask & maskBit))
    {
      amr.mMask |= maskBit;
      amr.mppRegisters[logicalIndex] = new Component{ std::forward<Args>(args) ... };
    }

    return reinterpret_cast<Component *>(amr.mppRegisters[logicalIndex]);
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

  template<typename System, typename ... Components>
  requires std::is_base_of_v<ISystem, System> && (std::is_base_of_v<IComponent, Components> && ...)
  inline static void          SubmitJob(System * pSystem) noexcept
  {
    Job job
    {
      .mMask     { (Component2Index<typename Identity<Components>::Type, false>() | ... | 0u) },
      .mpArgsInfo{ & typeid(std::tuple<typename Identity<Components>::Type ...>) },
      .mpSystem  { pSystem },
      .mNumArgs  { sizeof ... (Components) },
    };

    // register componentTuple2function mapping

    sJobs.insert(job);
  }

  template<typename ... Components>
  requires (std::is_base_of_v<IComponent, Components> && ...)
  inline static auto          MaskToTuple(std::uint8_t mask)
  {
    std::uint8_t bit{};

    while (255 > bit << 1)
    {

    }

    return {};
  }

  // maybe parallelize?
  // REMOVE FOLD!!!
  template<typename ... Systems>
  requires (std::is_base_of_v<ISystem, Systems> && ...)
  inline static void          DispatchSystems() noexcept
  {
    for (auto const & [name, amr] : sAmrs)
    {
      for (auto const & job : sJobs)
      {
        (
          (
            (/*pSystem.mMask & job.mMaskSystem &&*/ amr.mMask & job.mMask) // job mask contains components
              ? (* reinterpret_cast<typename Identity<Systems>::Pointer>(job.mpSystem))()
              : void()
          )
        , ...);
      }
    }
  }
}
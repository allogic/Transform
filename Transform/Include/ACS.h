#pragma once

#include <Debug.h>

#include <utility>
#include <map>
#include <set>
#include <array>
#include <string>
#include <concepts>
#include <cstdint>
#include <functional>
#include <future>

#pragma warning(disable : 4834)

namespace ACS
{
  struct IComponent
  {

  };
  struct ISystem
  {
    virtual void operator () (float elapsedTime) = 0;
  };
  struct IActor
  {
    std::string mActorName{};

    IActor(std::string const & actorName)
      : mActorName{ actorName } {}
  };

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  struct Identity
  {
    using Type = C;
    using Pointer = C*;
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
  struct Job
  {
    std::uint64_t mMask     {};
    void *        mpFunction{};

    inline bool operator < (Job const & job) const noexcept { return mMask < job.mMask; }
  };

  inline static std::uint64_t                      sDistinctTypeCount{};
  inline static std::map<std::uint64_t, Key>       sTypeRegistry     {};
  inline static std::map<std::string, Actor>       sActors           {};
  inline static std::map<std::uint64_t, ISystem *> sSystems          {};
  inline static std::set<Job>                      sJobs             {};

  // obsolete -> make constexpr hash map, maybe xor will do..
  // try reverse range AND monoid
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

  inline constexpr static std::uint64_t                 csTypeDistinctCount{};
  inline constexpr static std::uint64_t                 csTypeMask{};
  inline constexpr static std::array<std::uint64_t, 64> csTypeHashMap{};

  template<std::uint64_t Limit, typename ... C>
  requires (std::is_base_of_v<IComponent, C>, ...)
  inline constexpr std::uint64_t Types2MaskXor()
  {
    return (typeid(C).hash_code() | ... | 0) % Limit;
  }

  template<typename C>
  requires std::is_base_of_v<IComponent, C>
  inline constexpr std::uint64_t Component2Flag() noexcept
  {
    std::uint64_t const maskBit{ Types2MaskXor<64, C>() };

    return 0;
  }

  inline constexpr std::uint64_t Mask2Component(std::uint64_t mask) noexcept
  {
    return 0;
  }

  template<typename A>
  requires std::is_base_of_v<IActor, A>
  inline static A *         CreateActor(std::string const & actorName) noexcept
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
  inline static C *         GetOrAttach(std::string const & actorName, Args && ... args) noexcept
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
  inline static S *         Register(Args && ... args) noexcept
  {
    auto const [sysemIt, _] { sSystems.emplace(typeid(S).hash_code(), new S{ std::forward<Args>(args) ... }) };
    return reinterpret_cast<S *>(sysemIt->second);
  }

  template<typename S, typename ... Args>
  requires std::is_base_of_v<ISystem, S>
  inline static void        UpdateSystem(Args && ... args) noexcept
  {
    for (const auto & [hash, pSystem] : sSystems)
      (* reinterpret_cast<S *>(pSystem))(std::forward<Args>(args) ...);
  }

  // maybe parallelize?
  // build queuing system..
  template<typename S, typename ... Components>
  requires std::is_base_of_v<ISystem, S> && (std::is_base_of_v<IComponent, typename Identity<Components>::Type>, ...)
  inline static void        SubmitJob(void(* pFunction)(S *, typename Identity<Components>::Pointer...)) noexcept
  {
    Job job
    {
      (Type2Index<typename Identity<Components>::Type, false>() | ... | 0),
      pFunction
    };

    sJobs.insert(job);
  }

  inline static void        Dispatch() noexcept
  {
    for (auto const & [name, actor] : sActors)
      for (auto const & job : sJobs)
      {
        if (job.mMask == actor.mMask)
        {
          //predicate
          //(
          //  reinterpret_cast<typename Identity<Components>::Pointer>
          //  (
          //    actor.mppRegisters[Type2Index<typename Identity<Components>::Type, true>()]
          //  ) ...
          //);
        }


        //MEASURE_BEGIN(CalcMask);
        //std::uint64_t const mask{ (Type2Index<typename Identity<Components>::Type, false>() | ... | 0) };
        //MEASURE_END(CalcMask);

        //if (actor.mMask & mask)
        //{
        //  //MEASURE_BEGIN(Predicate);
        //  //predicate
        //  //(
        //  //  reinterpret_cast<typename Identity<Components>::Pointer>
        //  //  (
        //  //    actor.mppRegisters[Type2Index<typename Identity<Components>::Type, true>()]
        //  //  ) ...
        //  //);
        //  //MEASURE_END(Predicate);
        //}
      }
  }

}
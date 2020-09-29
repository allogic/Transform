#include <ACS.h>

#include <iostream>

template<typename T>
struct vec2
{
  T x{};
  T y{};
};

template<typename T>
struct rect
{
  vec2<T> position{};
  vec2<T> size    {};
};

using r32     = float;
using r32vec2 = vec2<r32>;
using r32rect = rect<r32>;

struct MyActor : ACS::IActor
{
  MyActor(std::string const& actorName)
    : IActor(actorName) {}
};

struct Transform : ACS::IComponent
{
  r32vec2 position{};
  r32     rotation{};
  r32vec2 scale   {};

  Transform(std::string const& actorName)
    : IComponent(actorName) {}
};

struct Camera : ACS::IComponent
{
  Camera(std::string const& actorName)
    : IComponent(actorName) {}
};

struct MyGroupComponent : ACS::IComponent
{
  Transform* mpTransform{};
  Camera*    mpCamera   {};

  MyGroupComponent(std::string const& actorName)
    : mpTransform{ ACS::Attach<Transform>(actorName) }
    , mpCamera{ ACS::Attach<Camera>(actorName) }
    , IComponent(actorName) {}
};

struct MySystem : ACS::ISystem
{
  void operator () (float elapsedTime) override
  {
    //ACS::ForEach<Transform>([=](Transform* pTransform)
    //{
    //  pTransform->position.x += elapsedTime;
    //  pTransform->position.y += elapsedTime;
    //
    //  std::cout << pTransform->position.x << ' ' << pTransform->position.y << std::endl;
    //});

    ACS::ForEach<MyGroupComponent>([=](MyGroupComponent* pGrpComp)
    {
      std::cout << pGrpComp->mpCamera << ' ' << pGrpComp->mpTransform << std::endl;
    });
  }
};

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct Sandbox
  : olc::PixelGameEngine
{
  Sandbox() { sAppName = "Sandbox"; }

  bool OnUserCreate() override
  {
    auto a0{ ACS::Create<MyActor>("a") };
    auto s0{ ACS::Attach<Transform>("a") };

    auto a1{ ACS::Create<MyActor>("b") };
    auto s1{ ACS::Attach<MyGroupComponent>("b") };

    ACS::Register<MySystem>();

    return true;
  }
  bool OnUserUpdate(float elapsedTime) override
  {
    ACS::Update<MySystem>(elapsedTime);

    return true;
  }
};

int main()
{
  Sandbox sandbox{};

  if (sandbox.Construct(256, 240, 4, 4))
    sandbox.Start();

  return 0;
}
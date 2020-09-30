#pragma once

#include <ACS.h>
#include <Components.h>

#include <olcPixelGameEngine.h>

#include <random>

struct Manager : ACS::ISystem
{
  olc::PixelGameEngine* mpEngine{};

  Manager(olc::PixelGameEngine* engine)
    : mpEngine{ engine }
    , ISystem() {}

  void operator () (float elapsedTime) override
  {
    static bool initialized{};

    if (!initialized)
    {
      initialized = true;

      for (unsigned int i{}; i < 1000; i++)
      {
        std::string const actorName{ "actor" + std::to_string(i) };

        auto pActor{ ACS::Create<MyActor>(actorName) };
        auto pGizmos{ ACS::GetOrAttach<Gizmos>(actorName) };
        auto pTransform{ ACS::GetOrAttach<Transform>(actorName) };
        auto pQuad{ ACS::GetOrAttach<Quad>(actorName) };

        pTransform->mPosition = r32vec2{ std::rand() % mpEngine->ScreenWidth(), std::rand() % mpEngine->ScreenHeight() };
        pQuad->mSize = r32vec2{ std::rand() % 10, std::rand() % 10 };
      }
    }
  }
};

struct Renderer : ACS::ISystem
{
  olc::PixelGameEngine* mpEngine{};

  Renderer(olc::PixelGameEngine* engine)
    : mpEngine{ engine }
    , ISystem() {}

  void operator () (float elapsedTime) override
  {
    ACS::ForEach<Gizmos>([=](Gizmos* pGizmos)
    {
      olc::vi2d const position
      {
        static_cast<int>(pGizmos->mpTransform->mPosition.x + pGizmos->mpQuad->mPosition.x),
        static_cast<int>(pGizmos->mpTransform->mPosition.y + pGizmos->mpQuad->mPosition.y)
      };

      olc::vi2d const size
      {
        static_cast<int>(pGizmos->mpQuad->mSize.x),
        static_cast<int>(pGizmos->mpQuad->mSize.y),
      };

      mpEngine->DrawRect(position, size);
    });
  }
};
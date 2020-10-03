#pragma once

#include <ACS.h>
#include <Components.h>

#include <olcPixelGameEngine.h>

#include <random>

struct BlockManager : ACS::ISystem
{
  olc::PixelGameEngine * mpEngine{};
  olc::Sprite            mSpriteDirt{ 16, 16 };

  BlockManager(olc::PixelGameEngine * pEngine)
    : mpEngine{ pEngine }
  {
    mSpriteDirt.LoadFromFile("C:\\Users\\Michael\\Downloads\\Transform\\Sprites\\BlockDirt.bmp");

    for (unsigned int i{}; i < 40; i++)
      for (unsigned int j{}; j < 30; j++)
      {
        std::string const actorName{ "actor_" + std::to_string(i) + "_" + std::to_string(j) };

        auto pActor{ ACS::Create<MyActor>(actorName) };
        auto pBlockResource{ ACS::GetOrAttach<BlockResource>(actorName) };
        auto pDecal{ ACS::GetOrAttach<Decal>(actorName, &mSpriteDirt) };

        pBlockResource->mPosition = olc::vi2d{ (int)i * 16, (int)j * 16 };
        pBlockResource->mSize = olc::vi2d{ 1, 1 };
      }
  }

  void operator () (float elapsedTime) override
  {

  }
};

struct BlockBehaviour : ACS::ISystem
{
  void operator () (float elapsedTime) override
  {
    
  }
};

struct BlockStaticRenderer : ACS::ISystem
{
  olc::PixelGameEngine * mpEngine{};

  BlockStaticRenderer(olc::PixelGameEngine * pEngine)
    : mpEngine{ pEngine } {}

  void operator () (float elapsedTime) override
  {
    ACS::ForEach<BlockResource, Decal>([=](
      BlockResource * pBlockResource,
      Decal *         pDecal)
    {
      mpEngine->DrawDecal(pBlockResource->mPosition, &pDecal->mDecal, pBlockResource->mSize);
    });
  }
};

struct BlockDynamicRenderer : ACS::ISystem
{
  olc::PixelGameEngine* mpEngine{};

  BlockDynamicRenderer(olc::PixelGameEngine* pEngine)
    : mpEngine{ pEngine } {}

  void operator () (float elapsedTime) override
  {
    
  }
};
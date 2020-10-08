#pragma once

#include <Common.h>
#include <ACS.h>
#include <Components.h>

#include <olcPixelGameEngine.h>

#include <random>

struct BlockManager : ISystem
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

        auto pActor{ ACS::CreateActor<MyActor>(actorName) };
        auto pBlockResource{ ACS::GetOrAttachComponent<BlockResource>(actorName) };
        auto pDecal{ ACS::GetOrAttachComponent<Decal>(actorName, & mSpriteDirt) };

        pBlockResource->mPosition = olc::vi2d{ (int)i * 16, (int)j * 16 };
        pBlockResource->mSize = olc::vi2d{ 1, 1 };
      }
  }

  void operator () (float elapsedTime) override
  {

  }
};

struct BlockAI : ISystem
{
  void operator () (float elapsedTime) override
  {
    
  }
};

// remove static block handler since it's no longer
// part of the component system

struct BlockDynamicRenderer : ISystem
{
  olc::PixelGameEngine * mpEngine{};

  BlockDynamicRenderer(olc::PixelGameEngine * pEngine)
    : mpEngine{ pEngine } {}

  void operator () (float elapsedTime) override
  {
    ACS::SubmitJob<BlockDynamicRenderer, BlockResource, Decal>(this);
  }

  void operator () (BlockDynamicRenderer * pDynamicRenderer, BlockResource * pBlockResource, Decal * pDecal)
  {
    pDynamicRenderer->mpEngine->DrawDecal(pBlockResource->mPosition, &pDecal->mDecal, pBlockResource->mSize);
  }
};
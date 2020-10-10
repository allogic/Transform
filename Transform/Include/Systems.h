#pragma once

#include <Common.h>
#include <ACS.h>
#include <Components.h>

#include <olcPixelGameEngine.h>

// remove constructors use call operator with ctx instead

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
        auto pBlockResource{ ACS::GetOrAttachComponent<BlockDynamic>(actorName) };
        auto pDecal{ ACS::GetOrAttachComponent<Decal>(actorName, & mSpriteDirt) };

        pBlockResource->mPosition = olc::vi2d{ (int)i * 16, (int)j * 16 };
        pBlockResource->mSize = olc::vi2d{ 1, 1 };
      }
  }

  void operator () (float elapsedTime) override
  {

  }
  void operator () (BlockManager * pBlockManager, void * * ppRegisters)
  {
    std::cout << typeid(this).name() << std::endl;
  }
};

struct BlockAI : ISystem
{
  olc::PixelGameEngine * mpEngine{};

  BlockAI(olc::PixelGameEngine * pEngine)
    : mpEngine{ pEngine } {}

  void operator () (float elapsedTime) override
  {
    ACS::SubmitJob<BlockAI, Transform>(this);
  }
  void operator () (BlockAI * pBlockAi, void * * ppRegisters)
  {
    std::cout << typeid(this).name() << std::endl;
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
    ACS::SubmitJob<BlockDynamicRenderer, BlockDynamic, Decal>(this);
  }
  // maybe call templated operator and properly forward actors arguments
  void operator () (BlockDynamicRenderer * pDynamicRenderer, void * * ppRegisters)
  {
    std::cout << typeid(this).name() << std::endl;

    //pDynamicRenderer->mpEngine->DrawDecal
    //(
    //  pDynamicRenderer->pBlockResource->mPosition,
    //  & pDecal->mDecal,
    //  pBlockResource->mSize
    //);
  }
};
#pragma once

#include <Common.h>
#include <ACS.h>
#include <Components.h>

#include <olcPixelGameEngine.h>

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

        auto pActor{ ACS::GetOrCreateActor<MyActor>(actorName) };
        auto pBlockResource{ ACS::GetOrCreateComponent<BlockDynamic>(actorName) };
        auto pDecal{ ACS::GetOrCreateComponent<Decal>(actorName, & mSpriteDirt) };

        pBlockResource->mPosition = olc::vi2d{ (int)i * 16, (int)j * 16 };
        pBlockResource->mSize = olc::vi2d{ 1, 1 };
      }
  }

  void operator () (float elapsedTime) override
  {

  }
  void operator () ()
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
    //ACS::SubmitJob<
    //  BlockAI,
    //  Transform
    //>(this);
  }
  void operator () ()
  {
    std::cout << typeid(this).name() << std::endl;
  }
};

struct BlockDynamicRenderer : ISystem
{
  olc::PixelGameEngine * mpEngine{};

  BlockDynamicRenderer(olc::PixelGameEngine * pEngine)
    : mpEngine{ pEngine } {}

  void operator () (float elapsedTime) override
  {
    ACS::SubmitJob<
      BlockDynamicRenderer,
      BlockDynamic,
      Decal
    >(this);
  }
  void operator () ()
  {
    //std::cout << typeid(this).name() << std::endl;

    //pDynamicRenderer->mpEngine->DrawDecal
    //(
    //  pDynamicRenderer->pBlockResource->mPosition,
    //  & pDecal->mDecal,
    //  pBlockResource->mSize
    //);
  }
};
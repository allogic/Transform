#include <ACS.h>
#include <Actors.h>
#include <Components.h>
#include <Systems.h>
#include <Debug.h>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>

struct Sandbox
  : olc::PixelGameEngine
{
  Sandbox() { sAppName = "Sandbox"; }

  bool OnUserCreate() override
  {
    ACS::Register<BlockManager>(this);
    ACS::Register<BlockBehaviour>();
    ACS::Register<BlockStaticRenderer>(this);
    ACS::Register<BlockDynamicRenderer>(this);

    return true;
  }
  bool OnUserUpdate(float elapsedTime) override
  {
    MEASURE_BEGIN(BlockManager);
    ACS::Update<BlockManager>(elapsedTime);
    MEASURE_END(BlockManager);

    MEASURE_BEGIN(BlockBehaviour)
    ACS::Update<BlockBehaviour>(elapsedTime);
    MEASURE_END(BlockBehaviour);

    MEASURE_BEGIN(BlockStaticRenderer)
    ACS::Update<BlockStaticRenderer>(elapsedTime);
    MEASURE_END(BlockStaticRenderer);

    MEASURE_BEGIN(BlockDynamicRenderer)
    ACS::Update<BlockDynamicRenderer>(elapsedTime);
    MEASURE_END(BlockDynamicRenderer);

    return true;
  }
};

int main()
{
  Sandbox sandbox{};

  if (sandbox.Construct(640, 480, 2, 2))
    sandbox.Start();

  return 0;
}
#include <ACS.h>
#include <Actors.h>
#include <Components.h>
#include <Systems.h>
#include <Debug.h>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>
#include <bitset>

struct Sandbox
  : olc::PixelGameEngine
{
  Sandbox() { sAppName = "Sandbox"; }

  bool OnUserCreate() override
  {
    ACS::RegisterSystem<BlockManager>(this);
    ACS::RegisterSystem<BlockAI>();
    ACS::RegisterSystem<BlockDynamicRenderer>(this);

    return true;
  }
  bool OnUserUpdate(float elapsedTime) override
  {
    MEASURE_BEGIN(BlockManager);
    ACS::UpdateSystem<BlockManager>(elapsedTime);
    MEASURE_END(BlockManager);
    
    MEASURE_BEGIN(BlockAI);
    ACS::UpdateSystem<BlockAI>(elapsedTime);
    MEASURE_END(BlockAI);

    MEASURE_BEGIN(BlockDynamicRenderer);
    ACS::UpdateSystem<BlockDynamicRenderer>(elapsedTime);
    MEASURE_END(BlockDynamicRenderer);

    MEASURE_BEGIN(DispatchSystems);
    ACS::DispatchSystems<BlockManager, BlockAI, BlockDynamicRenderer>();
    MEASURE_END(DispatchSystems);

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
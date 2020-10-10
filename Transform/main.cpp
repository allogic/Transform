#include <Transform.h>

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
    ACS::RegisterSystems<
      BlockManager,
      BlockAI,
      BlockDynamicRenderer
    >(this);

    return true;
  }
  bool OnUserUpdate(float elapsedTime) override
  {
    MEASURE_BEGIN(UpdateSystems);
    ACS::UpdateSystems<
      BlockManager,
      BlockAI,
      BlockDynamicRenderer
    >(elapsedTime);
    MEASURE_END(UpdateSystems);

    MEASURE_BEGIN(DispatchSystems);
    ACS::DispatchSystems<
      BlockManager,
      BlockAI,
      BlockDynamicRenderer
    >();
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
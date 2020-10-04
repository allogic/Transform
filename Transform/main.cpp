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
    ACS::Register<BlockManager>(this);
    ACS::Register<BlockAI>();
    ACS::Register<BlockDynamicRenderer>(this);

    char mask{};

    mask |= (1 << 3);
    mask |= (1 << 2);
    mask |= (1 << 1);
    mask |= (1 << 0);

    std::cout << std::bitset<8>(mask) << std::endl;

    //mask -= 8;

    //std::cout << std::bitset<8>(mask) << std::endl;

    for (unsigned int i{ 8 }; i > 0; i >>= 1)
    {
      std::cout << std::bitset<8>(i) << std::endl;
    }

    return false;
  }
  bool OnUserUpdate(float elapsedTime) override
  {
    // submit update instructions

    //MEASURE_BEGIN(BlockManager);
    //ACS::Update<BlockManager>(elapsedTime);
    //MEASURE_END(BlockManager);
    //
    //MEASURE_BEGIN(BlockBehaviour);
    //ACS::Update<BlockBehaviour>(elapsedTime);
    //MEASURE_END(BlockBehaviour);

    //MEASURE_BEGIN(BlockDynamicRenderer);
    //ACS::UpdateSystem<BlockDynamicRenderer>(elapsedTime);
    //MEASURE_END(BlockDynamicRenderer);

    // execute update instructions

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
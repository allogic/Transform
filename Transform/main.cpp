#include <Types.h>
#include <ACS.h>
#include <Actors.h>
#include <Components.h>
#include <Systems.h>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>

struct Sandbox
  : olc::PixelGameEngine
{
  Sandbox() { sAppName = "Sandbox"; }

  bool OnUserCreate() override
  {
    ACS::Register<Manager>(this);
    ACS::Register<Renderer>(this);

    return true;
  }
  bool OnUserUpdate(float elapsedTime) override
  {
    ACS::Update<Manager>(elapsedTime);
    ACS::Update<Renderer>(elapsedTime);

    return true;
  }
};

int main()
{
  Sandbox sandbox{};

  if (sandbox.Construct(720, 480, 2, 2))
    sandbox.Start();

  return 0;
}
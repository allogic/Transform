#pragma once

#include <Common.h>

#include <olcPixelGameEngine.h>

#include <string>

#ifndef ACS_EXCLUDE
#include <ACS.h>
#endif

struct Transform : IComponent
{
  olc::vi2d mPosition{};
  float     mRotation{};
  olc::vi2d mScale   {};
};
using Component0 = Transform;

struct Camera : IComponent
{

};
using Component1 = Camera;

struct Decal : IComponent
{
  // use std::optional for non default init mem
  olc::Decal mDecal;

  Decal(olc::Sprite * pSprite)
    : mDecal{ pSprite } {}
};
using Component2 = Decal;

struct Material : IComponent
{
  
};
using Component3 = Material;

struct BlockStatic : IComponent
{
  olc::vi2d mPosition{};
  olc::vi2d mSize    {};
};
using Component4 = BlockStatic;

struct BlockDynamic : IComponent
{
  olc::vi2d mPosition{};
  olc::vi2d mSize    {};
  olc::vf2d mVelocity{};
};
using Component5 = BlockDynamic;
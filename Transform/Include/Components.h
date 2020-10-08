#pragma once

#include <Common.h>

#include <olcPixelGameEngine.h>

#include <string>

struct Transform : IComponent
{
  olc::vi2d mPosition{};
  float     mRotation{};
  olc::vi2d mScale   {};
};

struct Camera : IComponent
{

};

struct Decal : IComponent
{
  // use std::optional for non default init mem
  olc::Decal mDecal;

  Decal(olc::Sprite * pSprite)
    : mDecal{ pSprite } {}
};

struct Material : IComponent
{
  
};

struct IBlockStatic : IComponent
{
  olc::vi2d mPosition{};
  olc::vi2d mSize    {};
};

struct IBlockDynamic : IComponent
{
  olc::vi2d mPosition{};
  olc::vi2d mSize    {};
  olc::vf2d mVelocity{};
};

enum struct BlockStaticTypes : unsigned char
{
  Resource,
};

enum struct BlockDynamicTypes : unsigned char
{
  Something,
};

struct BlockResource : IBlockStatic
{
  float             mSpawnRate{};
  BlockDynamicTypes mBlockType{};
};
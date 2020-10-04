#pragma once

#include <ACS.h>

#include <olcPixelGameEngine.h>

#include <string>

struct Transform : ACS::IComponent
{
  olc::vi2d mPosition{};
  float     mRotation{};
  olc::vi2d mScale   {};
};

struct Camera : ACS::IComponent
{

};

struct Decal : ACS::IComponent
{
  // use std::optional for non default init mem
  olc::Decal mDecal;

  Decal(olc::Sprite * pSprite)
    : mDecal{ pSprite } {}
};

struct Material : ACS::IComponent
{
  
};

struct IBlockStatic : ACS::IComponent
{
  olc::vi2d mPosition{};
  olc::vi2d mSize    {};
};

struct IBlockDynamic : ACS::IComponent
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
#pragma once

#include <Types.h>
#include <ACS.h>

#include <string>

struct Transform : ACS::IComponent
{
  r32vec2 mPosition{};
  r32     mRotation{};
  r32vec2 mScale   {};

  Transform(std::string const& actorName)
    : IComponent(actorName) {}
};

struct Camera : ACS::IComponent
{
  Camera(std::string const& actorName)
    : IComponent(actorName) {}
};

struct Quad : ACS::IComponent
{
  r32vec2 mPosition{};
  r32vec2 mSize    {};

  Quad(std::string const& actorName)
    : IComponent(actorName) {}
};

struct Gizmos : ACS::IComponent
{
  Transform* mpTransform{};
  Quad*      mpQuad     {};

  Gizmos(std::string const& actorName)
    : mpTransform{ ACS::GetOrAttach<Transform>(actorName) }
    , mpQuad{ ACS::GetOrAttach<Quad>(actorName) }
    , IComponent(actorName) {}
};
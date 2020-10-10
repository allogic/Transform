#pragma once

#include <string>
#include <cstdint>

struct IComponent
{
  
};
struct ISystem
{
  virtual void operator () (float elapsedTime) = 0;
};
struct IActor
{
  std::string const mActorName{};

  IActor(std::string const& actorName)
    : mActorName{ actorName } {}
};
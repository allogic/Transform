#pragma once

#include <string>

struct IComponent
{

};
struct ISystem
{
  virtual void operator () (float elapsedTime) = 0;
};
struct IActor
{
  std::string mActorName{};

  IActor(std::string const& actorName)
    : mActorName{ actorName } {}
};
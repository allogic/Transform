#pragma once

#include <string>
#include <cstdint>

struct IComponent
{
  
};
struct ISystem
{
  virtual void operator () (float elapsedTime) = 0;
  
  template<typename ... Components>
  friend void operator << (const ISystem * pSystem, std::tuple<Components...> && components);
};
struct IActor
{
  std::string const mActorName{};

  IActor(std::string const& actorName)
    : mActorName{ actorName } {}
};
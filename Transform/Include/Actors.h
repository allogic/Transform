#pragma once

#include <ACS.h>

#include <string>

struct MyActor : ACS::IActor
{
  MyActor(std::string const& actorName)
    : IActor(actorName) {}
};
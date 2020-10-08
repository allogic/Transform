#pragma once

#include <Common.h>

#include <string>

struct MyActor : IActor
{
  MyActor(std::string const & actorName)
    : IActor(actorName) {}
};
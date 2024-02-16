#pragma once


#ifndef ImGINE_FLECS
#define ImGINE_FLECS



#define FLECS_CPP
#include <flecs.h>
#include <iostream>


void CheckFlecs()
{
    flecs::world world;
    auto e = world.entity("Bob");
    e.is_alive(); // true!

    std::cout << "Entity name: " << e.name() << std::endl;

    e.destruct();
    std::cout << e.is_alive() << std::endl; // false!

}



#endif
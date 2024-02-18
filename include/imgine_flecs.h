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

    auto e = world.entity();

    //// Add a component. This creates the component in the ECS storage, but does not
    //// assign it with a value.
    //e.add<Velocity>();

    //// Set the value for the Position & Velocity components. A component will be
    //// added if the entity doesn't have it yet.
    //e.set<Position>({ 10, 20 })
    //    .set<Velocity>({ 1, 2 });

    //// Get a component
    //const Position* p = e.get<Position>();

    //// Remove component
    //e.remove<Position>();


    e.destruct();
    std::cout << e.is_alive() << std::endl; // false!



}



#endif
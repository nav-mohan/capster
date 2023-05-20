#include "geoutil.hpp"
#include <iostream>

Point inside1 = {43.4887976,-80.9314785};
Point inside2 = {43.6078687,-80.3194287};
Point inside3 = {43.4198664,-82.2591451};
Point inside4 = {42.4473599,-81.0100467};
Point inside5 = {43.3415702,-80.2241216};
Point inside6 = {43.9881932,-80.7905499};
Point inside7 = {43.7335073,-81.6228752};
Point inside8 = {43.0441965,-81.5402841};

std::string polygon = "43.3642064,-79.1335314 44.6788340,-80.8922636 43.4752886,-82.7747958 42.0776116,-81.0160261";

int main()
{
    GeoUtil g;
    g.set_alertArea(polygon);
    g.set_station(inside1); std::cout << g.is_inside() << std::endl;
    g.set_station(inside2); std::cout << g.is_inside() << std::endl;
    g.set_station(inside3); std::cout << g.is_inside() << std::endl;
    g.set_station(inside4); std::cout << g.is_inside() << std::endl;
    g.set_station(inside5); std::cout << g.is_inside() << std::endl;
    g.set_station(inside6); std::cout << g.is_inside() << std::endl;
    g.set_station(inside7); std::cout << g.is_inside() << std::endl;
    g.set_station(inside8); std::cout << g.is_inside() << std::endl;
}
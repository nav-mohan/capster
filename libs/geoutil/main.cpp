// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon/2922778#2922778
// https://wrfranklin.org/Research/Short_Notes/pnpoly.html
#include <iostream>
#include <stdlib.h>
#include <vector>

#define EPSILON 0.1
#define LARGE_FLOAT 99999.9f

using namespace std;

struct Point {
	float x, y;
};

struct line {
	Point p1, p2;
};

// checks if (x2,y2) lies on <-(x1,y1)----(x3,y3)->
bool is_online(float x2, float y2, float x1, float y1, float x3, float y3)
{
    if((x2 > x1) && (x2 > x3)) return false; // (x2,y2) is to the right of <-(x1,y1)---(x3,y3)->
    if((x2 < x1) && (x2 < x3)) return false; // (x2,y2) is to the left of <-(x1,y1)---(x3,y3)->
    if((y2 > y1) && (y2 > y3)) return false; // (x2,y2) is above <-(x1,y1)---(x3,y3)->
    if((y2 < y1) && (y2 < y3)) return false; // (x2,y2) is below <-(x1,y1)---(x3,y3)->
    
    float d = ((x2-x1) * (y3-y1) - (x3-x1) * (y2-y1));
    if (fabs(d) < EPSILON*EPSILON) {std::cout << "CLOSE\n" ; return true;}
    
    return false;
}

// check if two vectors intersect
// v1 is a ray drawn from the station's coordinates to infinte x
// v2 is a side of the polygon
bool are_intersecting(
    float v1x1, float v1y1, float v1x2, float v1y2, // in our case v1x2 = LARGE_FLOAT and v1y2 = v1y1
    float v2x1, float v2y1, float v2x2, float v2y2
)
{
    // q1) does v2 intersect the infintely long v1?
    // derive the a1*x + b1*y + c1 = 0 form of v1 - this is an infinitely long v1
    float a1 = v1y2 - v1y1; // = 0 in our case
    float b1 = v1x1 - v1x2; // ~ LARGE_FLOAT in our case
    float c1 = (v1x2 * v1y1) - (v1x1 * v1y2); // ~(LARGE_FLOAT * v1y1) in our case

    // plug v2's both endpoints into the above a1x+b1y+c1=0 equation
    float d1 = a1*v2x1 + b1*v2y1 + c1;
    float d2 = a1*v2x2 + b1*v2y2 + c1;
    
    if (d1 > 0 && d2 > 0) return 0; // both endpoints of v2 lie on the same side of infinite-v1
    if (d1 < 0 && d2 < 0) return 0;// both endpoints of v2 lie on the same (other) side of infinite-v1

    // q2) does v1 intersect the infintely long v2?
    float a2 = v2y2 - v2y1;
    float b2 = v2x1 - v2x2;
    float c2 = (v2x2 * v2y1) - (v2x1 * v2y2);

    // substitute v1's (x,y) value into a2*x+b2*y+c2=0
    d1 = a2*v1x1 + b2*v1y1 + c2;
    d2 = a2*v1x2 + b2*v1y2 + c2;

    if (d1 > 0 && d2 > 0) return 0; // both endpoints of v1 lie on the same side of infinite-v2
    if (d1 < 0 && d2 < 0) return 0;// both endpoints of v1 lie on the same (other) side of infinite-v2

    // if we get here it means v1 and v2 are parallel or overlapping i.e d1 == d2 == 0;
    // area of a triangle test to check if they are collinear i.e parallel or overlapping
    if (fabs(a1*b2 - a2*b1) < EPSILON*EPSILON) return 0;

    // else they are intersecting
    return 1;
}

bool is_inside(std::vector<Point> polygon, Point station)
{
    float sx = station.x;
    float sy = station.y;
    uint32_t countIntersections = 0;
    for(int i = 0; i < polygon.size()-1; i++)
    {
        float x1 = polygon.at(i).x;
        float y1 = polygon.at(i).y;
        float x2 = polygon.at(i+1).x;
        float y2 = polygon.at(i+1).y;
        
        if(is_online(sx,sy,x1,y1,x2,y2)) return true;
        
        if(are_intersecting(sx,sy,LARGE_FLOAT,sy,x1,y1,x2,y2))
            countIntersections++;
    }
    std::cout << "INTERSECTION = " << countIntersections << std::endl;
    if(countIntersections & 1)
        return true;
    return false;
}

// Driver code
int main()
{
    Point right = { 43.3642064,-79.1335314};
    Point top = {44.6788340,-80.8922636};
    Point left = {43.4752886,-82.7747958};
    Point bottom = { 42.0776116,-81.0160261};

    Point inside1 = {43.4887976,-80.9314785};
    Point inside2 = {43.6078687,-80.3194287};
    Point inside3 = {43.4198664,-82.2591451};
    Point inside4 = {42.4473599,-81.0100467};
    Point inside5 = {43.3415702,-80.2241216};
    Point inside6 = {43.9881932,-80.7905499};
    Point inside7 = {43.7335073,-81.6228752};
    Point inside8 = {43.0441965,-81.5402841};

    Point outside1 = {42.6106851,-82.1448075};
    Point outside2 = {44.1931755,-79.4681835};
    Point outside3 = {44.3398647,-82.3434016};
    Point outside4 = {43.0138468,-82.9076764};
    Point outside5 = {42.0099984,-79.7784034};
    Point outside6 = {42.6991497,-79.1669777};
    Point outside7 = {44.1270751,-79.1818829};
    Point outside8 = {44.5949245,-79.9361914};


    Point closeOutside1 = {42.9021228,-82.0528813};
    Point closeOutside2 = {44.1521302,-80.1321386};
    Point closeOutside3 = {43.7375223,-79.5829655};
    Point closeOutside4 = {43.2838281,-79.1893612};
    Point closeOutside5 = {42.1289159,-80.8798320};
    Point closeOutside6 = {43.3486447,-82.6157921};
    Point closeOutside7 = {42.6318034,-81.7638825};
    Point closeOutside8 = {42.1938773,-81.2089203};
    Point closeOutside9 = {44.6634618,-80.9424389};


    Point closeInside1 = {42.9046311,-82.0491580};
    Point closeInside2 = {44.6433972,-80.8752946};
    Point closeInside3 = {43.8490328,-79.8329565};
    Point closeInside4 = {43.2130893,-79.4030655};
    Point closeInside5 = {42.1326134,-80.9707036};
    Point closeInside6 = {42.9788279,-82.0664942};
    Point closeInside7 = {43.3505598,-82.5507047};
    Point closeInside8 = {42.4953540,-80.4780292};

    std::vector<Point> polygon;
    polygon.push_back(top);
    polygon.push_back(left);
    polygon.push_back(bottom);
    polygon.push_back(right);
    polygon.push_back(top);

    std::cout << is_inside(polygon,inside1) << std::endl;
    std::cout << is_inside(polygon,inside2) << std::endl;
    std::cout << is_inside(polygon,inside3) << std::endl;
    std::cout << is_inside(polygon,inside4) << std::endl;
    std::cout << is_inside(polygon,inside5) << std::endl;
    
    // std::cout << is_inside(polygon,outside1) << std::endl;
    // std::cout << is_inside(polygon,outside2) << std::endl;
    // std::cout << is_inside(polygon,outside3) << std::endl;
    // std::cout << is_inside(polygon,outside4) << std::endl;
    // std::cout << is_inside(polygon,outside5) << std::endl;
    // std::cout << is_inside(polygon,outside6) << std::endl;
    // std::cout << is_inside(polygon,outside7) << std::endl;
    // std::cout << is_inside(polygon,outside8) << std::endl;
    
    // std::cout << is_inside(polygon,closeOutside1) << std::endl;
    // std::cout << is_inside(polygon,closeOutside2) << std::endl;
    // std::cout << is_inside(polygon,closeOutside3) << std::endl;
    // std::cout << is_inside(polygon,closeOutside4) << std::endl;
    // std::cout << is_inside(polygon,closeOutside5) << std::endl;
    // std::cout << is_inside(polygon,closeOutside6) << std::endl;
    // std::cout << is_inside(polygon,closeOutside7) << std::endl;
    // std::cout << is_inside(polygon,closeOutside8) << std::endl;
    // std::cout << is_inside(polygon,closeOutside9) << std::endl;

    // std::cout << is_inside(polygon,closeInside1) << std::endl;
    // std::cout << is_inside(polygon,closeInside2) << std::endl;
    // std::cout << is_inside(polygon,closeInside3) << std::endl;
    // std::cout << is_inside(polygon,closeInside4) << std::endl;
    // std::cout << is_inside(polygon,closeInside5) << std::endl;
    // std::cout << is_inside(polygon,closeInside6) << std::endl;
    // std::cout << is_inside(polygon,closeInside7) << std::endl;
    // std::cout << is_inside(polygon,closeInside8) << std::endl;



    // std::cout << is_online(qr,q) << std::endl;
    // std::cout << is_online(qr,r) << std::endl;
	
    // std::cout << is_online(rs,r) << std::endl;
    // std::cout << is_online(rs,s) << std::endl;

    // std::cout << is_online(ps,p) << std::endl;
    // std::cout << is_online(ps,s) << std::endl;

    return 0;
}

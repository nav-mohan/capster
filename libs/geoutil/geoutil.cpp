#include "geoutil.hpp"

bool GeoUtil::is_online(float x2, float y2, float x1, float y1, float x3, float y3)
{
    if((x2 > x1) && (x2 > x3)) return false; 
    if((x2 < x1) && (x2 < x3)) return false; 
    if((y2 > y1) && (y2 > y3)) return false; 
    if((y2 < y1) && (y2 < y3)) return false; 
    
    float d = ((x2-x1) * (y3-y1) - (x3-x1) * (y2-y1));
    if (fabs(d) < EPSILON*EPSILON) {std::cout << "CLOSE\n" ; return true;}
    
    return false;
}

bool GeoUtil::are_intersecting(
    float v1x1, float v1y1, float v1x2, float v1y2,
    float v2x1, float v2y1, float v2x2, float v2y2
)
{
    float a1 = v1y2 - v1y1; 
    float b1 = v1x1 - v1x2; 
    float c1 = (v1x2 * v1y1) - (v1x1 * v1y2);

    float d1 = a1*v2x1 + b1*v2y1 + c1;
    float d2 = a1*v2x2 + b1*v2y2 + c1;
    
    if (d1 > 0 && d2 > 0) return 0; 
    if (d1 < 0 && d2 < 0) return 0;

    float a2 = v2y2 - v2y1;
    float b2 = v2x1 - v2x2;
    float c2 = (v2x2 * v2y1) - (v2x1 * v2y2);

    d1 = a2*v1x1 + b2*v1y1 + c2;
    d2 = a2*v1x2 + b2*v1y2 + c2;

    if (d1 > 0 && d2 > 0) return 0; 
    if (d1 < 0 && d2 < 0) return 0;

    if (fabs(a1*b2 - a2*b1) < EPSILON*EPSILON) return 0;

    return 1;
}

bool GeoUtil::is_inside()
{
    float sx = station_.x_;
    float sy = station_.y_;
    uint32_t countIntersections = 0;
    for(int i = 0; i < alertArea_.size()-1; i++)
    {
        float x1 = alertArea_.at(i).x_;
        float y1 = alertArea_.at(i).y_;
        float x2 = alertArea_.at(i+1).x_;
        float y2 = alertArea_.at(i+1).y_;
        
        if(is_online(sx,sy,x1,y1,x2,y2)) return true;
        
        if(are_intersecting(sx,sy,LARGE_FLOAT,sy,x1,y1,x2,y2))
            countIntersections++;
    }
    std::cout << "INTERSECTION = " << countIntersections << std::endl;
    if(countIntersections & 1)
        return true;
    return false;
}

void GeoUtil::set_alertArea(std::string areaPolygon)
{
    alertArea_.clear();
    std::regex re("\\s+");
    std::regex_token_iterator<std::string::const_iterator> iter(areaPolygon.begin(),areaPolygon.end(),re,-1);
    std::regex_token_iterator<std::string::const_iterator> end;
    while(iter != end)
    {
        alertArea_.push_back(Point(iter->str()));
        iter++;
    }
    if(alertArea_.at(alertArea_.size()-1) != alertArea_.at(0))
    {
        std::cout << "Looping\n";
        alertArea_.push_back(alertArea_.at(0));// to close the polygon loop 
    }
}
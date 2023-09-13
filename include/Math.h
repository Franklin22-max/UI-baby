#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <cmath>



#define PI 3.1415926535897932384626433832795

inline double _fabs(double num)
{
    return (num < 0)? -num : num;
}

inline double deg_to_rad(double deg)
{
    return deg * PI / 180;
}

inline double rad_to_deg(double rad)
{
    return rad * 180 / PI;
}




#endif // MATH_H_INCLUDED

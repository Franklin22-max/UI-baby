#ifndef SURFACEHANDLER_H_INCLUDED
#define SURFACEHANDLER_H_INCLUDED

#include "Shapes.h"




inline void putPixel(SDL_Surface* __surface, int x, int y, SDL_Color color)
{
    if(x < __surface->w && y < __surface->h && x >= 0 && y >= 0)
    {
        if(SDL_MUSTLOCK(__surface)){SDL_LockSurface( __surface );}
        UINT32* pixel = (UINT32*)__surface->pixels;
        pixel[y * __surface->w + x] = SDL_MapRGBA(__surface->format,color.r,color.g,color.b,color.a);
        if(SDL_MUSTLOCK(__surface)){SDL_UnlockSurface( __surface );}
    }
}



inline SDL_Surface* LOAD_IMAGE(const std::string path)
{
    SDL_Surface* s = IMG_Load(path.c_str());
    if(s)
    {
        SDL_Surface* img = SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_RGBA8888,0);
        SDL_FreeSurface(s);
        return img;
    }
    else return NULL;
}

/** \brief  trim the edges of surface and make it roundish
    *
    * \param  the source
    * \param  how deep trimming should be , should be b/w (0 - 100)
    * \ return a trimmed surface or null on failure
    * \ note  should manually free returned surface
    */
inline SDL_Surface* make_round_edges(SDL_Surface* src, float percentage)
{
    int x,y;
    x = src->w/2;
    y = src->h/2;
    float r = sqrt(x*x + y*y)  * (percentage / 100);
    int d = r*2;

    SDL_Surface* dest = SDL_CreateRGBSurfaceWithFormat(0, std::min(d,src->w), std::min(d,src->h), 0,SDL_PIXELFORMAT_RGBA8888);

    int __h = (src->h - std::min(d,src->h))/2.f;
    int __w = (src->w - std::min(d,src->w))/2.f;

    if(dest)
    {
        uint32_t* dest_ptr = (uint32_t*)dest->pixels;
        uint32_t* src_ptr = (uint32_t*)src->pixels;

        for(int i = 0; i < src->h; i++) // y - axis
        {
            for(int j = 0; j < src->w; j++)// x - axis
            {
                if(sqrt((i - y)*(i - y)  + (j - x)*(j - x)) < r )
                    dest_ptr[(i - __h) * dest->w + (j - __w)] = src_ptr[i * src->w + j];
            }
        }
    }
    return dest;
}


inline void fill_circle(int x, int y, int r,  SDL_Surface* __surface, SDL_Color color)
{
    if( x + r > 0 && x - r < __surface->w && r > 0 && y + r > 0 && y - r < __surface->h)
    {
        int d = r + r;
        int count = 0;
        int _y = y - r;

        for(int i = 0; i < d; i++)// y-axis
        {
            for(int j = 0; j < r; j++)
            {
                // check if point is inside circle
                if(fabs(((x + j) - x)*((x + j) - x) + ((_y + i) - y)*((_y + i) - y)) <= r*r)
                {
                    // fill right hand side on the x-axis
                    putPixel(__surface,((x + j)), _y + i, color);
                    count++;
                }
                else
                    break;
            }
            // fill the left hand side on the x-axis
            for(int j = 0; j < count; j++)
                putPixel(__surface,(x - j), _y + i, color);
            count = 0;
        }
    }
}

inline void fill_polygon(std::vector<vec2d> vertices, SDL_Surface* __surface, SDL_Color color)
{
    int x = INFINITE,y = INFINITE,x2 = 0,y2 = 0;
    for(int i = 0; i < vertices.size(); i++)
    {
        y = std::min(y,vertices[i].y);
        x = std::min(x,vertices[i].x);
        y2 = std::max(y2,vertices[i].y);
        x2 = std::max(x2,vertices[i].x);
    }

    if( x2 > 0 && x < __surface->w && y2 > 0 && y < __surface->h)
    {
        double h = y2-y, w = x2 - x;
        for(int i = 0; i < h; i++)// y-axis
        {
            for(int j = 0; j < w; j++)// x-axis
            {
                vec2d p = { j + x, i + y};

                // check if point is within polygon
                if(isPointInsidePolygon(p,vertices))
                    putPixel(__surface, p.x, p.y, color);
            }
        }
    }
}

inline void fill_triangle(vec2d p1, vec2d p2, vec2d p3,  SDL_Surface* __surface, SDL_Color color)
{
    double d1 = sqrt((p1.x - p2.x * 1.f)*(p1.x - p2.x * 1.f) + (p1.y - p2.y * 1.f)*(p1.y - p2.y * 1.f));
    double d2 = sqrt((p2.x - p3.x * 1.f)*(p2.x - p3.x * 1.f) + (p2.y - p3.y * 1.f)*(p2.y - p3.y * 1.f));
    double d3 = sqrt((p3.x - p1.x * 1.f)*(p3.x - p1.x * 1.f) + (p3.y - p1.y * 1.f)*(p3.y - p1.y * 1.f));

    vec2df m1 = {(p1.x - p2.x * 1.f)/d1, (p1.y - p2.y * 1.f)/d1};
    vec2df m2 = {(p2.x - p3.x * 1.f)/d2, (p2.y - p3.y * 1.f)/d2};
    vec2df m3 = {(p3.x - p1.x * 1.f)/d3, (p3.y - p1.y * 1.f)/d3};

    double max = std::max(d3,d2);
    double min = std::min(d3,d2);

    double sub3 = (d3 != max)? max/min : 1;
    double sub2 = (d2 != max)? max/min: 1;

    vec2df _p1,_p2;
    for(double i = min; i > 0; i--)
    {
        d3 -= sub2;
        d2 -= sub3;

        _p1 = {p3.x - m3.x * d3, p3.y - m3.y  * d3};
        _p2 = {p3.x + m2.x * d2, p3.y + m2.y  * d2};

        double d1 = sqrt((_p1.x - _p2.x * 1.f)*(_p1.x - _p2.x * 1.f) + (_p1.y - _p2.y * 1.f)*(_p1.y - _p2.y * 1.f));
        if(d1 != 0) vec2df m1 = {(_p1.x - _p2.x * 1.f)/d1, (_p1.y - _p2.y * 1.f)/d1};

        for(double j = 0; j < d1; j++)
        {
            vec2df p = {_p2.x + m1.x * j, _p2.y + m1.y * j};
            putPixel(__surface, p.x, p.y, color);
        }
    }
}



#endif // SURFACEHANDLER_H_INCLUDE

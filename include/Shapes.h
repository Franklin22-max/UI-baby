#ifndef SHAPES_H_INCLUDED
#define SHAPES_H_INCLUDED

#include "Math.h"
#include "common.h"


class shapes
{
protected:
    int x, y;
public:
    shapes(int x, int y): x(x), y(y) {};
    virtual double Area() = 0;
    virtual double Perimeter() = 0;
    virtual double get(std::string key) = 0;
    virtual void set(std::string key, double value) = 0;
};


class circle : shapes
{
    double radius;
public:
    circle(int x, int y, double radius)
    : shapes(x,y), radius(radius) {}

    double Area()
    {
        return PI * radius * radius;
    }

    double Perimeter()
    {
        return 2 * PI * radius;
    }

    double get(std::string key)
    {
        if(key == "x")
            return x;
        else if(key == "y")
            return y;
        else if(key == "r" || key == "radius" || key == "R")
            return radius;
    }

    void set(std::string key, double value)
    {
        if(key == "x")
            x = value;
        else if(key == "y")
            y = value;
        else if(key == "r" || key == "radius" || key == "R")
            radius = value;
    }
};



class polygon : shapes
{
    int sides;
    double  length;
    std::vector<vec2d> vertices;
public:
    polygon(int x, int y, int sides, double length)
    :shapes(x,y), sides(sides), length(length)
    {
        set_vertices();
    }

    void set_vertices()
    {
        if(vertices.size() > 0)
            vertices.erase(std::begin(vertices), std::end(vertices));

        // using a circle as a gTESTde
        double angle = 360 / sides;
        double radTESTs = sqrt( apothem() * apothem() + (length/2) * (length/2));

        for(int i = 0; i < sides; i++)
        {
            vertices[i].x = x + cos(deg_to_rad(angle * i)) * radTESTs;
            vertices[i].y = y + sin(deg_to_rad(angle * i)) * radTESTs;
        }
    }

    std::vector<vec2d> get_vertices()
    {
        return vertices;
    }

    double apothem()
    {
        return length /(2 * tan(deg_to_rad(180/sides)));
    }

    double Perimeter()
    {
        return length * sides;
    }

    double area()
    {
        return Perimeter() * apothem();
    }

    double get(std::string key)
    {
        if(key == "x")
            return x;
        else if(key == "y")
            return y;
        else if(key == "a" || key == "apothem" || key == "A")
            return apothem();
        else if(key == "length" || key == "l")
            return length;
    }

    void set(std::string key, double value)
    {
        if(key == "x")
            x = value;
        else if(key == "y")
            y = value;
        else if(key == "length" || key == "l")
            length = value;
    }
};






inline bool is_point_inside_rect(const vec2d& p, const SDL_Rect& rect)
{
    SDL_Point _p = {p.x, p.y};
    return SDL_PointInRect(&_p,&rect);
}

inline bool is_line_intersecting(const Line& A, const Line& B)
{
    float h = (B.tail.x - B.head.x) * (A.head.y - A.tail.y) - (A.head.x - A.tail.x) * (B.tail.y - B.head.y);
    float t1 = ((B.head.y - B.tail.y) * (A.head.x - B.head.x) + (B.tail.x - B.head.x) * (A.head.y - B.head.y)) / h;
    float t2 = ((A.head.y - A.tail.y) * (A.head.x - B.head.x) + (A.tail.x - A.head.x) * (A.head.y - B.head.y)) / h;

    return (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f);
}


inline bool isPointInsidePolygon(vec2d p,std::vector<vec2d> vertices)
{
    for (uint16_t i = 0; i < vertices.size(); i++)
    {
        uint16_t j = (i + 1) % vertices.size();
        vec2d m = {-(vertices[j].y - vertices[i].y) , vertices[j].x - vertices[i].x};
        //double distance = sqrtf(m.y * m.y + m.x * m.x);
        //m = {m.y/distance, m.x/distance};

        float min = INFINITY , max = -INFINITY;
        for (uint16_t i = 0; i < vertices.size(); i++)
        {
            float dot = (vertices[i].x * m.x + vertices[i].y * m.y);
            min = std::min(min, dot);
            max = std::max(max, dot);
        }

        float p_dot = (p.x * m.x + p.y * m.y);

        if(!(max >= p_dot && p_dot >= min))
        return false;
    }
    return true;
}

/** \brief
    * \param A first line
    * \param B second line
    * \sa  is_line_intersecting()
    */

inline vec2d get_line_intersection_point(const Line& A, const Line& B)
{

}



inline SDL_Rect get_intersection_rect(const SDL_Rect& A, const SDL_Rect& B)
{
    SDL_Rect R;

    if(A.x <= B.x && A.x+A.w > B.x)
    {
        R.x = B.x;
        R.w = (A.x+A.w > B.x+B.w)? (B.x+B.w) - R.x  :   (A.x+A.w) - R.x;
    }
    else if(A.x >= B.x && A.x <= B.x+B.w)
    {
        R.x = A.x;
        R.w = (A.x+A.w > B.x+B.w)? (B.x+B.w) - R.x  :   (A.x+A.w) - R.x;
    }
    else return {0,0,0,0};

    if(A.y <= B.y && A.y+A.h > B.y)
    {
        R.y = B.y;
        R.h = (A.y+A.h > B.y+B.h)? (B.y+B.h) - R.y  :   (A.y+A.h) - R.y;
        return R;
    }
    else if(A.y >= B.y && A.y <= B.y+B.h)
    {
        R.y = A.y;
        R.h = (A.y+A.h > B.y+B.h)? (B.y+B.h) - R.y  :   (A.y+A.h) - R.y;
        return R;
    }
    else return {0,0,0,0};
}


inline Line get_intersection_line(Line line, const SDL_Rect& rect)
{
    Line R = {0,0,0,0};

    // make sure that the head of the line is at the left side
    if(line.head.x > line.tail.x)
        std::swap(line.head, line.tail);

    // if line is horizontal
    if(line.head.y == line.tail.y && (line.head.y > rect.y && line.head.y < rect.y + rect.h))
    {
        if(line.head.x <= rect.x && line.tail.x > rect.x)
        {
            R.head.y = R.tail.y = line.head.y;
            R.head.x = rect.x;
            R.tail.x = (line.tail.x > rect.x+rect.w)? rect.x+rect.w : line.tail.x;
            return R;
        }
        else if(line.head.x > rect.x && line.head.x < rect.x+rect.w)
        {
            R.head.y = R.tail.y = line.head.y;
            R.head.x = line.head.x;
            R.tail.x = (line.tail.x > rect.x+rect.w)? rect.x+rect.w : line.tail.x;
            return R;
        }
        else return R;
    }
    // if line is vertical
    else if(line.head.x == line.tail.x && (line.head.x > rect.x && line.head.x < rect.x + rect.w))
    {
        int min_y = std::min(line.head.y,line.tail.y);
        int max_y = std::max(line.head.y,line.tail.y);

        if(min_y <= rect.y && max_y > rect.y)
        {
            R.head.x = R.tail.x = line.head.x;
            R.head.y = rect.y;
            R.tail.y = (max_y > rect.y+rect.h)? rect.y+rect.h : max_y;
            return R;
        }
        else if(min_y > rect.y && min_y < rect.y+rect.h)
        {
            R.head.x = R.tail.x = line.head.x;
            R.head.y = min_y;
            R.tail.y = (max_y > rect.y+rect.h)? rect.y+rect.h : max_y;
            return R;
        }
        else {return R;}
    }
    // if line is diagonal
    else
    {
        bool head_in_rect = is_point_inside_rect(line.head,rect);
        bool tail_in_rect = is_point_inside_rect(line.tail,rect);

        // if the entire line is within rectangle
        if( head_in_rect && tail_in_rect)
            return line;
        else
        {
            bool head_set = false;
            if(head_in_rect)
            {
                R.head = line.head;
                head_set = true;
            }
            else if(tail_in_rect)
            {
                R.head = line.tail;
                head_set = true;
            }

            double slope = ((line.tail.y - line.head.y)*1.0) / (line.tail.x - line.head.x);
            // left side
            if(is_line_intersecting(line, {rect.x, rect.y, rect.x, rect.y+rect.h}))
            {
                if (head_set)
                {
                    R.tail.x = rect.x;
                    R.tail.y = -slope * (line.tail.x - rect.x) + line.tail.y;
                    return R;
                }
                else
                {
                    R.head.x = rect.x;
                    R.head.y = -slope * (line.tail.x - rect.x) + line.tail.y;
                    head_set = true;
                }

            }
            // right
            if(is_line_intersecting(line, {rect.x+rect.w, rect.y, rect.x+rect.w, rect.y+rect.h}))
            {
                if (head_set)
                {
                    R.tail.x = (rect.w + rect.x); 
                    R.tail.y = slope * ((rect.w + rect.x) - line.head.x) + line.head.y;
                    return R;
                }
                else
                {
                    R.head.x = (rect.w + rect.x);
                    R.head.y = slope * ((rect.w + rect.x) - line.head.x) + line.head.y;
                    head_set = true;
                }
            }
            // up
            if(is_line_intersecting(line, {rect.x, rect.y, rect.x+rect.w, rect.y}))
            {
                if(head_set)
                {
                    R.tail.x = line.head.x + (rect.y - line.head.y) / slope;
                    R.tail.y = rect.y;
                    return R;
                }
                else
                {
                    R.head.x = line.head.x + (rect.y - line.head.y) / slope;
                    R.head.y = rect.y;
                    head_set = true;
                }
            }
            // bottom
            if(is_line_intersecting(line, {rect.x, rect.y+rect.h, rect.x+rect.w, rect.y+rect.h}))
            {
                if (head_set)
                {
                    R.tail.x = line.head.x + ((rect.y + rect.h) - line.head.y) / slope;
                    R.tail.y = rect.y + rect.h;
                    return R;
                }
                else
                {
                    R.head.x = line.head.x + ((rect.y + rect.h) - line.head.y) / slope;
                    R.head.y = rect.y + rect.h;
                    head_set = true;
                }
            }

            R = {0,0,0,0};
            return R;
        }
    }
}


#endif // SHAPES_H_INCLUDED

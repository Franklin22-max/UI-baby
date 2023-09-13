#ifndef component_H_INCLUDED
#define component_H_INCLUDED

#include "Administrator.h"


class component
{
protected:
    vec2d pos;
    vec2d size;
    View* view;
    std::string id;
    bool is_active;
    bool in_focus;
    bool on_hover;

public:

    virtual ~component() = default;

    virtual void Disable() = 0;
    virtual void Enable() = 0;
    virtual void Logic(const vec2d _mouse) = 0;
    virtual void Update() = 0;
    virtual void Render(SDL_Rect* clip_border = NULL) = 0;


    component(View* view, std::string id, int x, int y, int w, int h)
        :pos({ x,y }), size({ w,h }), id(id), is_active(false), in_focus(false)
    {
        this->view = view;
    }

    View* get_view()
    {
        return view;
    }

    virtual void Run(const vec2d _mouse, SDL_Rect* clip_border = NULL)
    {
        Logic(_mouse);
        Update();
        Render(clip_border);
    }

    virtual double get(std::string key)
    {
        if(key == "x")
            return pos.x;
        else if(key == "y")
            return pos.y;
        else if(key == "w")
            return size.w;
        else if(key == "h")
            return size.h;
    }

    virtual void set(std::string key, double value)
    {
        if(key == "x")
            pos.x = value;
        else if(key == "y")
            pos.y = value;
        else if(key == "w")
            size.w = value;
        else if(key == "h")
            size.h = value;
    }
};



/** \brief creates object of component sub-classes with ease, thanks to c++ compiler
    *
    * \param   arguments for sub component class
    * \return pointer to created component
    * \note components are created on the heap so manually deallocate
    */

template<class T, class... Args>
component* make_component(Args... args)
{
    T* comp = new T(args...);
    return comp;
}


#endif // componentx_H_INCLUDED

#ifndef TRIGGER_H_INCLUDED
#define TRIGGER_H_INCLUDED

#include "view.h"
#include "component.h"


class trigger : component
{
public:
    enum class STATE { click, held, release, none};
protected:
    /** \brief
        *  STATE -> state of the button { click , held or release }
        *  uint32_t -> length of time the button has been held
        *  \ width and heights is for image in case
        */
    std::function<void( STATE& , std::string key, unsigned int duration )> callback = NULL;
    text_renderer* TR;
    unsigned int held_start = 0;
    STATE state = STATE::none;

public:

    trigger(View* view, std::string uuid, std::string text, int x = 0, int y = 0,int w =0, int h = 0, std::function<void( STATE&, std::string, unsigned int duration  )> callback = NULL)
    :component(view,uuid,x,y,w,h), callback(callback)
    {
        TR = new text_renderer(view,"",14,text);
    }

    virtual ~trigger()
    {
        delete TR;
    }

    void Disable()
    {
        is_active = false;
    }

    void Enable()
    {
        is_active = true;
    }

    void Logic(const vec2d _mouse)
    {
        if(is_active)
        {

        }
    }

    virtual void Update() = 0;

    virtual void Render(SDL_Rect* clip_border) = 0;


    const STATE& get_state()
    {
        return state;
    }



    double get(std::string key) override
    {
        if(key == "x")
            return pos.x;
        else if(key == "y")
            return pos.y;
        else if(key == "w")
            return size.w;
        else if(key == "h")
            return size.h;
        else if(key == "font size" || key == "font_size")
            return TR->get("font_size");
        else return -1;

    }

    void set(std::string key, double value) override
    {
        if(key == "x")
            pos.x = value;
        else if(key == "y")
            pos.y = value;
        else if(key == "w")
            size.w = value;
        else if(key == "h")
            size.h = value;
        else if(key == "font size" || key == "font_size")
            TR->set("font_size",value);
    }


};


#endif // TRIGGER_H_INCLUDED

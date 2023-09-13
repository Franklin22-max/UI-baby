#ifndef VALUE_ADJUSTER_H_INCLUDED
#define VALUE_ADJUSTER_H_INCLUDED
/*
#include "component.h"


{
    class value_adjustment : component
    {
        double current_value;
        double max_value;
        double min_value;
        SDL_Rect pointer;
        text_renderer* TR;
        bool shift_pointer;
        SDL_Color bg, p_cl;
        bool can_calculate;


        void resolve_current_value()
        {
            double magnitude = max_value - min_value;
            current_value = min_value + (((pointer.x - pos.x) * 1.f) / (size.w - pointer.w)) * magnitude;
            TR->Load_text(to_string(current_value));
        }
    public:

        void Enable()
        {
            is_active = true;
        }

        void Disable()
        {
            is_active = false;
        }

        ~value_adjustment()
        {
            delete TR;
        }

        value_adjustment(View* view, std::string uuid, double min_value, double max_value, int x = 0, int y = 0, int w = 200, SDL_Color bg = {180,220,230,255}, SDL_Color pointer_cl = {50,50,100,255})
        :component(view,uuid,x,y,w,5), min_value(min_value), max_value(max_value), bg(bg), p_cl(pointer_cl)
        {
            TR = new text_renderer(view,"");
            TR->load_Font( theme::get_instance()->get_default_font()->font_name, 12);

            pointer = {x , y , 6, 10};
            pointer.y = pos.y + size.h/3;

            resolve_current_value();

            TR->set("x", pos.x+size.w + 2);
            TR->set("y",pos.y +  (size.h - TR->get("h"))/2);
        }

        void Logic(const vec2d _mouse) override
        {
            if(is_active)
            {
                vec2d mouse = view->resolve_point(_mouse);
                in_focus = (mouse.x >= pos.x && mouse.x <= pos.x + size.w && mouse.y >= pos.y && mouse.y <= pos.y + size.h);

                if(Administrator::get_instance()->get_key_state(view,"mouse left") == Event::key_state::held && (in_focus || (mouse.x >= pointer.x && mouse.x <= pointer.x + pointer.w && mouse.y >= pointer.y && mouse.y <= pointer.y + pointer.h)))
                {
                    if(mouse.x >= pos.x && mouse.x <= pos.x + (size.w - pointer.w))
                    {
                        pointer.x = mouse.x;
                        can_calculate = true;
                    }
                }
            }
        }

        void Update() override
        {
            if(is_active)
            {
                if(can_calculate)
                {
                    resolve_current_value();
                    TR->set("x", pos.x+size.w + 2 );
                    TR->set("y",pos.y +  (size.h - TR->get("h"))/2);
                    can_calculate = false;
                }
            }
        }



        void Render(SDL_Rect* clip_border = NULL) override
        {
            SDL_Rect R = {pos.x,pos.y,size.w,size.h};
            // draw back ground
            view->RenderFillRect(clip_border,&R,bg);
            // draw current
            TR->Render(clip_border);
            // draw pointer
            view->RenderFillRect(clip_border,&pointer,p_cl);
        }

        double get_current_value()
        {
            return current_value;
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
            else if(key == "current value" || key == "current_value")
                return current_value;
            else if(key == "max_value" || key == "max value")
                return max_value;
            else if(key == "min_value" || key == "min value")
                return min_value;
            else return -1;

        }

        void set(std::string key, double value) override
        {
            if(key == "x")
            {
                double val = pointer.x - pos.x;
                pos.x = value;
                pointer.x = pos.x + val;
            }
            else if(key == "y")
                pos.y = value;
            else if(key == "w")
                size.w = value;
            else if(key == "h")
                size.h = value;
            else if(key == "font size" || key == "font_size")
                TR->set("font_size",value);
            else if(key == "max_value" || key == "max value")
                max_value = value;
            else if(key == "min_value" || key == "min value")
                min_value = value;
        }

    };
}

*/
#endif // VALUE_ADJUSTER_H_INCLUDED

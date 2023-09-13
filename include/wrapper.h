#pragma once

#include "Administrator.h"
#include "component.h"
#include <vector>


/** \brief  A way to organise components
    it has two operation modes VERTICAL GROWT, in which case grids define how many columns needed
    and the general with of each column is defined by the width of largest component in that column

    the order mode is HORIZONTAL in which case grid defines the rows, and the height of each row
    is defined by that of the largest element in that row

    NB:
 */




class wrapper : component
{
public:
    enum class GROWTH_DIRECTION { HORIZONTALLY, VERTICALLY};
private:
    uint8_t pad;
    uint8_t grid_no;
    vec2d plane_size; // the size of the plane where components are place
    GROWTH_DIRECTION dir;
    random_vector<component*> components;





    void arrange()
    {
        if(dir == GROWTH_DIRECTION::VERTICALLY)
        {
            plane_size = {0,0};
            int* max_grid_width  = new int[grid_no];
            for(int i =0; i < grid_no; i++)
                max_grid_width[i] = 0;

            // get the largest width for each grid
            for(int i = 0; i < components.size(); i++)
            {
                double num = max_grid_width[i % grid_no];
                max_grid_width[i % grid_no] = std::max( num , components[i]->get("w"));
            }
            // set each grid start position
            for(int i = 0; i < grid_no; i++)
                if(i > 0)   max_grid_width[i] += (pad + max_grid_width[i - 1]); else max_grid_width[i] += pad;

            plane_size.w = max_grid_width[grid_no - 1];

            int max_heigth = 0;
            for(int i = 0; i < components.size(); i++)
            {
                if(i % grid_no == 0)
                {
                    plane_size.h += max_heigth;
                    max_heigth = 0;

                    components[i]->set("x", pos.x);
                    components[i]->set("y", pos.y + plane_size.h);
                    max_heigth = components[i]->get("h");
                }
                else
                {
                    components[i]->set("x", max_grid_width[(i % grid_no) - 1] + pos.x);
                    components[i]->set("y", pos.y + plane_size.h);
                    int num = components[i]->get("h");
                    max_heigth = std::max(num, max_heigth);

                    if(i == components.size() - 1)
                        plane_size.h += max_heigth;

                }
            }

            delete max_grid_width;
        }
        else if(dir == GROWTH_DIRECTION::HORIZONTALLY)
        {
            plane_size = {0,0};
            int* max_grid_h = new int[grid_no];
            for(int i =0; i < grid_no; i++)
                max_grid_h[i] = 0;

            // get the largest height for each grid
            for(int i = 0; i < components.size(); i++)
            {
                int num = components[i]->get("h");
                max_grid_h[i % grid_no] = std::max(max_grid_h[i % grid_no], num);
            }
            // set each grid start position
            for(int i = 0; i < grid_no; i++)
                if(i > 0)   max_grid_h[i] += (pad + max_grid_h[i - 1]); else max_grid_h[i] += pad;

            plane_size.h = max_grid_h[grid_no - 1];

            int max_width = 0;
            for(int i = 0; i < components.size(); i++)
            {
                if(i % grid_no == 0)
                {
                    plane_size.w += max_width;
                    max_width = 0;

                    components[i]->set("y", pos.y);
                    components[i]->set("x", pos.x + plane_size.w);
                    max_width = components[i]->get("w");
                }
                else
                {
                    components[i]->set("y", max_grid_h[(i % grid_no) - 1] + pos.y);
                    components[i]->set("x", pos.x + plane_size.w);
                    int num = components[i]->get("w");
                    max_width = std::max(num, max_width);

                    if(i == components.size() - 1)
                        plane_size.w += max_width;

                }
            }

            delete max_grid_h;
        }

    }
public:
    /** \brief
        * \param pan spacing b/w two components
        * \param dir this is the direction the wrapper is allowed to grow
        * \param grid_no : this is the number of component required to fill the non growing direction
        */

    wrapper(View* view,std::string id, int x, int y,int w,int h, uint8_t pad, uint8_t grid_no = 1, GROWTH_DIRECTION dir = GROWTH_DIRECTION::VERTICALLY)
    :component(view,id,x,y,w,0), dir(dir), grid_no(grid_no), pad(pad)
    {
        plane_size = {0,0};
    }


    ~wrapper()
    {

    }

    /** \brief
        * \return a constant pointer to inputed component or null pointer on failure
        * \note use make component to add component, the component must only exist in wrapper to avoid conflict
        */

    const component* add_component(component* comp)
    {
        if(comp)
        {
            comp->Enable();
            components.push_back(comp);
            auto ptr = comp->get_view();
            ptr = view;
            return comp;
        }
        else return nullptr;
    }



    void Disable()
    {
        is_active = false;
    }


    void Enable()
    {
        if(view)
            is_active = true;
    }


    void Logic(const vec2d _mouse)
    {
        if(is_active)
        {
            vec2d mouse = view->resolve_point(_mouse);

            on_hover = (mouse.x >= pos.x && mouse.x <= pos.x + size.w && mouse.y >= pos.y && mouse.y <= pos.y + size.h);

            if(on_hover == true && Administrator::get_instance()->get_key_state(this->view,"mouse_left") == Event::key_state::click)
                in_focus = true;
            else if (Administrator::get_instance()->GET_ACTIVE_VIEW() != view   ||  (on_hover != true && Administrator::get_instance()->get_key_state(this->view,"mouse_left") == Event::key_state::click)
                            || (on_hover != true && Administrator::get_instance()->get_key_state(this->view,"mouse_right") == Event::key_state::click))
                in_focus = false;


            if(in_focus)
                for(auto &comp : components)
                    comp->Logic(_mouse);
            else
                for(auto &comp : components)
                    comp->Logic({-9999, -9999});
        }
    }


    void Update()
    {
        if(is_active)
        {
            for(auto &comp : components)
                comp->Update();
        }
    }

    void Render(SDL_Rect* clip_border = NULL) override
    {
        if(is_active)
        {
            SDL_Rect border = {pos.x,pos.y,size.w, size.h};
            for(auto &i: components)
            {
                // use wrapper display as clip border
                i->Render(&border);
            }
        }
    }

};


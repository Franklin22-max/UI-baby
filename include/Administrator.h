#ifndef ADMINISTRATOR_H_INCLUDED
#define ADMINISTRATOR_H_INCLUDED

#include "view.h"
#include "Event.h"
#include "Error.h"
#include <SDL_syswm.h>
#include <algorithm>

    /** \brief  Positions views and also manage event allocation to different views
     */

    class Administrator
    {
    public:
        enum VIEW_PLACEMENT {ALWAYS_ON_TOP_AND_GRAB_EVENT = 0 , ALWAYS_ON_TOP = 1 , ANY_POSITION = 2};
    private:
        struct VIEW_NODE
        {
            VIEW_PLACEMENT placement;
            View* view;
            VIEW_NODE(VIEW_PLACEMENT placement , View* view)
            : placement(placement), view(view)  {   };
        };

        Administrator() { event = Event::get_instance(); };
        Administrator(const Administrator&) = delete;

        SDL_Window* window;
        Event* event;
        // the view with the event
        View* active_view = NULL;
        // buffer for storing views
        random_vector<VIEW_NODE> views;
        // components that reqTESTres text inputs
        std::map<std::string,bool> text_components;


        bool isViewInFocus(View* v)
        {
            vec2d mouse = event->get_mouse_pos();
            return (mouse.x >= v->display.x && mouse.y >= v->display.y && mouse.x <= v->display.x+v->display.w && mouse.y <= v->display.y+v->display.h);
        }



        void arrange_views()
        {
            if(!views.empty())
            {
                bool inOrder = false;
                while( !inOrder)
                {
                    inOrder = true;
                    random_vector<VIEW_NODE>::iterator prev;
                    for(auto node = views.begin(); node != views.end(); ++node)
                    {
                        if(node != views.begin())
                        {
                            // based on how enum (VIEW_PLACEMENT) is index we are going to arrange the view
                            if(node->placement < prev->placement)
                            {
                                inOrder = false;
                                // swap nodes
                                VIEW_NODE hold = (*node);
                                (*node) = (*prev);
                                (*prev) = hold;
                            }
                        }
                        prev = node;
                    }
                }
            }
        }





        random_vector<VIEW_NODE>::iterator find(View* view)
        {
            for(auto node = views.begin(); node != views.end(); ++node)
            {
                if((*node).view  == view) return node;
            }

            return views.end();
        }



        void __remove_view(View* view)
        {
            random_vector<VIEW_NODE>::iterator it  = this->find(view);
            if(it != views.end())
                views.erase(it);
        }


        static Administrator* instance;
    public:
        static Administrator* get_instance(){ return instance = (instance != nullptr)? instance : new Administrator;};


        const View* GET_ACTIVE_VIEW()
        {
            return active_view;
        }

        void INIT(SDL_Window* window)
        {
            this->window = window;
        }


        void DESTROY()
        {
            for(auto &node : views)
                delete node.view;// delete view pointers before clearing the buffer
            views.erase(views.begin(), views.end());
        }

        HWND GET_WINDOW_HANDLER()
        {
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);
            SDL_GetWindowWMInfo(window,&info);
            return info.info.win.window;
        }



        void CHANGE_VIEW_PLACEMENT(View* view, VIEW_PLACEMENT placement)
        {
            if(view != nullptr && view != NULL)
            {
                random_vector<VIEW_NODE>::iterator node = this->find(view);
                if(node != views.end())
                    (*node).placement = placement;
                else
                    Error::write_error("Couldn't change view placement: view doesn't exist in admin");
            }
            else
                Error::write_error("Couldn't change view placement: view pointer is empty");
        }


        View* CREATE_VIEW(VIEW_PLACEMENT placement,SDL_Renderer* renderer,int x,int y,int w=350,int h=600,SDL_Color bg = {0,0,0,0},View::MODE adjust = View::MODE::DYNAMIC, vec2d texture_size = {350,150})
        {
            View* __view = new View(renderer,x,y,w,h,bg,adjust,texture_size);

            if(__view)
            {
                views.push_front(VIEW_NODE(placement,__view));
                arrange_views();
                return __view;
            }
            Error::write_error("Couldn't create view");
            return nullptr;
        }


        void EVENT_POOL()
        {   // run event via function operator
            (*event)();
            // set active view when mouse is clicked
            if(event->mouse_left.get_state() == Event::key_state::click || event->mouse_right.get_state() == Event::key_state::click || event->mouse_left.get_state() == Event::key_state::held || event->mouse_right.get_state() == Event::key_state::held)
            {
                for(auto node = views.begin(); node != views.end(); ++node)
                {
                    if(node->placement == VIEW_PLACEMENT::ALWAYS_ON_TOP_AND_GRAB_EVENT)
                    {
                        // make active view only when view is in focus
                        active_view = ( isViewInFocus(node->view))?  node->view : NULL;
                        // arrange views
                        arrange_views();
                        break;
                    }
                    else if( isViewInFocus(node->view))
                    {
                        VIEW_NODE hold = (*node);
                        // remove node and add it to the front of the buffer
                        __remove_view( node->view);
                        views.push_front(hold);
                        // arrange views
                        arrange_views();
                        // set the view as active view
                        active_view = hold.view;
                        break;
                    }
                    else if(node == (--views.end()))
                        active_view = NULL;
                }
            }
        }



        void RENDER_PRESENT()
        {
            for(int i = views.size()-1; i >= 0; i--)
                views[i].view->RenderPresent();
        }


        void REMOVE_VIEW(View* view)
        {
            random_vector<VIEW_NODE>::iterator it  = this->find(view);
            if(it != views.end())
            {
                delete it->view;// delete view pointers before erasing
                views.erase(it);
            }
        }















        /** \brief
         *  All functions below are encapsulations of event calls with a higher level control
         */

        Event::key_state get_special_key_state(std::string key)
        {
            return event->get_key_state(key);
        }

        vec2d get_mouse_pos(View* view)
        {
            if(view == active_view)
            {
                if(view == active_view)
                    return event->get_mouse_pos();
                else
                    return {-1,-1};
            }
        }

        Event::key_state get_key_state(View* view,std::string key)
        {
            if(view == active_view)
                return event->get_key_state(key);
            else
                return Event::key_state::none;
        }

        bool get_modstate(View* view, std::string key_mode_name)
        {
            if(view == active_view)
                return event->get_modstate(key_mode_name);
            return false;
        }

        void start_text_input(View* view,std::string id)
        {
            if(SDL_IsTextInputActive() == SDL_FALSE)
            {
                if(view == active_view)
                {
                    text_components[id] = true;
                    event->start_text_input();
                }
            }
        }

        void stop_text_input(View* view, std::string id)
        {
            bool skip = false;
            text_components[id] = false;
            for(auto &i : text_components)
            {
                if(i.second == true)// ignore stop text input if another component is using it
                {
                    skip = true;    break;
                }
            }

            if(!skip)
            {
                if(view == active_view)
                    event->stop_text_input();
            }
        }

        char* get_inputed_text(View* view)
        {
            if(view == active_view)
                return event->get_text_char();
            else
                return nullptr;
        }

        bool has_inputed_text(View* view)
        {
            if(view == active_view)
                return event->has_inputed_text();
            return false;
        }


    };

#endif // ADMINISTRATOR_H_INCLUDED

#ifndef TEST_INCLUDE_H
#define TEST_INCLUDE_H


#include "common.h"
#include "Event.h"
#include "Renderer.h"
#include "f_time.h"
#include "Administrator.h"
#include "text_input.h"
#include "File.h"
#include "wrapper.h"
#include "SurfaceHandler.h"
#include "Theme.h"
#include "trigger.h"
#include "value_adjuster.h"
#include "button.h"

#define RENDERER_SYNC           SDL_RENDERER_PRESENTVSYNC
#define RENDERER_ACCELERATED    SDL_RENDERER_ACCELERATED
#define RENDERER_SOFTWARE       SDL_RENDERER_SOFTWARE
#define RENDERER_TARGETTEXTURE  SDL_RENDERER_TARGETTEXTURE




static int num = 0;
class TEST
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    Administrator* administrator;
    Event* event;
    bool isRunning = false;
    int window_h,window_w;
    Time timer;

    View* view1;
    View* view2;
    View* view3;

    wrapper* wrp;
    image_renderer* ir;
    image_renderer* ir2;
    text_renderer* tr;
    text_input* text_in;
    text_input* text_in2;

    //value_adjustment* va;

    SDL_Surface* surf;
    SDL_Texture* texture = NULL;

    vec2d former_mouse;

public:
    std::string INIT(const char* title,int window_w, int window_h, UINT32 renderer_flag)
    {
        this->window_h = window_h;
        this->window_w = window_w;

        //  initialize SDL2
        if(SDL_Init(SDL_INIT_EVERYTHING) > 0) return std::string("Error initializing SDL: ")+SDL_GetError();
        // initialize SDL_TTF
        if(TTF_Init()) return std::string("Error initializing TTF: ")+SDL_GetError();
        // initialize SDL_IMAGE
        if(!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP))  return std::string("Error initializing IMG: ");
        // initialize mixer
        if(!Mix_Init(MIX_INIT_MP3 | MIX_INIT_MOD | MIX_INIT_FLAC | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_OPUS)) return std::string("Error initializing MIXER: ");

        // create window
        window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,window_w,window_h,SDL_WINDOW_RESIZABLE);
        if(!window) return std::string("Error creating window: ")+SDL_GetError();

        // create renderer
        renderer = SDL_CreateRenderer(window,-1,renderer_flag);
        if(!renderer)  return std::string("Error creating renderer: ")+SDL_GetError();

        std::string err;
        theme::get_instance()->INIT(renderer);
        theme::get_instance()->load_font("consolaz", R"(fonts\calibri.ttf)",14,&err);
        theme::get_instance()->set_default_font("consolaz",14);
        std::cout<<err;

        // create event object
        event = Event::get_instance();
        administrator = Administrator::get_instance();

        // insert window
        administrator->INIT(window);

        // if all passed enable
        isRunning = true;
        SDL_Color bebe = {250,255,255,255};
        SDL_Color white = {225,230,230,250};
        SDL_Color black = {0,0,0,0};
        vec2d size = {1400,1000};
        timer.set_max_FPS(240);

        view1 = administrator->CREATE_VIEW(Administrator::ANY_POSITION,renderer,300,200,600,400,white,View::MODE::STATIC,size);
        view2 = administrator->CREATE_VIEW(Administrator::ANY_POSITION,renderer,300,100,400,250,white,View::MODE::STATIC,size);
        view3 = administrator->CREATE_VIEW(Administrator::ANY_POSITION,renderer,700,250,500,400,bebe,View::MODE::STATIC,size);

        //auto img = theme::get_instance()->Load_image("narayana",R"(images/Narayana.png)");
        //ir = new image_renderer(view2,img,0,0,600,360);
        tr = new text_renderer(view1,"Whenever dharma declines, oh son of Bharata and evil prevails, i incarnate myself, to destroy the evil-doers and restore dharma in the hearth of men. I am born from ages to ages.",black,200,50,text_renderer::WRAPPED);
        //wrp = new wrapper(view1,20,20,200,200,0,1);
        text_in = new text_input(view1,"text input 1",200,300,250);
        text_in2 = new text_input(view3,"text input 2",200,200,250);
        //va = new value_adjustment(view3,"va1",0,194, 200,100);

        //va->Enable();
        text_in->Enable();
        text_in2->Enable();


        SDL_Surface* narayana = LOAD_IMAGE(R"(images/Narayana.png)");

        if(narayana)
        {
            uint32_t* p = (uint32_t*)narayana->pixels;
            SDL_Surface* round_vishnu = make_round_edges(narayana,30.0);
            texture = SDL_CreateTextureFromSurface(renderer,round_vishnu);

            SDL_FreeSurface(round_vishnu);
            SDL_FreeSurface(narayana);
        }

        return "";
    }


    void RUN()
    {
        num++;
        timer.frame_sync();
        UPDATE();


        vec2d mouse;
        if(Administrator::get_instance()->GET_ACTIVE_VIEW() == view1)
            mouse = Administrator::get_instance()->get_mouse_pos(view1);
        else if(Administrator::get_instance()->GET_ACTIVE_VIEW() == view3)
            mouse = Administrator::get_instance()->get_mouse_pos(view3);

        View* v  = (Administrator::get_instance()->GET_ACTIVE_VIEW() == view1)? view1 : view3;

        if(Administrator::get_instance()->get_special_key_state("f5") == Event::key_state::held)
            v->zoom_in();
        else if(Administrator::get_instance()->get_special_key_state("f6") == Event::key_state::held)
            v->zoom_out();
        else if(Administrator::get_instance()->get_special_key_state("mouse right") == Event::key_state::held && (former_mouse.x != mouse.x || former_mouse.y != mouse.y))
        {
            v->pan_horizontally(former_mouse.x - mouse.x);
            v->pan_vertically(former_mouse.y - mouse.y);
        }



        //ir->Render();
        tr->Render();

        text_in->Logic(mouse);
        text_in->Update();
        text_in->Render();

        text_in2->Logic(mouse);
        text_in2->Update();
        text_in2->Render();

        /*va->Logic(mouse);
        va->Update();
        va->Render();*/

        if(texture)
            view1->RenderCopy(texture,NULL);

        administrator->RENDER_PRESENT();
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderClear(renderer);

        former_mouse = mouse;
    }

    void TERMINATE()
    {
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    void UPDATE()
    {
        administrator->EVENT_POOL();

        if(administrator->get_special_key_state("quit") == Event::key_state::click || administrator->get_special_key_state("esc") == Event::key_state::click)
            isRunning = false;
    }

    bool is_running()
    {
        return isRunning;
    }
};

#endif // TEST_INCLUDE_H

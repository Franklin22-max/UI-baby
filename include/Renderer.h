#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "Error.h"
#include "view.h"
#include "Theme.h"


class Renderder
{
protected:
    SDL_Texture* texture = nullptr;
    SDL_Renderer* renderer = nullptr;
    View* view;
    vec2d position;
    vec2d size;
public:
    virtual double get(std::string key) = 0;
    virtual void set(std::string key, double value) = 0;
};

///                             TEXT RENDERER
class text_renderer : public Renderder
{
public:
    enum TYPE { WRAPPED, LINEAR} type;
private:
    uint16_t wrapper_length;
    bool can_reload = true;
    std::string text;
    std::string font_name;
    SDL_Color color;
    const theme::FONT_NODE* font = nullptr;


    void init(std::string font_name = "", int fs = 14)
    {
        this->renderer = view->get_renderer();
        load_Font(font_name,fs);
        Load_text(text);
    }

public:

    text_renderer(View* view, std::string font_name, int font_size,std::string text,SDL_Color color = {0,0,255,255},int x = 100,int y = 100, TYPE type = LINEAR, uint16_t wrapper_length = 250)
    :text(text), color(color), type(type), wrapper_length(wrapper_length)
    {
        this->view = view;
        this->position = {x,y};
        init(font_name,font_size);

    }

    text_renderer(View* view,std::string text, SDL_Color color = {0,0,255,255}, int x = 100,int y = 100, TYPE type = LINEAR, uint16_t wrapper_length = 250)
    :text(text), color(color), type(type), wrapper_length(wrapper_length)
    {
        this->view = view;
        this->position = {x,y};
        init();
    }


    void load_Font(std::string font_name, int fs)
    {
        const theme::FONT_NODE* f_node = theme::get_instance()->get_default_font();

        if(f_node)
        {
            this->font_name = font_name;
            this->font = f_node;
            can_reload = true;
        }
        else Error::write_error("No default Font");
    }

    bool is_text_active()
    {
        return (this->font)? true : false;
    }

    /** \brief loads a text texture
        * \return the size of loaded texture
        *
        */

    vec2d Load_text(std::string _text)
    {
        if(_text != text)
        {
            text = _text;
            can_reload = true;
        }

        if(can_reload && !text.empty() && font)// help prevent unnecessary loading of texture
        {
            if(font->font)
            {
                // load text either linear or wrapped
                SDL_Surface* surface = (type == LINEAR)? TTF_RenderText_Blended(font->font,text.c_str(),color)  :   TTF_RenderText_Blended_Wrapped(font->font,text.c_str(),color,wrapper_length);
                if(!surface)
                {
                    Error::write_error("LOADTEXT SURFACE: " + std::string(SDL_GetError()));
                    size = {0,0};
                    return size;
                }
                else
                {
                    if(texture != nullptr)// free texture memory b/f assigning a new one
                        SDL_DestroyTexture(texture);

                    texture = SDL_CreateTextureFromSurface(renderer,surface);
                    if(!texture)
                    {
                        Error::write_error("LOADTEXT TEXTURE: " + std::string(SDL_GetError()));
                        texture = nullptr;          size = {0,0};
                    }
                    else
                    {
                        SDL_QueryTexture(texture,0,0,&size.w,&size.h);
                        can_reload = false;
                    }
                    SDL_FreeSurface(surface);
                    return size;
                }
            }
        }
        return size;
    }

    void clear()
    {
        text = "";
        size = {0,0};
        if(texture) SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    void Render(SDL_Rect* clip_board = NULL,SDL_Rect* view_port = NULL,uint8_t alpha = 255,double angle = 0.0, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        if(texture && font)
        {
            if(font->font)
            {
                SDL_SetTextureAlphaMod(texture,alpha);
                SDL_Point point = {size.w/2,size.h/2};// center
                SDL_Rect display = {position.x,position.y,size.w,size.h};

                if(view_port != NULL)// make sure text does not look funky when view port is shifted
                {
                    display.w = view_port->w;
                    display.h = view_port->h;
                }
                view->RenderCopy(texture,clip_board,view_port,&display,angle,&point,flip);
            }
        }
    }

    double get(std::string key) override
    {
        if(key == "x")
            return position.x;
        else if(key == "y")
            return position.y;
        else if(key == "w")
            return size.w;
        else if(key == "h")
            return size.h;
        else if((key == "font size") || (key == "font_size") && (font))
            return font->font_size;
        else return -1;
    }

    void set(std::string key, double value) override
    {
        if(key == "x")
            position.x = value;
        else if(key == "y")
            position.y = value;
        else if(key == "font size" || key == "font_size")
        {
            load_Font(font_name,value);
        }
    }

    void set_type(TYPE type, uint16_t length = 0)
    {
        wrapper_length = (length > 0)? length : wrapper_length;
        this->type = type;
    }

    void set_color(SDL_Color cl)
    {
        if(cl != color)
        {
            can_reload = true;
            color = cl;
        }
    }

};



///                                     IMAGE RENDERER

class image_renderer : public Renderder
{
    std::string path;
public:
    image_renderer(View* view,const theme::IMG_NODE* img_node,int x = 0, int y = 0,int w = 100, int h = 100)
    {
        this->view = view;
        this->renderer = view->get_renderer();
        this->position = {x,y};
        this->size = {w,h};


        Load_image(img_node,position,size);
    }

    vec2d Load_image(const theme::IMG_NODE* img_node,vec2d pos = {100,100},vec2d size = {100,100})
    {
        this->position = pos;
        this->size = size;
        this->texture = NULL;
        // load image
        if(!img_node)
        {
            Error::write_error("LOAD_IMG TEXTURE: " + std::string(SDL_GetError()));
            return {0,0};
        }
        else
        {
            this->texture = img_node->img;
            this->path = img_node->path;
            int w,h;
            SDL_QueryTexture(texture,NULL,NULL,&w,&h);
            return {w,h};
        }
    }

    void Render(SDL_Rect* clip_board = NULL,SDL_Rect* view_port = NULL,uint8_t alpha = 255,double angle = 0.0, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        if(texture)
        {
            SDL_SetTextureAlphaMod(texture,alpha);
            SDL_Point point = {size.w/2,size.h/2};// center
            SDL_Rect display = {position.x,position.y,size.w,size.h};
            view->RenderCopy(texture,clip_board,view_port,&display,angle,&point,flip);
        }
    }

    double get(std::string key) override
    {
        if(key == "x")
            return position.x;
        else if(key == "y")
            return position.y;
        else if(key == "w")
            return size.w;
        else if(key == "h")
            return size.h;
        else return -1;
    }

    void set(std::string key, double value) override
    {
        if(key == "x")
            position.x = value;
        else if(key == "y")
            position.y = value;
    }


};


#endif // RENDERER_H_INCLUDED

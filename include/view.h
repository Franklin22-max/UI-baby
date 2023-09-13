#ifndef VIEW_H
#define VIEW_H


#include "common.h"
#include "Shapes.h"
#include "Event.h"


struct rendererable
{
    rendererable(SDL_Texture* texture,SDL_Rect* view_port = NULL,SDL_Rect* display = NULL,double angle = 0.0,SDL_Point* point = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE)
    :texture(texture), view_port(view_port), display(display), angle(angle), point(point),flip(flip) {};

    SDL_Texture* texture;
    SDL_Rect* view_port;
    SDL_Rect* display;
    double angle;
    SDL_Point* point;
    SDL_RendererFlip flip;
};

/** \brief gives a high level abstraction on view controls like panning and zooming
    *  \ DYNAMIC-> this means that the target texture resizes it self to the minimum size reqTESTred to render a texture
    *  \note view does not call SDL_RenderPresent so this should be called manually
    */
class View
{
public:
 /*
    dynamic means internanal texture has to adjust it size base of input size while static means texture stay the same
 */
    enum MODE{ DYNAMIC, STATIC} texture_mode;

    static vec2d get_max_texture_size()
    {
        return max_texture_size;
    }
private:
    // tells the max texture size of device
    static vec2d max_texture_size;
    // tell the largest width or height of texture copied
    vec2d max_copied_size;
    // holds initial values assigned by user in case of mode change
    vec2d assigned_display_size;
    vec2d assigned_texture_size;

    SDL_Renderer* renderer;
    SDL_Texture* target_texture;

    double scale;

    vec2d position;
    vec2d texture_size;

    SDL_Rect display;
    SDL_Rect zoom_box;
    SDL_Rect view_port;

    double pan_offset_x;
    double pan_offset_y;
    SDL_Color bg;

    friend class Administrator;


    void RenderPresent()
    {
        view_port = {zoom_box.x + (int)pan_offset_x, zoom_box.y + (int)pan_offset_y, zoom_box.w, zoom_box.h};
        SDL_RenderCopy(renderer,target_texture,&view_port,&display);

        if(texture_mode == DYNAMIC)
            make_new_texture(std::max(max_copied_size.w, assigned_texture_size.w),  std::max(max_copied_size.h, assigned_texture_size.h));
        else
            make_new_texture(assigned_texture_size.w,assigned_display_size.h);
    }

    void zoom()
    {
        vec2d fzoom = {zoom_box.w,zoom_box.h};
        zoom_box = {(int)(display.w - display.w/scale)/2,    (int)(display.h - display.h/scale)/2,   int(display.w/scale),  int(display.h/scale)};

        pan_offset_x += (pan_offset_x + zoom_box.x < 0)? _fabs(pan_offset_x + zoom_box.x) : 0;
        pan_offset_x -= (pan_offset_x + zoom_box.x > (texture_size.w - zoom_box.w))?    (pan_offset_x + zoom_box.x)-(texture_size.w  - zoom_box.w) : 0;

        pan_offset_y += (pan_offset_y + zoom_box.y < 0)? _fabs(pan_offset_y + zoom_box.y) : 0;
        pan_offset_y -= (pan_offset_y + zoom_box.y > (texture_size.h - zoom_box.h))?    (pan_offset_y + zoom_box.y)-(texture_size.h - zoom_box.h) : 0;
    }


    void make_new_texture(int w, int h, bool bypass = false)
    {
        if(texture_size.w != w || texture_size.h != h || !target_texture)
        {
            __resize_texture(w,h);

            if(target_texture)
                SDL_DestroyTexture(target_texture);

            target_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,texture_size.w,texture_size.h);
            if(target_texture)
            {
                max_copied_size = {0,0};
                SDL_SetRenderTarget(renderer,target_texture);
                SDL_SetRenderDrawBlendMode(renderer,    SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer,bg.r,bg.g,bg.b,bg.a);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawBlendMode(renderer,    SDL_BLENDMODE_NONE);
                SDL_SetRenderTarget(renderer,NULL);

            }
            else Error::write_error("View Texture Error: " + std::string(SDL_GetError()));
        }
        else
        {
            SDL_SetRenderTarget(renderer,target_texture);
            SDL_SetRenderDrawBlendMode(renderer,    SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer,bg.r,bg.g,bg.b,bg.a);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawBlendMode(renderer,    SDL_BLENDMODE_NONE);
            SDL_SetRenderTarget(renderer,NULL);
        }
    }

    View(SDL_Renderer* renderer,int x,int y,int w,int h,SDL_Color bg,MODE adjust, vec2d texture_size)
    :renderer(renderer),display({x,y,w,h}), assigned_texture_size(texture_size), bg(bg), texture_mode(adjust)
    {
        target_texture = nullptr;
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer,&info);
        max_texture_size = {info.max_texture_width,info.max_texture_height};
        // make sure that size is not greater than the maximum texture size of the device
        assigned_texture_size.w = std::min(info.max_texture_width,assigned_texture_size.w);
        assigned_texture_size.h = std::min(info.max_texture_height,assigned_texture_size.h);
        max_copied_size = texture_size = {0,0};

        scale = 1;
        assigned_display_size = {w,h};
        zoom_box = {0,0,w,h};
        // create a new target texture
        make_new_texture(assigned_texture_size.w,assigned_texture_size.h);
    }

    void __resize_texture(int w, int h)
    {
        // make sure that size is not greater than the maximum texture size of the device
        w = std::min(max_texture_size.w,w);
        h = std::min(max_texture_size.h,h);

        texture_size.w = w;
        texture_size.h = h;

        zoom_box.w = std::min(assigned_display_size.w,texture_size.w);
        zoom_box.h = std::min(assigned_display_size.h,texture_size.h);
        display.w = zoom_box.w;
        display.h = zoom_box.h;

        zoom();// make sure that view data adjust
    }
    /** \brief
        *
        * \param
        * \param
        *
        * \return
        */

    void resolve_dynamic_sizing(int w,int h,SDL_Rect* display)
    {
        if(texture_mode == DYNAMIC)
        {
            // get max input texture size
            max_copied_size.w = std::max(max_copied_size.w,     (display != NULL)? display->x+display->w : w);
            max_copied_size.h = std::max(max_copied_size.h,     (display != NULL)? display->y+display->h : h);

            // make sure that texture size is not greater than renderer's max texture size
            max_copied_size.w = std::min(max_texture_size.w,max_copied_size.w);
            max_copied_size.h = std::min(max_texture_size.h,max_copied_size.h);
        }
    }

    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void __RenderFillRect(SDL_Rect* rect, SDL_Color cl, SDL_BlendMode bl = SDL_BLENDMODE_NONE)
    {
        SDL_SetRenderTarget(renderer,target_texture);
        SDL_SetRenderDrawBlendMode(renderer,bl);
        SDL_SetRenderDrawColor(renderer,cl.r,cl.g,cl.b,cl.a);
        SDL_RenderFillRect(renderer,rect);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(renderer,NULL);
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void __RenderDrawLine(Line line,SDL_Color cl,SDL_BlendMode bl = SDL_BLENDMODE_NONE)
    {

        SDL_SetRenderTarget(renderer,target_texture);
        SDL_SetRenderDrawBlendMode(renderer,bl);
        SDL_SetRenderDrawColor(renderer,cl.r,cl.g,cl.b,cl.a);
        SDL_RenderDrawLine(renderer,line.head.x , line.head.y, line.tail.x, line.tail.y);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(renderer,NULL);
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void __RenderDrawPoint(int x,int y,SDL_Color cl,SDL_BlendMode bl = SDL_BLENDMODE_NONE)
    {
        SDL_SetRenderTarget(renderer,target_texture);
        SDL_SetRenderDrawBlendMode(renderer,bl);
        SDL_SetRenderDrawColor(renderer,cl.r,cl.g,cl.b,cl.a);
        SDL_RenderDrawPoint(renderer,x,y);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(renderer,NULL);
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void __RenderCopy(rendererable data)
    {
        __RenderCopy(data.texture,data.view_port,data.display,data.angle,data.point,data.flip);
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void __RenderCopy(SDL_Texture* texture,SDL_Rect* view_port = NULL,SDL_Rect* display = NULL,double angle = 0.0,SDL_Point* point = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        SDL_SetRenderTarget(renderer,target_texture);

        int w =0 ,h=0;
        if(texture != NULL)
            SDL_QueryTexture(texture,NULL,NULL,&w,&h);

        resolve_dynamic_sizing(w,h,display);
        // copy texture data to our target texture
        SDL_RenderCopyEx(renderer,texture,view_port,display,angle,point,flip);
        SDL_SetRenderTarget(renderer,NULL);
    }


public:


    ~View()
    {
        if(target_texture)
            SDL_DestroyTexture(target_texture);
    }


    // get the normalize position of inputted vector within view
    vec2d resolve_point(vec2d pos)
    {
        double ratio_w = (zoom_box.w * 1.0)/display.w;
        double ratio_h = (zoom_box.h * 1.0)/display.h;

        vec2d val;

        if (pos.x > display.x && pos.x < display.x + display.w && pos.y > display.y && pos.y < display.y + display.h)
        {
            val.x = (pos.x - display.x) * ratio_w + pan_offset_x + zoom_box.x;
            val.y = (pos.y - display.y) * ratio_h + pan_offset_y + zoom_box.y;
        }
        else
            val = {-1,-1};
        return val;
    }

    SDL_Renderer* get_renderer()
    {
        return renderer;
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void RenderDrawPoint(SDL_Rect* clip_border,SDL_Point point,SDL_Color cl,SDL_BlendMode bl = SDL_BLENDMODE_NONE)
    {
        SDL_Rect ts = {0,0,texture_size.w,texture_size.h};
        SDL_Rect c = (clip_border == NULL)? ts : *clip_border;
        if(SDL_PointInRect(&point,&c))
            __RenderDrawPoint(point.x,point.y, cl,bl);
    }

    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void RenderDrawLine(SDL_Rect* clip_border, Line line,SDL_Color cl,SDL_BlendMode bl = SDL_BLENDMODE_NONE)
    {
        Line empty; empty.head = empty.tail = {0,0};
        SDL_Rect ts = {0,0,texture_size.w,texture_size.h};
        SDL_Rect c = (clip_border == NULL)? ts : *clip_border;
        Line l = get_intersection_line(line,c);
        if(l != empty)
            __RenderDrawLine(l,cl,bl);

    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void RenderDrawRect(SDL_Rect* clip_border,SDL_Rect* src,SDL_Color cl,SDL_BlendMode bl = SDL_BLENDMODE_NONE)
    {
        SDL_Rect ts = {0,0,texture_size.w,texture_size.h};
        SDL_Rect c = (clip_border == NULL)? ts : *clip_border;
        SDL_Rect r = get_intersection_rect(*src, c);

        SDL_SetRenderTarget(renderer,target_texture);
        SDL_SetRenderDrawBlendMode(renderer,bl);
        SDL_SetRenderDrawColor(renderer,cl.r,cl.g,cl.b,cl.a);
        // left
        if(r.x > c.x) SDL_RenderDrawLine(renderer,r.x,  r.y,    r.x,    r.y+r.h);
        // right
        if(r.x + r.w < c.x + c.w) SDL_RenderDrawLine(renderer,r.x+r.w,  r.y,    r.x+r.w,    r.y+r.h);
        // top
        if(r.y > c.y) SDL_RenderDrawLine(renderer,r.x,  r.y,    r.x+r.w,    r.y);
        // buttom
        if(r.y + r.h < c.y + c.h) SDL_RenderDrawLine(renderer,r.x,  r.y+r.h,    r.x+r.w,    r.y+r.h);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(renderer,NULL);
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void RenderFillRect(SDL_Rect* clip_border,SDL_Rect* src,SDL_Color cl,SDL_BlendMode bl = SDL_BLENDMODE_NONE)
    {
        SDL_Rect ts = {0,0,texture_size.w,texture_size.h};
        SDL_Rect c = (clip_border == NULL)? ts : *clip_border;

        SDL_Rect r = get_intersection_rect(*src, c);
        __RenderFillRect(&r, cl,bl);
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void RenderCopy(SDL_Texture* texture,SDL_Rect* clip_border,SDL_Rect* view_port = NULL,SDL_Rect* display = NULL,double angle = 0.0,SDL_Point* point = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        int w,h;
        SDL_QueryTexture(texture,NULL,NULL,&w,&h);

        SDL_Rect k = {0,0,w,h};
        SDL_Rect z = {0,0,texture_size.w, texture_size.h};

        SDL_Rect d = ( display != NULL)? *display : k;
        SDL_Rect v = ( view_port != NULL)? *view_port : k;
        SDL_Rect c = (clip_border != NULL)? *clip_border : z;
        // get clipped rect
        SDL_Rect r = get_intersection_rect(d, c);
        int _x,_y,_w,_h;

        SDL_Rect empty = {0,0,0,0};
        // don't draw if rect not visible

        if(r == d)
        {
            __RenderCopy(texture,&v,&d,angle,point,flip);
        }
        else if(r != empty)
        {
            // get ratio of display size to view port
            double __h = (v.h * 1.0f) / d.h;
            double __w = (v.w * 1.0f) / d.w;

            _x = r.x - d.x;     _y = r.y - d.y;     _w = d.w - r.w;     _h = d.h - r.h;
            //  set visible display
            d.x += _x;      v.x += _x * __w;
            d.y += _y;      v.y += _y * __h;
            d.w -= _w;      v.w -= _w * __w;
            d.h -= _h;      v.h -= _h * __h;

            __RenderCopy(texture,&v,&d,angle,point,flip);
        }
    }
    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    rendererable RenderExport(double angle = 0.0,SDL_Point* point = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        SDL_Rect vp = {zoom_box.x + pan_offset_x, zoom_box.y + pan_offset_y, zoom_box.w, zoom_box.h};
        view_port = vp;
        
        return rendererable(target_texture,&view_port,&display,angle,point,flip);
    }

    /** \brief
        *
        * \param
        * \param
        * \return
        *
        */

    void pan_horizontally(double offset)
    {
        offset *= (zoom_box.w * 1.0)/display.w;

        if(pan_offset_x + zoom_box.x + offset >= 0 && offset + pan_offset_x + zoom_box.x <= texture_size.w - zoom_box.w)
        {
            pan_offset_x += offset;
        }
    }

    void pan_vertically(double offset)
    {
        offset *= (zoom_box.h * 1.0)/display.h;

        if(pan_offset_y + zoom_box.y + offset >= 0 && offset + pan_offset_y + zoom_box.y <= texture_size.h - zoom_box.h)
        {
            pan_offset_y += offset;
        }
    }

    void zoom_in()
    {
        // ten is the max scaling size
        if(scale /0.9888 <= 10.0)
        {
            scale /=0.9888;
            zoom();
        }
    }

    void zoom_out()
    {
        if(scale *0.9888 >= 0 && (display.w/(scale*0.9888)) < texture_size.w && (display.h/(scale*0.9888)) < texture_size.h)
        {
            scale *=0.9888;
            zoom();
        }
    }

    void default_zoom()
    {
        scale  = 1;
        zoom();
    }

    vec2d get_size()
    {
        return {display.w,display.h};
    }

    vec2d get_texture_size()
    {
        return texture_size;
    }

    void background_color(SDL_Color cl)
    {
        bg = cl;
    }

    void set_mode(MODE sizing_mode)
    {
        texture_mode = sizing_mode;
    }


    void resize_texture(int t_w, int t_h)
    {
        assigned_display_size.w = std::min(max_texture_size.w, t_w);
        assigned_display_size.h = std::min(max_texture_size.h, t_h);
    }

    void resize(int w, int h)
    {
        assigned_display_size.w = std::min(w,texture_size.w);
        assigned_display_size.h = std::min(h,texture_size.h);
        zoom();// make sure that view data adjust
    }

};

#endif // VIEW_H

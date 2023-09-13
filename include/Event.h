#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "common.h"

class Event
{
public:
    enum class Mode
    {
        text, ordinary
    };
    ///                                     KEY_STATE

    //Note: click appears for only one frame and then turns to held
    enum class key_state
    {
        click, released, held, none
    };

public:
    Mode INPUT_MODE = Mode::ordinary;
    static Event* get_instance(){ return instance = (instance != nullptr)? instance : new Event();};
private:
    Event() = default;
    static Event* instance;
   
    friend class Administrator;

    SDL_Event m_event;
    char* text_char;
    bool text_inputed;
    bool start_text;
    int32_t mouse_x,mouse_y;
    ///                                     KEYS
    struct key
    {
        uint32_t held_duration()
        {
            return (state == key_state::held)? SDL_GetTicks() - start : 0;
        }

        key_state get_state()
        {
            return state;
        }

        void set_state(key_state ks)
        {
            // keep key as held, after sdl returns it to click , if key was not released
            if(state == key_state::held && ks == key_state::click)
                state = key_state::held;
            else
                state = ks;
        }

        void operator()()
        {
            if(state == key_state::released)
            {
                start = 0;
                state = key_state::none;
            }
            else if(state == key_state::click)
            {
                start = SDL_GetTicks();
                state = key_state::held;
            }
        }
    private:
        key_state state = key_state::none;
        uint32_t start = 0;

    };





    void start_text_input()
    {
        start_text = true;
    }

    void stop_text_input()
    {
        start_text = false;
    }

    char* get_text_char()
    {
        return text_char;
    }

    bool has_inputed_text()
    {
        return text_inputed;
    }

    vec2d get_mouse_pos()
    {
        return {mouse_x, mouse_y};
    }

    bool get_modstate( std::string mode_name)
    {
        if(mode_name == "lctrl" || mode_name == "LCTRL")
            return LCTRL;
        else if(mode_name == "rctrl" || mode_name == "RCTRL")
            return RCTRL;
        else if(mode_name == "rshift" || mode_name == "RSHIFT")
            return RSHIFT;
        else if(mode_name == "lshift" || mode_name == "LSHIFT")
            return LSHIFT;
        else if(mode_name == "lalt" || mode_name == "LALT")
            return LALT;
        else if(mode_name == "ralt" || mode_name == "RALT")
            return RALT;
        else if(mode_name == "caps" || mode_name == "CAPS")
            return CAPS;
        else if(mode_name == "num" || mode_name == "NUM")
            return NUM;
        else
            return false;
    }

private:
    ///                           LIST EVENT keys
    key
        mouse_left,mouse_right,left,right,up,down,enter,backspace,home,delete_,end_,pause,tab,esc,greater,less,
        f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,plus,minus,divide,multiply,equals,comma,stop,hash,openParenteces,question,
        semi_colon,quote,colon,ampersand,backslash,forwardslash,openBracket,closeBracket,closeParenteces,space,
        a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,at,dollar, quit;

    ///                            MODE KEYS LIST
    bool LCTRL,RCTRL,LSHIFT,RSHIFT,CAPS,LALT,RALT,NUM;



    key_state get_key_state(std::string key)
    {
        if(key == "mouse_left" || key == "mouse left")
            return mouse_left.get_state();
        else if(key == "mouse_right" || key == "mouse right")
            return mouse_right.get_state();
        else if(key == "left")
            return  left.get_state();
        else if(key == "right")
            return right.get_state();
        else if(key == "up")
            return up.get_state();
        else if(key == "down")
            return down.get_state();
        else if(key == "enter" || key == "return")
            return enter.get_state();
        else if(key == "backspace")
            return backspace.get_state();
        else if(key == "space")
            return space.get_state();
        else if(key == "a")
            return a.get_state();
        else if(key == "b")
            return b.get_state();
        else if(key == "c")
            return c.get_state();
        else if(key == "d")
            return d.get_state();
        else if(key == "e")
            return e.get_state();
        else if(key == "f")
            return f.get_state();
        else if(key == "g")
            return g.get_state();
        else if(key == "h")
            return h.get_state();
        else if(key == "i")
            return i.get_state();
        else if(key == "j")
            return j.get_state();
        else if(key == "k")
            return k.get_state();
        else if(key == "l")
            return l.get_state();
        else if(key == "m")
            return m.get_state();
        else if(key == "n")
            return n.get_state();
        else if(key == "o")
            return o.get_state();
        else if(key == "p")
            return p.get_state();
        else if(key == "q")
            return q.get_state();
        else if(key == "r")
            return r.get_state();
        else if(key == "s")
            return s.get_state();
        else if(key == "t")
            return t.get_state();
        else if(key == "u")
            return u.get_state();
        else if(key == "v")
            return v.get_state();
        else if(key == "w")
            return w.get_state();
        else if(key == "x")
            return x.get_state();
        else if(key == "y")
            return y.get_state();
        else if(key == "z")
            return z.get_state();
        else if(key == "1")
            return _1.get_state();
        else if(key == "2")
            return _2.get_state();
        else if(key == "3")
            return _3.get_state();
        else if(key == "4")
            return _4.get_state();
        else if(key == "5")
            return _5.get_state();
        else if(key == "6")
            return _6.get_state();
        else if(key == "7")
            return _7.get_state();
        else if(key == "8")
            return _8.get_state();
        else if(key == "9")
            return _9.get_state();
        else if(key == "0")
            return _0.get_state();
        else if(key == "f1")
            return f1.get_state();
        else if(key == "f2")
            return f2.get_state();
        else if(key == "f3")
            return f3.get_state();
        else if(key == "f4")
            return f4.get_state();
        else if(key == "f5")
            return f5.get_state();
        else if(key == "f6")
            return f6.get_state();
        else if(key == "f7")
            return f7.get_state();
        else if(key == "f8")
            return f8.get_state();
        else if(key == "f9")
            return f9.get_state();
        else if(key == "f10")
            return f10.get_state();
        else if(key == "f11")
            return f11.get_state();
        else if(key == "home")
            return home.get_state();
        else if(key == "delete")
            return delete_.get_state();
        else if(key == "pause")
            return pause.get_state();
        else if(key == "tab")
            return  tab.get_state();
        else if(key == "esc" || key == "escape")
            return esc.get_state();
        else if(key == "greater_than" || key == "greater" || key == ">")
            return greater.get_state();
        else if(key == "lesser" || key == "lesser_than" || key == "<")
            return less.get_state();
        else if(key == "plus" || key == "+")
            return plus.get_state();
        else if(key == "minus" || key == "-")
            return minus.get_state();
        else if(key == "divide" || key == "/")
            return divide.get_state();
        else if(key == "multiply" || key == "*")
            return multiply.get_state();
        else if(key == "quit")
            return quit.get_state();
        else if(key == "=" || key == "equals" )
            return equals.get_state();
        else if(key == "@")
            return at.get_state();
        else if(key == "$")
            return dollar.get_state();
        else if(key == "!")
            return quote.get_state();
        else if(key == "?")
            return question.get_state();
        else if(key == ",")
            return comma.get_state();
        else if(key == ".")
            return stop.get_state();
        else if(key == ";")
            return semi_colon.get_state();
        else if(key == ":")
            return colon.get_state();
        else if(key == "[")
            return openBracket.get_state();
        else if(key == "]")
            return closeBracket.get_state();
        else if(key == "(")
            return openParenteces.get_state();
        else if(key == ")")
            return closeParenteces.get_state();
        else if(key == "#")
            return hash.get_state();
        else if(key == "&")
            return ampersand.get_state();
        else if(key == "\\")
            return backslash.get_state();
        else if(key == "/")
            return forwardslash.get_state();
        else
            return key_state::none;
    }

    ///                           RUN EVENT
    void operator()()
    {

        Mode INPUT_MODE = Mode::ordinary;
        text_inputed = false;
        // run event keys
        mouse_left(); mouse_right(); left(); right(); up(); down(); enter(); backspace(); home(); delete_(); end_(); pause(); tab(); esc();
        f1(); f2(); f3(); f4(); f5(); f6(); f7(); f8(); f9(); f10(); f11(); f12(); plus(); minus(); divide(); multiply(); comma(); stop(); hash(); openParenteces();
        semi_colon(); quote(); colon(); ampersand(); backslash(); forwardslash(); openBracket(); closeBracket(); closeParenteces(); greater(); question();
        a(); b(); c(); d(); e(); f(); g(); h(); i(); j(); k(); l(); m(); n(); o(); p(); q(); r(); s(); t(); u(); v(); w(); x(); y(); z(); less();
        _0(); _1(); _2(); _3(); _4(); _5(); _6(); _7(); _8(); _9(); at(); dollar(); quit(); space(); equals();

        //   set key mods
        LSHIFT = (SDL_GetModState() & KMOD_LSHIFT)? true : false;
        RSHIFT = (SDL_GetModState() & KMOD_RSHIFT)? true : false;
        LCTRL =  (SDL_GetModState() & KMOD_LCTRL)? true : false;
        RCTRL =  (SDL_GetModState() & KMOD_RCTRL)? true : false;
        LALT =   (SDL_GetModState() & KMOD_LALT)? true : false;
        RALT =   (SDL_GetModState() & KMOD_RALT)? true : false;
        CAPS =   (SDL_GetModState() & KMOD_CAPS)? true : false;
        NUM  =   (SDL_GetModState() & KMOD_NUM)? true : false;

        if(start_text && !SDL_IsTextInputActive())
            SDL_StartTextInput();
        else if(SDL_IsTextInputActive()) SDL_StopTextInput();

        while(SDL_PollEvent(&m_event))
        {
            ///             MOUSE EVENT
            switch (m_event.type)
            {
                case SDL_QUIT:
                quit.set_state(key_state::click);
                break;
                case SDL_MOUSEMOTION:
                mouse_x = m_event.motion.x;
                mouse_y = m_event.motion.y;
                break;
                case SDL_MOUSEBUTTONDOWN:
                if(m_event.button.button == SDL_BUTTON_LEFT)
                    mouse_left.set_state(key_state::click);
                else if(m_event.button.button == SDL_BUTTON_RIGHT)
                    mouse_right.set_state(key_state::click);
                break;
                case SDL_MOUSEBUTTONUP:
                if(m_event.button.button == SDL_BUTTON_LEFT)
                    mouse_left.set_state(key_state::released);
                else if(m_event.button.button == SDL_BUTTON_RIGHT)
                    mouse_right.set_state(key_state::released);
                break;
            }

            ///                     TEXT INPUT
            if(SDL_IsTextInputActive() && m_event.type == SDL_TEXTINPUT)
            {
                if(!(LCTRL && RCTRL && LALT && RALT))
                {
                    text_char = m_event.text.text;
                    text_inputed = true;
                }
                INPUT_MODE = Mode::text;
                continue;
            }


            ///                     SIMPLE KEYBOARD INPUT
            if(m_event.type == SDL_KEYDOWN)
            {
                switch(m_event.key.keysym.sym)
                {
                case SDLK_LEFT:
                    left.set_state(key_state::click);
                    break;
                case SDLK_RIGHT:
                    right.set_state(key_state::click);
                    break;
                case SDLK_UP:
                    up.set_state(key_state::click);
                    break;
                case SDLK_DOWN:
                    down.set_state(key_state::click);
                    break;
                case SDLK_F1:
                    f1.set_state(key_state::click);
                    break;
                case SDLK_F2:
                    f2.set_state(key_state::click);
                    break;
                case SDLK_F3:
                    f3.set_state(key_state::click);
                    break;
                case SDLK_F4:
                    f4.set_state(key_state::click);
                    break;
                case SDLK_F5:
                    f5.set_state(key_state::click);
                    break;
                case SDLK_F6:
                    f6.set_state(key_state::click);
                    break;
                case SDLK_F7:
                    f7.set_state(key_state::click);
                    break;
                case SDLK_F8:
                    f8.set_state(key_state::click);
                    break;
                case SDLK_F9:
                    f9.set_state(key_state::click);
                    break;
                case SDLK_F10:
                    f10.set_state(key_state::click);
                    break;
                case SDLK_F11:
                    f11.set_state(key_state::click);
                    break;
                case SDLK_F12:
                    f12.set_state(key_state::click);
                    break;
                case SDLK_PLUS:
                    plus.set_state(key_state::click);
                    break;
                case SDLK_MINUS:
                    minus.set_state(key_state::click);
                    break;
                case SDLK_KP_MULTIPLY:
                    multiply.set_state(key_state::click);
                    break;
                case SDLK_KP_DIVIDE:
                    divide.set_state(key_state::click);
                    break;
                case SDLK_AT:
                    at.set_state(key_state::click);
                    break;
                case SDLK_DOLLAR:
                    dollar.set_state(key_state::click);
                    break;
                case SDLK_EQUALS:
                    equals.set_state(key_state::click);
                    break;
                case SDLK_0:
                    _0.set_state(key_state::click);
                    break;
                case SDLK_1:
                    _1.set_state(key_state::click);
                    break;
                case SDLK_2:
                    _2.set_state(key_state::click);
                    break;
                case SDLK_3:
                    _3.set_state(key_state::click);
                    break;
                case SDLK_4:
                    _4.set_state(key_state::click);
                    break;
                case SDLK_5:
                    _5.set_state(key_state::click);
                    break;
                case SDLK_6:
                    _6.set_state(key_state::click);
                    break;
                case SDLK_7:
                    _7.set_state(key_state::click);
                    break;
                case SDLK_8:
                    _8.set_state(key_state::click);
                    break;
                case SDLK_9:
                    _9.set_state(key_state::click);
                    break;
                case SDLK_a:
                    a.set_state(key_state::click);
                    break;
                case SDLK_b:
                    b.set_state(key_state::click);
                    break;
                case SDLK_c:
                    c.set_state(key_state::click);
                    break;
                case SDLK_d:
                    d.set_state(key_state::click);
                    break;
                case SDLK_e:
                    e.set_state(key_state::click);
                    break;
                case SDLK_f:
                    f.set_state(key_state::click);
                    break;
                case SDLK_g:
                    g.set_state(key_state::click);
                    break;
                case SDLK_h:
                    h.set_state(key_state::click);
                    break;
                case SDLK_i:
                    i.set_state(key_state::click);
                    break;
                case SDLK_j:
                    j.set_state(key_state::click);
                    break;
                case SDLK_k:
                    k.set_state(key_state::click);
                    break;
                case SDLK_l:
                    l.set_state(key_state::click);
                    break;
                case SDLK_m:
                    m.set_state(key_state::click);
                    break;
                case SDLK_n:
                    n.set_state(key_state::click);
                    break;
                case SDLK_o:
                    o.set_state(key_state::click);
                    break;
                case SDLK_p:
                    p.set_state(key_state::click);
                    break;
                case SDLK_q:
                    q.set_state(key_state::click);
                    break;
                case SDLK_r:
                    r.set_state(key_state::click);
                    break;
                case SDLK_s:
                    s.set_state(key_state::click);
                    break;
                case SDLK_t:
                    t.set_state(key_state::click);
                    break;
                case SDLK_u:
                    u.set_state(key_state::click);
                    break;
                case SDLK_v:
                    v.set_state(key_state::click);
                    break;
                case SDLK_w:
                    w.set_state(key_state::click);
                    break;
                case SDLK_x:
                    x.set_state(key_state::click);
                    break;
                case SDLK_y:
                    y.set_state(key_state::click);
                    break;
                case SDLK_z:
                    z.set_state(key_state::click);
                    break;
                case SDLK_COMMA:
                    comma.set_state(key_state::click);
                    break;
                case SDLK_STOP:
                    stop.set_state(key_state::click);
                    break;
                case SDLK_BACKSLASH:
                    backslash.set_state(key_state::click);
                    break;
                case SDLK_SLASH:
                    forwardslash.set_state(key_state::click);
                    break;
                case SDLK_LEFTBRACKET:
                    openBracket.set_state(key_state::click);
                    break;
                case SDLK_RIGHTBRACKET:
                    closeBracket.set_state(key_state::click);
                    break;
                case SDLK_LEFTPAREN:
                    openParenteces.set_state(key_state::click);
                    break;
                case SDLK_RIGHTPAREN:
                    closeParenteces.set_state(key_state::click);
                    break;
                case SDLK_SEMICOLON:
                    semi_colon.set_state(key_state::click);
                    break;
                case SDLK_COLON:
                    colon.set_state(key_state::click);
                    break;
                case SDLK_AMPERSAND:
                    ampersand.set_state(key_state::click);
                    break;
                case SDLK_RETURN:
                    enter.set_state(key_state::click);
                    break;
                case SDLK_DELETE:
                    delete_.set_state(key_state::click);
                    break;
                case SDLK_END:
                    end_.set_state(key_state::click);
                    break;
                case SDLK_PAUSE:
                    pause.set_state(key_state::click);
                    break;
                case SDLK_HASH:
                    hash.set_state(key_state::click);
                    break;
                case SDLK_GREATER:
                    greater.set_state(key_state::click);
                    break;
                case SDLK_LESS:
                    less.set_state(key_state::click);
                    break;
                case SDLK_QUOTE:
                    quote.set_state(key_state::click);
                    break;
                case SDLK_QUESTION:
                    question.set_state(key_state::click);
                    break;
                case SDLK_TAB:
                    tab.set_state(key_state::click);
                    break;
                case SDLK_BACKSPACE:
                    backspace.set_state(key_state::click);
                    break;
                case SDLK_SPACE:
                    space.set_state(key_state::click);
                    break;
                case SDLK_ESCAPE:
                    esc.set_state(key_state::click);
                    break;
                }
            }// KEYDOWN


            ///                 KEY UP
            else if(m_event.type == SDL_KEYUP)
            {
                switch(m_event.key.keysym.sym)
                {
                case SDLK_LEFT:
                    left.set_state(key_state::released);
                    break;
                case SDLK_RIGHT:
                    right.set_state(key_state::released);
                    break;
                case SDLK_UP:
                    up.set_state(key_state::released);
                    break;
                case SDLK_DOWN:
                    down.set_state(key_state::released);
                    break;
                case SDLK_F1:
                    f1.set_state(key_state::released);
                    break;
                case SDLK_F2:
                    f2.set_state(key_state::released);
                    break;
                case SDLK_F3:
                    f3.set_state(key_state::released);
                    break;
                case SDLK_F4:
                    f4.set_state(key_state::released);
                    break;
                case SDLK_F5:
                    f5.set_state(key_state::released);
                    break;
                case SDLK_F6:
                    f6.set_state(key_state::released);
                    break;
                case SDLK_F7:
                    f7.set_state(key_state::released);
                    break;
                case SDLK_F8:
                    f8.set_state(key_state::released);
                    break;
                case SDLK_F9:
                    f9.set_state(key_state::released);
                    break;
                case SDLK_F10:
                    f10.set_state(key_state::released);
                    break;
                case SDLK_F11:
                    f11.set_state(key_state::released);
                    break;
                case SDLK_F12:
                    f12.set_state(key_state::released);
                    break;
                case SDLK_PLUS:
                    plus.set_state(key_state::released);
                    break;
                case SDLK_MINUS:
                    minus.set_state(key_state::released);
                    break;
                case SDLK_KP_MULTIPLY:
                    multiply.set_state(key_state::released);
                    break;
                case SDLK_KP_DIVIDE:
                    divide.set_state(key_state::released);
                    break;
                case SDLK_EQUALS:
                    equals.set_state(key_state::released);
                    break;
                case SDLK_AT:
                    at.set_state(key_state::released);
                    break;
                case SDLK_DOLLAR:
                    dollar.set_state(key_state::released);
                    break;
                case SDLK_0:
                    _0.set_state(key_state::released);
                    break;
                case SDLK_1:
                    _1.set_state(key_state::released);
                    break;
                case SDLK_2:
                    _2.set_state(key_state::released);
                    break;
                case SDLK_3:
                    _3.set_state(key_state::released);
                    break;
                case SDLK_4:
                    _4.set_state(key_state::released);
                    break;
                case SDLK_5:
                    _5.set_state(key_state::released);
                    break;
                case SDLK_6:
                    _6.set_state(key_state::released);
                    break;
                case SDLK_7:
                    _7.set_state(key_state::released);
                    break;
                case SDLK_8:
                    _8.set_state(key_state::released);
                    break;
                case SDLK_9:
                    _9.set_state(key_state::released);
                    break;
                case SDLK_a:
                    a.set_state(key_state::released);
                    break;
                case SDLK_b:
                    b.set_state(key_state::released);
                    break;
                case SDLK_c:
                    c.set_state(key_state::released);
                    break;
                case SDLK_d:
                    d.set_state(key_state::released);
                    break;
                case SDLK_e:
                    e.set_state(key_state::released);
                    break;
                case SDLK_f:
                    f.set_state(key_state::released);
                    break;
                case SDLK_g:
                    g.set_state(key_state::released);
                    break;
                case SDLK_h:
                    h.set_state(key_state::released);
                    break;
                case SDLK_i:
                    i.set_state(key_state::released);
                    break;
                case SDLK_j:
                    j.set_state(key_state::released);
                    break;
                case SDLK_k:
                    k.set_state(key_state::released);
                    break;
                case SDLK_l:
                    l.set_state(key_state::released);
                    break;
                case SDLK_m:
                    m.set_state(key_state::released);
                    break;
                case SDLK_n:
                    n.set_state(key_state::released);
                    break;
                case SDLK_o:
                    o.set_state(key_state::released);
                    break;
                case SDLK_p:
                    p.set_state(key_state::released);
                    break;
                case SDLK_q:
                    q.set_state(key_state::released);
                    break;
                case SDLK_r:
                    r.set_state(key_state::released);
                    break;
                case SDLK_s:
                    s.set_state(key_state::released);
                    break;
                case SDLK_t:
                    t.set_state(key_state::released);
                    break;
                case SDLK_u:
                    u.set_state(key_state::released);
                    break;
                case SDLK_v:
                    v.set_state(key_state::released);
                    break;
                case SDLK_w:
                    w.set_state(key_state::released);
                    break;
                case SDLK_x:
                    x.set_state(key_state::released);
                    break;
                case SDLK_y:
                    y.set_state(key_state::released);
                    break;
                case SDLK_z:
                    z.set_state(key_state::released);
                    break;
                case SDLK_COMMA:
                    comma.set_state(key_state::released);
                    break;
                case SDLK_STOP:
                    stop.set_state(key_state::released);
                    break;
                case SDLK_BACKSLASH:
                    backslash.set_state(key_state::released);
                    break;
                case SDLK_SLASH:
                    forwardslash.set_state(key_state::released);
                    break;
                case SDLK_LEFTBRACKET:
                    openBracket.set_state(key_state::released);
                    break;
                case SDLK_RIGHTBRACKET:
                    closeBracket.set_state(key_state::released);
                    break;
                case SDLK_LEFTPAREN:
                    openParenteces.set_state(key_state::released);
                    break;
                case SDLK_RIGHTPAREN:
                    closeParenteces.set_state(key_state::released);
                    break;
                case SDLK_SEMICOLON:
                    semi_colon.set_state(key_state::released);
                    break;
                case SDLK_COLON:
                    colon.set_state(key_state::released);
                    break;
                case SDLK_AMPERSAND:
                    ampersand.set_state(key_state::released);
                    break;
                case SDLK_RETURN:
                    enter.set_state(key_state::released);
                    break;
                case SDLK_DELETE:
                    delete_.set_state(key_state::released);
                    break;
                case SDLK_END:
                    end_.set_state(key_state::released);
                    break;
                case SDLK_PAUSE:
                    pause.set_state(key_state::released);
                    break;
                case SDLK_HASH:
                    hash.set_state(key_state::released);
                    break;
                case SDLK_GREATER:
                    greater.set_state(key_state::released);
                    break;
                case SDLK_LESS:
                    less.set_state(key_state::released);
                    break;
                case SDLK_QUOTE:
                    quote.set_state(key_state::released);
                    break;
                case SDLK_QUESTION:
                    question.set_state(key_state::released);
                    break;
                case SDLK_TAB:
                    tab.set_state(key_state::released);
                    break;
                case SDLK_BACKSPACE:
                    backspace.set_state(key_state::released);
                    break;
                case SDLK_SPACE:
                    space.set_state(key_state::released);
                    break;
                case SDLK_ESCAPE:
                    esc.set_state(key_state::released);
                    break;
                }

            }// KEYUP

        }// pollEvent

    }// RUN EVENT

};


#endif // EVENT_H_INCLUDED

#pragma once


#include "component.h"




class text_input : component
{
    struct char_info
    {
        char_info(uint8_t i ,uint16_t w)
        :index(i), width(w){}
        uint8_t index = 0;
        uint16_t width = 0;
    };

    enum class POINTER_SHIFT { RIGHT, LEFT, NONE}           shift_pointer = POINTER_SHIFT::NONE;
    enum class DELETE_TYPE   { FRONT, BACK, NONE}           delete_type = DELETE_TYPE::NONE;
    enum class SELECT_STAGE  { HEAD, HEAD_AND_TAIL, NONE}   select_stage = SELECT_STAGE::NONE;
    enum class TIMER_STAGE   { TRIGGERD, DELAY, COUNT, NONE}       timer_stage = TIMER_STAGE::NONE;

    vec2d mouse;

    std::string text; // total text in input
    std::string visible_text;// part text visible to user

    SDL_Rect text_box;
    SDL_Rect select_box;

    text_renderer* TR;
    text_renderer* PTR;

    bool blink_pointer;
    uint32_t ptr_timer;
    uint32_t timer;
    uint8_t char_width;
    int16_t text_header;
    int8_t front_offset;
    int8_t back_offset;
    uint8_t padding;

    std::list<char_info> char_stack;
    std::list<char_info>::iterator pointer;
    std::list<char_info>::iterator select_head;
    std::list<char_info>::iterator select_tail;
    std::list<char_info>::iterator _select_head;
    std::list<char_info>::iterator _select_tail;

public:
    ~text_input()
    {
        delete TR;
        delete PTR;
    }

    text_input(View* view, std::string id, int x, int y, int w = 250, SDL_Color fg = {20,50,20,255}, SDL_Color p_fg = {255,0,0,255}, int padding = 20)
    :component(view,id,x,y,w,0), padding(padding)
    {
        TR = new text_renderer(view,"",fg);
        PTR = new text_renderer(view,"|",p_fg);
        this->size.h = padding + TR->get("font_size");
        text_box = {pos.x + padding, this->pos.y + padding/2, size.w - padding*2, (int)TR->get("font_size")};
        in_focus = timer = char_width = text_header = front_offset = back_offset = 0;
    }


    void Disable() override
    {
        is_active = false;
        Administrator::get_instance()->stop_text_input(view,id);
    }

    void Enable() override
    {
        if(view)
            is_active = true;
    }

    void Logic(const vec2d _mouse) override
    {
        if(is_active)
        {
            this->mouse = view->resolve_point(_mouse);

            on_hover = (mouse.x >= pos.x && mouse.x <= pos.x + size.w && mouse.y >= pos.y && mouse.y <= pos.y + size.h);

            if(on_hover == true && Administrator::get_instance()->get_key_state(this->view,"mouse_left") == Event::key_state::click)
                in_focus = true;
            else if (Administrator::get_instance()->GET_ACTIVE_VIEW() != view   ||  (on_hover != true && Administrator::get_instance()->get_key_state(this->view,"mouse_left") == Event::key_state::click)
                                    ||   (on_hover != true && Administrator::get_instance()->get_key_state(this->view,"mouse_right") == Event::key_state::click))
                in_focus = false;

            // shift pointer
            if(in_focus && Administrator::get_instance()->get_key_state(this->view,"left") == Event::key_state::click)
            {
                shift_pointer = POINTER_SHIFT::LEFT;
                timer_stage = TIMER_STAGE::TRIGGERD;
                blink_pointer = false;
            }
            else if(in_focus && Administrator::get_instance()->get_key_state(this->view,"right") == Event::key_state::click)
            {
                shift_pointer = POINTER_SHIFT::RIGHT;
                timer_stage = TIMER_STAGE::TRIGGERD;
                blink_pointer = false;
            }
            //  delete text
            else if(in_focus && Administrator::get_instance()->get_key_state(this->view,"backspace") == Event::key_state::click)
            {
                delete_type = DELETE_TYPE::BACK;
                timer_stage = TIMER_STAGE::TRIGGERD;
                blink_pointer = false;
            }
            else if(in_focus && Administrator::get_instance()->get_key_state(this->view,"delete") == Event::key_state::click)
            {
                delete_type = DELETE_TYPE::FRONT;
                timer_stage = TIMER_STAGE::TRIGGERD;
                blink_pointer = false;
            }
            else if(in_focus && ((Administrator::get_instance()->get_key_state(this->view,"delete") == Event::key_state::released) ||
                    (Administrator::get_instance()->get_key_state(this->view,"backspace") == Event::key_state::released) ||
                    (Administrator::get_instance()->get_key_state(this->view,"left") == Event::key_state::released) ||
                    (Administrator::get_instance()->get_key_state(this->view,"right") == Event::key_state::released)))
            {
                timer_stage = TIMER_STAGE::NONE;
                delete_type = DELETE_TYPE::NONE;
                shift_pointer = POINTER_SHIFT::NONE;
                blink_pointer = true;
            }


        }
    }

    void Update() override
    {
        if(is_active)
        {
            if(in_focus && TR->is_text_active())
            {
                std::cout << "in text input update\n";
                ///                        POINTER
                POINTER_AND_SELECTION();
                ///                         MODIFY TEXT
                MODIFY_TEXT();

                ///                        INSERTING TEXT
                Administrator::get_instance()->start_text_input(view,id);
                if(Administrator::get_instance()->has_inputed_text(view) == true)
                {
                    if(select_stage == SELECT_STAGE::HEAD_AND_TAIL)
                    {
                        DELETE_SELECTED();
                        select_stage = SELECT_STAGE::NONE;
                    }
                    char* __chars = Administrator::get_instance()->get_inputed_text(view);
                    std::string chars(__chars);
                    for(int i = 0; i < chars.length(); i++)
                        if(__chars) ADD_CHAR(chars[i]);

                    blink_pointer = false;
                }
                else if(delete_type == DELETE_TYPE::NONE && shift_pointer == POINTER_SHIFT::NONE)
                    blink_pointer = true;

                ///                         DELETE TEXT
                if(delete_type == DELETE_TYPE::BACK || delete_type == DELETE_TYPE::FRONT )
                    select_stage = SELECT_STAGE::NONE;

                if(TIMER_GATE())
                    REMOVE_CHAR();


            }
            else
                Administrator::get_instance()->stop_text_input(view,id);
        }
    }

    void Render(SDL_Rect* clip_border = NULL) override
    {
        if(is_active && TR->is_text_active())
        {
            TR->set("y", text_box.y);
            PTR->set("y", text_box.y);

            TR->set("x", text_box.x);
            PTR->set("x", (char_stack.size() > 0)? text_box.x + (pointer->width - text_header) - PTR->get("w")/2.0 : text_box.x);

            select_box.y = text_box.y;
            select_box.h = text_box.h;

            if(char_stack.size() > 0)
            {
                vec2d __size = TR->Load_text(visible_text);
                SDL_Rect view_port = { front_offset, 0, __size.w - (front_offset + back_offset), __size.h};
                TR->Render(clip_border, &view_port);
            }

            if(select_stage == SELECT_STAGE::HEAD_AND_TAIL)
                view->RenderFillRect(clip_border,&select_box,{0, 200,200, 70},SDL_BlendMode::SDL_BLENDMODE_BLEND);

            if(POINTER_BLINKER())
                PTR->Render(clip_border);
        }
    }

    bool POINTER_BLINKER()
    {
        if(blink_pointer == true && in_focus)
        {
            if(SDL_GetTicks() - ptr_timer < 500)
                return true;
            else if(SDL_GetTicks() - ptr_timer > 1000)
                ptr_timer = SDL_GetTicks();

            return false;
        }
        else
            ptr_timer = SDL_GetTicks();
        return true;
    }

    bool TIMER_GATE()
    {
        if(timer_stage == TIMER_STAGE::TRIGGERD)
        {
            timer = SDL_GetTicks();
            timer_stage = TIMER_STAGE::DELAY;
            return true;
        }
        else if(timer_stage == TIMER_STAGE::DELAY)
        {
            if(SDL_GetTicks() - timer > 500)
            {
                timer_stage = TIMER_STAGE::COUNT;
                timer = SDL_GetTicks();
                return true;
            }
        }
        else if(timer_stage == TIMER_STAGE::COUNT)
        {
            if(SDL_GetTicks() - timer > 30)
            {
                timer = SDL_GetTicks();
                return true;
            }
        }
        return false;
    }

    std::string get_selected_text()
    {
        if(select_stage == SELECT_STAGE::HEAD_AND_TAIL)
            return substr(text,_select_head->index, _select_tail->index+1);
        else return "";
    }


    std::string get_text()
    {
        return text;
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
        else if(key == "padding")
        {
            padding = value;
            size.h = TR->get("font_size");
        }
        else if(key == "font size" || key == "font_size")
        {
            TR->set("font_size",value);
            PTR->set("font_size",value);
            size.h = TR->get("font_size");
        }
        text_box = {pos.x + padding, this->pos.y + padding/2, size.w - padding*2, (int)TR->get("font_size")};
    }
private:

    uint16_t current_char_width(std::list<char_info>::iterator pointer)
    {
        if(pointer == char_stack.begin())
            return pointer->width;
        else if(pointer == char_stack.end())
            return 0;
        else
        {
            auto j = pointer; --j;
            return (pointer->width - j->width );
        }
    }

    ///                                      TEXT INPUT INTERNALS

    void RESOLVE_VIEW()
    {
        if(char_stack.size() > 0)
        {
            // adjust text header to display only useful text
            if(pointer == char_stack.end()  && pointer == char_stack.begin() && (--char_stack.end())->width < text_box.w)
                text_header = 0;

            else if(pointer->width > text_header + text_box.w)
                text_header = pointer->width - text_box.w;

            else if(pointer->width < text_header)
                text_header = pointer->width;

            else if(delete_type == DELETE_TYPE::FRONT && text_header + text_box.w > (--char_stack.end())->width)
                text_header -= ((text_header + text_box.w) - (--char_stack.end())->width);

            else if(delete_type == DELETE_TYPE::BACK)
                text_header -= char_width;


            // make sure text header is never less than zero
            if(text_header < 0) text_header = 0;

            std::list<char_info>::iterator head;
            std::list<char_info>::iterator tail;

            front_offset = back_offset = 0;
            int prev = 0;
            // get starting char
            for(auto i = char_stack.begin(); i != char_stack.end(); ++i)
            {
                if(text_header == 0)
                {
                    head = i; break;
                }
                else if(i->width == text_header)
                {
                    head = i; ++head; break;
                }
                else if(i->width > text_header && text_header > prev)
                {
                    head = i; front_offset = text_header - prev; break;
                }
                prev = i->width;
            }

            // get ending char
            for(auto i = head; i != char_stack.end(); ++i)
            {
                if(i->width == text_header+text_box.w)
                {
                    tail = i; break;
                }
                else if(i->width > text_header+text_box.w && text_header+text_box.w > prev)
                {
                    tail = i; back_offset = i->width - (text_header+text_box.w); break;
                }
                else if(i == --char_stack.end())
                    tail = i;
                prev = i->width;
            }


            // get visible text with info above
            visible_text = substr(text,head->index, tail->index+1);
        }
    }

    void POINTER_AND_SELECTION()
    {
        if(char_stack.size() > 0)
        {
            if(shift_pointer == POINTER_SHIFT::LEFT && (TIMER_GATE()))
            {
                if(pointer != char_stack.end())
                    --pointer;
                RESOLVE_VIEW();
            }
            else if(shift_pointer == POINTER_SHIFT::RIGHT && (TIMER_GATE()))
            {
                if(pointer != --char_stack.end())
                    ++pointer;
                RESOLVE_VIEW();
            }
            else if(on_hover && Administrator::get_instance()->get_key_state(view,"mouse_left") == Event::key_state::held)
            {
                if(SDL_GetTicks() - timer > 50)
                {
                    int prev = 0;
                    int pos = (mouse.x - text_box.x) + text_header;
                    for(auto i = char_stack.begin(); i != char_stack.end(); ++i)
                    {
                        if(i->width == pos && i != --char_stack.end())
                        {
                            pointer = i; ++pointer; break;
                        }
                        else if(pos > prev && pos < i->width)
                        {
                            pointer = i; break;
                        }
                        else if(pos > (--char_stack.end())->width)
                        {
                            pointer = --char_stack.end(); break;
                        }
                        else if(pos <= 0)
                        {
                            pointer = char_stack.end(); break;
                        }
                        prev = i->width;
                    }


                    if(select_stage == SELECT_STAGE::NONE)
                    {
                        select_head = pointer;
                        select_stage = SELECT_STAGE::HEAD;
                    }
                    else if(select_stage == SELECT_STAGE::HEAD && select_head == pointer)
                        select_stage = SELECT_STAGE::HEAD;

                    else if(select_stage == SELECT_STAGE::HEAD || select_stage == SELECT_STAGE::HEAD_AND_TAIL)
                    {
                        select_tail = pointer;
                        select_stage = SELECT_STAGE::HEAD_AND_TAIL;
                    }
                    timer = SDL_GetTicks();
                }
                RESOLVE_VIEW();
            }
            else if(Administrator::get_instance()->get_key_state(view,"mouse_left") == Event::key_state::click)
            {
                select_stage = SELECT_STAGE::NONE;
            }
        }
    }

    void ADD_CHAR(char _char)
    {
        if(text.length() < 255)
        {
            // get size of char when combined with other chars
            std::string str1 = "Hello", str2 = std::string("He") + _char + std::string("llo");
            vec2d _1 = TR->Load_text(str1);
            vec2d _2 = TR->Load_text(str2);
            char_width = _2.w - _1.w;

            if(char_width <= TR->get("font size") && char_width > 0)
            {
                // pointer is after all text
                if(char_stack.size() == 0 || pointer == --char_stack.end())
                {
                    text += _char;
                    char_stack.emplace(char_stack.end() , char_stack.size(), (char_stack.size() == 0)? char_width : (--char_stack.end())->width + char_width );

                    pointer = --char_stack.end();
                    RESOLVE_VIEW();
                }
                // pointer is in b/w chars
                else if( pointer != --char_stack.end() && pointer != char_stack.end())
                {
                    for(auto i = --char_stack.end(); i != char_stack.end(); --i)
                    {
                        if(pointer == i)
                        {
                            auto j = i; ++j;
                            std::string h; h += _char;
                            text = insert(text,i->index+1,h);
                            char_stack.emplace(j,i->index+1, i->width + char_width);
                            ++pointer;
                            RESOLVE_VIEW();
                            break;
                        }
                        i->width += char_width;
                        i->index += 1;
                    }
                }
                // pointer is b/f text
                else if(pointer == char_stack.end())
                {
                    for(auto i = --char_stack.end(); i != char_stack.end(); --i)
                    {
                        i->width += char_width;
                        i->index += 1;
                    }
                    std::string h; h += _char;
                    text = insert(text,0,h);
                    char_stack.emplace(char_stack.begin(),0, char_width);
                    pointer = char_stack.begin();
                    RESOLVE_VIEW();
                }

            }
            else
            {
                TR->clear();
                char_width = 0;
            }
        }
    }

    void REMOVE_CHAR()
    {
        if(char_stack.size() > 0)
        {
            if(delete_type == DELETE_TYPE::BACK)// with backspace button
            {
                char_width = current_char_width(pointer);

                // pointer is after all text
                if(pointer == --char_stack.end())
                {
                    text = substr(text,0, text.length()-1);
                    char_stack.erase(--char_stack.end());

                    pointer = --char_stack.end();
                    RESOLVE_VIEW();
                }
                // pointer is in b/w chars
                else if( pointer != --char_stack.end() && pointer != char_stack.end())
                {
                    for(auto i = --char_stack.end(); i != char_stack.end(); --i)
                    {
                        if(pointer == i)
                        {
                            --pointer;
                            text = remove(text,i->index);
                            char_stack.erase(i);
                            RESOLVE_VIEW();
                            break;
                        }
                        i->width -= char_width;
                        i->index -= 1;
                    }
                }
            }
            else if(delete_type == DELETE_TYPE::FRONT)// with delete button
            {
                auto e = pointer; ++e;
                char_width = current_char_width(e);

                // pointer is before all text
                if(pointer == char_stack.end())
                {
                    char_width = char_stack.begin()->width;

                    for(auto i = --char_stack.end(); i != char_stack.begin(); --i)
                    {
                        i->width -= char_width;
                        i->index -= 1;
                    }
                    text = remove(text,0);
                    char_stack.erase(char_stack.begin());
                    RESOLVE_VIEW();
                }
                // pointer is in b/w chars
                else if( pointer != --char_stack.end() && pointer != char_stack.end())
                {
                    for(auto i = --char_stack.end(); i != char_stack.end(); --i)
                    {
                        if(pointer == i)
                        {
                            text = remove(text,i->index+1);
                            char_stack.erase(++i);
                            RESOLVE_VIEW();
                            break;
                        }
                        i->width -= char_width;
                        i->index -= 1;
                    }
                }
            }
        }
    }

    void MODIFY_TEXT()
    {
        bool ctrl =  (Administrator::get_instance()->get_modstate(view,"LCTRL") || Administrator::get_instance()->get_modstate(view,"RCTRL"));
        ///                   PASTE
        if(ctrl && Administrator::get_instance()->get_key_state(view,"v") == Event::key_state::click)
        {
            DELETE_SELECTED();
            std::string CBT = SDL_GetClipboardText();

            for(int i = 0; i < CBT.length(); i++)
                ADD_CHAR(CBT[i]);
        }
        ///                    SELECT ALL
        else if(ctrl && Administrator::get_instance()->get_key_state(view,"a") == Event::key_state::click)
        {
            select_head = char_stack.end();
            select_tail = --char_stack.end();
            select_stage = SELECT_STAGE::HEAD_AND_TAIL;
        }


        if(select_stage == SELECT_STAGE::HEAD_AND_TAIL)
        {
            SET_HEAD_AND_TAIL();

            ///             COPY
            if(ctrl && Administrator::get_instance()->get_key_state(view,"c") == Event::key_state::click)
            {
                COPY();
            }
            ///             CUT
            if(ctrl && Administrator::get_instance()->get_key_state(view,"x") == Event::key_state::click)
            {
                CUT();
            }
            ///             DELETE
            else if(delete_type != DELETE_TYPE::NONE)
            {
                DELETE_SELECTED();
            }
        }
        else if(select_stage == SELECT_STAGE::NONE)
            select_box = {0,0,0,0};
    }

    void SET_HEAD_AND_TAIL()
    {
        _select_head = select_head;
        _select_tail = select_tail;

        if(_select_tail == char_stack.end() && _select_head != char_stack.end())
            std::swap(_select_head,_select_tail);
        else if(_select_head != char_stack.end())
            if(_select_head->index > _select_tail->index)
                std::swap(_select_head,_select_tail);

        ///                           set select box
        int pos1 = (_select_head == char_stack.end())? 0 : _select_head->width;
        int pos2 = _select_tail->width;

        ++_select_head;

        if(pos1 > text_header && pos1 <= text_header+text_box.w)
            select_box.x = text_box.x + (pos1 - text_header);
        else
            select_box.x = text_box.x;

        if((pos1 < text_header && pos2 < text_header) || (pos1 > text_header+text_box.w && pos2 > text_header+text_box.w))
            select_box.w = 0;
        else if(pos2 > text_header+text_box.w)
            select_box.w = (text_box.x+text_box.w) - select_box.x;
        else if(pos2 <= text_header+text_box.w)
            select_box.w = pos2 - (text_header + select_box.x - text_box.x);

    }

    void COPY()
    {
        std::string hold = substr(text,_select_head->index, _select_tail->index+1);
        SDL_SetClipboardText(hold.c_str());
    }

    void DELETE_SELECTED()
    {
        if(select_stage == SELECT_STAGE::HEAD_AND_TAIL)
        {
            int count = _select_tail->index - _select_head->index+1;
            pointer = _select_tail;
            delete_type = DELETE_TYPE::BACK;
            for(int i = 0; i < count; i++)
                REMOVE_CHAR();
            delete_type = DELETE_TYPE::NONE;
            select_stage = SELECT_STAGE::NONE;
        }
    }

    void CUT()
    {
        COPY();
        DELETE_SELECTED();
    }

};


#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <system_error>
#include <deque>


class Error
{
    // A thread safe queue of error messages
    static std::deque<std::string> error_messages;
public:

    static void write_error(std::string msg)
    {
        if(msg != "")
            error_messages.push_back(msg);
    }

    static std::string get_error()
    {
        if(error_messages.size() > 0)
        {
            std::string err = (*error_messages.begin());
            error_messages.pop_front();
            return err;
        }

        return "";
    }


    class exception : std::exception
    {
        const char* msg;
    public:

        exception(const char* msg) : msg(msg)
        {
        }

        const char* what()
        {
            return msg;
        }
    };
};

#endif // ERROR_H_INCLUDED

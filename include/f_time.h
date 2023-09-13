#ifndef F_TIME_H_INCLUDED
#define F_TIME_H_INCLUDED

#include <chrono>
#include <cstdint>
#include <thread>




class Time
{
    using HRC_time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

    tm     time_holder;
    double FPS;
    double presumed_FPS;
    double current_FPS;
    double min_time;
    double frame_time;
    double start_time = 0;
    double delta_time;
public:
    Time(){ FPS = 240;}
    Time(double FPS)
    :FPS(FPS)
    {}

  
    /** \brief set maximum frame rate of program
        */

    void set_max_FPS(double fps = 1000)
    {
        FPS = (fps <= 0)? 60 : fps;
        min_time = 1000000 / FPS;
    }

    /** \brief returns what frame rate  could have been if frame was not synced to fps
        */

    double get_presumed_FPS()
    {
        return presumed_FPS;
    }

    double get_current_FPS()
    {
        return current_FPS;
    }

    double get_delta_time()
    {
        return delta_time;
    }

    void frame_sync()
    {
        // get time from system
        HRC_time_point _ctime = std::chrono::high_resolution_clock::now();
        frame_time = std::chrono::time_point_cast<std::chrono::microseconds>(_ctime).time_since_epoch().count() - start_time;
        start_time = std::chrono::time_point_cast<std::chrono::microseconds>(_ctime).time_since_epoch().count();
        presumed_FPS = 1000000/frame_time;

        if(min_time > frame_time)
        {
            std::this_thread::sleep_for(std::chrono::duration<std::chrono::microseconds::rep,std::chrono::microseconds::period>(std::chrono::microseconds::rep(min_time - frame_time)));
            delta_time = min_time/1000000;
            current_FPS = FPS;
        }
        else
        {
            current_FPS = presumed_FPS;
            delta_time = frame_time/1000000;
        }

        if(delta_time > 1)
            delta_time = delta_time = min_time/1000000;
    }
};


#endif // F_TIME_H_INCLUDED

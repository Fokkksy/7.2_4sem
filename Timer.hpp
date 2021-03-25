#ifndef Timer_h
#define Timer_h
//#include <iostream>
//#include <iomanip>
//#include <algorithm>
//#include <chrono>
//#include <vector>


class Timer
{
public:
    using clock_t = std::chrono::steady_clock;
    using time_point_t = clock_t::time_point;
    using duration_t = clock_t::duration;
    Timer(const std::string& timer_name = "Untitled") : m_timer_name(timer_name), is_timer_work(true), m_time(duration_t::zero()), m_begin(clock_t::now()) {};
    ~Timer()
    {
        
    }
    void stop()
    {
        if (is_timer_work)
        {
            is_timer_work = false;
            m_time += (clock_t::now() - m_begin);
        }
    }
    void go_on()
    {
        if (!is_timer_work)
        {
            is_timer_work = true;
            m_begin = clock_t::now();
        }
    }
    void reset()
    {
        is_timer_work = true;
        m_time = duration_t::zero();
        m_begin = clock_t::now();

    }
    void print()
    {
        auto timer_worked = is_timer_work;
        stop();
        std::cout << m_timer_name << " : "<< std::chrono::duration_cast<
        std::chrono::milliseconds> (m_time).count() << " milliseconds\n";
        if (timer_worked)
        {
            go_on();
        }
    }

    double math()
    {
        auto timer_worked = is_timer_work;
        stop();
        double value;
        value = std::chrono::duration_cast<std::chrono::milliseconds> (m_time).count();
        if (timer_worked)
        {
          go_on();
        }
        return value;
    }
private:
    const std::string m_timer_name;
    bool is_timer_work;
    duration_t m_time;
    time_point_t m_begin;
};


#endif /* Timer_h */

#ifndef TSQUEUE_H_INCLUDED
#define TSQUEUE_H_INCLUDED

#include <queue>
#include <mutex>
#include <thread>



template<typename TYPE>
class tsqueue // thread safe queue
{
    std::deque<TYPE> message_queue;
    std::mutex mq_lock;
public:

    void push_front(TYPE data)
    {
        std::lock_guard<std::mutex> l(mq_lock);
        message_queue.push_front(data);
    }

    void push_back(TYPE data)
    {
        std::lock_guard<std::mutex> l(mq_lock);
        message_queue.push_back(data);
    }

    TYPE pop_back()
    {
        std::lock_guard<std::mutex> l(mq_lock);
        TYPE hold = message_queue.back();
        message_queue.pop_back();
        return hold;
    }

    TYPE pop_front()
    {
        std::lock_guard<std::mutex> l(mq_lock);
        TYPE hold = message_queue.front();
        message_queue.pop_front();
        return hold;
    }

    void erase()
    {
        std::lock_guard<std::mutex> l(mq_lock);
        message_queue.erase(std::begin(message_queue),std::end(message_queue));
    }

    size_t size()
    {
        std::lock_guard<std::mutex> l(mq_lock);
        return message_queue.size();
    }

    TYPE& front()
    {
        std::lock_guard<std::mutex> l(mq_lock);
        return message_queue.front();
    }

    TYPE& back()
    {
        std::lock_guard<std::mutex> l(mq_lock);
        return message_queue.back();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> l(mq_lock);
        return message_queue.empty();
    }

};


#endif // TSQUEUE_H_INCLUDED


#ifndef QUEUE_THREAD_SAFE
#define QUEUE_THREAD_SAFE

#include <queue>
#include "internal/double lineal container thread safe.hpp"

namespace std
{
    namespace threadsafe
    {

        /* queue threadsafe:

        - Constructors:
            - Default constructor (queue empty)
            - Default constructor with limit_size (queue wont can grow more of limit_size)
            - Copy an existing std:queue

        - Operators:
            - Push one element to the queue, the thread will be blocked if size() == limit_size
            - Push one element to the queue but with one time_limit for block. It will throw Time_Expired exception if time expires
            - bool Try_Pop one element in the queue and put it in the argument. If queue is empty will return false, otherwise return true
            - wait_Pop one element in the queue, if the queue is empty, the thread will block.
            - wait_pop one element in the queue but with one time_limit for block. It will throw Time_Expired exception if time expires

            - bool tryFront put front of queue in the argument. If queue is empty will return false, otherwise return true.
            - bool tryBack its same but with back of queue.
            - waitFront will return one copy of front. If the queue is empty will block, you can select time_expire limit too.

            - size() will return length of queue
            - empty() will return size() == 0
            - setLimit will put one limit_size for queue. With one number <0 the limit will be disabled.
        */

        template <class T>
        class queue : public double_lineal_container<T>
        {
        public:

            queue() {}
            queue(int64_t tam) : double_lineal_container<T>(tam){}
            queue(const std::queue<T> &q) : internal_queue(q){}

        private:
            std::queue<T> internal_queue;

            bool empty_nothreadsafe() const noexcept override
            {
                return internal_queue.empty();
            }

            void pop_nothreadsafe(T& element) override
            {
                element = internal_queue.front();
                internal_queue.pop();
            }

            void push_nothreadsafe(const T& element) override
            {
                internal_queue.push(element);
            }

            void push_nothreadsafe(T && element) override
            {
                internal_queue.push(std::move(element));
            }

            void top_nothreadsafe(T& element) const override
            {
                element = internal_queue.front();
            }

            void back_nothreadsafe(T &element) const override
            {
                element = internal_queue.back();
            }

            size_t size_nothreadsafe() const noexcept override {return internal_queue.size();}

        };
    }
}

#endif // QUEUE_THREAD_SAFE

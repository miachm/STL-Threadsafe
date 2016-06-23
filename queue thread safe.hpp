#ifndef QUEUE_THREAD_SAFE
#define QUEUE_THREAD_SAFE

#include <queue>
#include "double lineal container thread safe.hpp"

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
            queue(int64_t tam) : lineal_container<T>(tam){}
            queue(const std::queue<T> &cola) : Cola(cola){}

        private:
            std::queue<T> Cola;
            std::mutex Cerrojo;
            std::condition_variable Variable;
            std::condition_variable Variable_Push;
            int64_t tam_max = -1;

            bool empty_nothreadsafe() const noexcept override
            {
                return Cola.empty();
            }

            // Idem que la anterior pero con un límite de tiempo, lanza excepción si se cumple el tiempo y no ha sido despertado

            void pop_nothreadsafe(T& element) override
            {
                element = Cola.front();
                Cola.pop();
            }

            void push_nothreadsafe(const T& element) override
            {
                Cola.push(element);
            }

            void push_nothreadsafe(T && elemento) override
            {
                Cola.push(std::move(elemento));
            }

            void top_nothreadsafe(T& element) const override
            {
                element = Cola.front();
            }

            void back_nothreadsafe(T &element) const override
            {
                element = Cola.back();
            }

            size_t size_nothreadsafe() const noexcept override {return Cola.size();}

        };
    }
}

#endif // QUEUE_THREAD_SAFE

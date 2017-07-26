#ifndef QUEUE_THREAD_SAFE
#define QUEUE_THREAD_SAFE

#include <queue>
#include "internal/lineal_container-threadsafe.hpp"

namespace std
{
    namespace threadsafe
    {

        template <class T>
        class priority_queue : public lineal_container<T>
        {
        public:

            priority_queue() {}
            priority_queue(int64_t tam) : lineal_container<T>(tam){}
            priority_queue(const std::priority_queue<T> &q) : queue(q){}

        private:
            std::priority_queue<T> queue;

            bool empty_nothreadsafe() const noexcept override
            {
                return queue.empty();
            }

            void pop_nothreadsafe(T& element) override
            {
                element = queue.top();
                queue.pop();
            }

            void push_nothreadsafe(const T& element) override
            {
                queue.push(element);
            }

            void push_nothreadsafe(T && element) override
            {
                queue.push(std::move(element));
            }

            void top_nothreadsafe(T& element) const override
            {
                element = queue.top();
            }

            size_t size_nothreadsafe() const noexcept override {return queue.size();}

        };
    }
}

#endif // QUEUE_THREAD_SAFE

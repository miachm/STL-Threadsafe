#ifndef QUEUE_THREAD_SAFE
#define QUEUE_THREAD_SAFE

#include <queue>
#include "lineal container thread safe.hpp"

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
            priority_queue(const std::priority_queue<T> &cola) : Cola(cola){}

        private:
            std::priority_queue<T> Cola;
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
                element = Cola.top();
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
                element = Cola.top();
            }

            size_t size_nothreadsafe() const noexcept override {return Cola.size();}

        };
    }
}

#endif // QUEUE_THREAD_SAFE

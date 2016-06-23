#ifndef STACK_THREAD_SAFE
#define STACK_THREAD_SAFE
#include <stack>
#include "lineal container thread safe.hpp"
namespace std
{
    namespace threadsafe
    {
        template<class T>
        class stack : public lineal_container<T>
        {
            std::stack<T> Pila;
            int64_t tam_max = -1;

            void top_nothreadsafe(T &elemento) const override
            {
                elemento = Pila.top();
            }
            void pop_nothreadsafe(T &elemento) override
            {
                elemento = Pila.top();
                Pila.pop();
            }
            void push_nothreadsafe(const T & elemento) override
            {
                Pila.push(elemento);
            }

            void push_nothreadsafe(T && elemento) override
            {
                Pila.push(std::move(elemento));
            }

            bool empty_nothreadsafe() const noexcept override {return Pila.empty();}
            size_t size_nothreadsafe() const noexcept override {return Pila.size();}

        public:
            stack(){}
            stack (int64_t e) : lineal_container<T>(e) {}
            stack(const stack<T> &pila) : Pila(pila) {}
        };
    }
}
#endif // STACK_THREAD_SAFE

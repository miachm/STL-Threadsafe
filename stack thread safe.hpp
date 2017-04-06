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
            std::stack<T> internal_stack;

            void top_nothreadsafe(T &element) const override
            {
                element = internal_stack.top();
            }
            void pop_nothreadsafe(T &element) override
            {
                element = internal_stack.top();
                internal_stack.pop();
            }
            void push_nothreadsafe(const T & element) override
            {
                internal_stack.push(element);
            }

            void push_nothreadsafe(T && element) override
            {
                internal_stack.push(std::move(element));
            }

            bool empty_nothreadsafe() const noexcept override {return internal_stack.empty();}
            size_t size_nothreadsafe() const noexcept override {return internal_stack.size();}

        public:
            stack(){}
            stack (int64_t e) : lineal_container<T>(e) {}
            stack(const stack<T> &s) : internal_stack(s) {}
        };
    }
}
#endif // STACK_THREAD_SAFE

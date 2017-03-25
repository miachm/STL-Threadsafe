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
            std::stack<T> stack;

            void top_nothreadsafe(T &element) const override
            {
                element = stack.top();
            }
            void pop_nothreadsafe(T &element) override
            {
                element = stack.top();
                stack.pop();
            }
            void push_nothreadsafe(const T & element) override
            {
                stack.push(element);
            }

            void push_nothreadsafe(T && element) override
            {
                stack.push(std::move(element));
            }

            bool empty_nothreadsafe() const noexcept override {return stack.empty();}
            size_t size_nothreadsafe() const noexcept override {return stack.size();}

        public:
            stack(){}
            stack (int64_t e) : lineal_container<T>(e) {}
            stack(const stack<T> &s) : stack(s) {}
        };
    }
}
#endif // STACK_THREAD_SAFE

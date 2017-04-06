#ifndef DOUBLE_LINEAL_CONTAINER_THREAD_SAFE
#define DOUBLE_LINEAL_CONTAINER_THREAD_SAFE
#include "lineal container thread safe.hpp"
namespace std
{
    namespace threadsafe
    {
        /*
        This class extends linear_container with back operations.
        Check linear_container's docs for more details
        */
        template<class T>
        class double_lineal_container : public lineal_container<T>
        {
    protected:
            virtual void back_nothreadsafe(T &element) const = 0;
        public:
            double_lineal_container(){}
            double_lineal_container(int64_t e) : lineal_container<T>(e) {}

            bool try_back(T& element) const
            {
                unique_lock<std::mutex> e(lineal_container<T>::lock);

                if (lineal_container<T>::empty_nothreadsafe()) return false;
                else
                {
                    lineal_container<T>::back_nothreadsafe(element);
                    return true;
                }
            }

            void wait_back(T &element)
            {
                unique_lock<std::mutex> e(lineal_container<T>::lock);
                lineal_container<T>::consumer_enterprotocol(e);
                lineal_container<T>::back_nothreadsafe(element);
            }

            template<class Rep,class Period>
            void wait_back(T &element,const std::chrono::duration<Rep, Period>& rel_time)
            {
                unique_lock<std::mutex> e(lineal_container<T>::lock);

                lineal_container<T>::consumer_enterprotocol(e);
                lineal_container<T>::back_nothreadsafe(element);
                //consumer_exitprotocol(e);
            }
        };
    }
}

#endif // DOUBLE_LINEAL_CONTAINER_THREAD_SAFE

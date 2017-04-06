#ifndef LINEAL_CONTAINER_THREAD_SAFE
#define LINEAL_CONTAINER_THREAD_SAFE
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <exception>
#include <thread>

namespace std
{
    namespace threadsafe
    {
        struct Time_Expired : public exception
        {
            std::thread::id Id;

            Time_Expired(std::thread::id id) : Id(id){}

            const char* what() const noexcept override
            {
                return "Time expired in wait() from one std::safethread::container.";
            }
        };
        
        /*
        linear_container is an abstract class that defines a generic container class
        that allows push, pop, size and top operations. This is used for reuse code in differents
        containers (like stack, queue...).
        */
        
        template<class T>
        class lineal_container
        {
            uint64_t tam_max = std::numeric_limits<uint64_t>::max();
            std::condition_variable Consumer_Variable;
            std::condition_variable Producer_Variable;

            protected:
            mutable std::mutex lock;
            virtual bool empty_nothreadsafe() const noexcept = 0;
            virtual void push_nothreadsafe(const T &elemento) = 0;
            virtual void push_nothreadsafe(T && elemento) = 0;
            virtual void pop_nothreadsafe(T & elemento) = 0;
            virtual void top_nothreadsafe(T & elemento) const = 0;
            virtual size_t size_nothreadsafe() const noexcept = 0;

            // Wait for a
            // The first argument is the current mutex
            // The second argument is the condiction variable
            // Third is the condition that has to evaluate, for example, wait for a new item

            void waitSomething(std::unique_lock<std::mutex> &e,std::condition_variable &Condicion,
                               bool (lineal_container<T>::*predicado)() const)
            {
                while (((*this).*predicado)())
                {
                    Condicion.wait(e);
                }
            }

            template<class Rep,class Period>
            void waitSomething(std::unique_lock<std::mutex> &e,std::condition_variable &Condicion,
                               bool (lineal_container<T>::*predicado)()const,
                               const std::chrono::duration<Rep, Period>& rel_time)
            {
                auto time = std::chrono::steady_clock::now();

                while (((*this).*predicado)())
                {
                    if (Condicion.wait_until(e,time+rel_time) == std::cv_status::timeout)
                    {
                        throw Time_Expired(std::this_thread::get_id());
                    }
                }
            }

            bool is_ToUp() const { return size_nothreadsafe() >= tam_max;}

            void producer_enterprotocol(std::unique_lock<std::mutex> &e)
            {
                if (is_ToUp())
                    waitSomething(e,Producer_Variable,&lineal_container<T>::is_ToUp);
            }

            template<class Rep,class Period>
            void producer_enterprotocol(std::unique_lock<std::mutex> &e,const std::chrono::duration<Rep, Period>& rel_time)
            {
                waitSomething(e,Producer_Variable,rel_time,&lineal_container<T>::empty_nothreadsafe);
            }

            void producer_exitprotocol(std::unique_lock<std::mutex> &e)
            {
                e.unlock();
                Consumer_Variable.notify_one();
            }

            void consumer_enterprotocol(std::unique_lock<std::mutex> &e)
            {
                waitSomething(e,Consumer_Variable,&lineal_container<T>::empty_nothreadsafe);
            }

            template<class Rep,class Period>
            void consumer_enterprotocol(std::unique_lock<std::mutex> &e,const std::chrono::duration<Rep, Period>& rel_time)
            {
                waitSomething(e,Consumer_Variable,&lineal_container<T>::empty_nothreadsafe,rel_time);
            }

            void consumer_exitprotocol(std::unique_lock<std::mutex> &e)
            {
                e.unlock();
                Producer_Variable.notify_one();
            }
    public:
            lineal_container() {}
            lineal_container(int64_t e) : tam_max(e) {}
            lineal_container(const lineal_container&e) = delete;

            void push(const T &element)
            {
                unique_lock<std::mutex> e(lock);
                producer_enterprotocol(e);
                push_nothreadsafe(element);
                producer_exitprotocol(e);
            }

            void push(T &&element)
            {
                unique_lock<std::mutex> e(lock);
                producer_enterprotocol(e);
                push_nothreadsafe(std::move(element));
                producer_exitprotocol(e);
            }

            bool try_pop(T &element)
            {
                unique_lock<std::mutex> e(lock);
                if (empty_nothreadsafe())
                {
                    return false;
                }
                else
                {
                    pop_nothreadsafe(element);
                    consumer_exitprotocol(e);
                    return true;
                }
            }

            void wait_pop(T &element)
            {
                unique_lock<std::mutex> e(lock);
                consumer_enterprotocol(e);
                pop_nothreadsafe(element);
                consumer_exitprotocol(e);
            }

            template<class Rep,class Period>
            void wait_pop(T &element,const std::chrono::duration<Rep, Period>& rel_time)
            {
                unique_lock<std::mutex> e(lock);
                consumer_enterprotocol(e,rel_time);
                pop_nothreadsafe(element);
                consumer_exitprotocol(e);
            }

            bool try_top(T &element) const
            {
                unique_lock<std::mutex> e(lock);
                if (empty_nothreadsafe())
                {
                    return false;
                }
                else
                {
                    top_nothreadsafe(element);
                    return true;
                }
            }

            void wait_top(T &element)
            {
                unique_lock<std::mutex> e(lock);

                consumer_enterprotocol(e);
                top_nothreadsafe(element);
                //consumer_exitprotocol(e);
            }

            template<class Rep,class Period>
            void wait_top(T &element,const std::chrono::duration<Rep, Period>& rel_time)
            {
                unique_lock<std::mutex> e(lock);

                consumer_enterprotocol(e);
                top_nothreadsafe(element);
                //consumer_exitprotocol(e);
            }

            void setLimit(uint64_t e)
            {
                tam_max = e;
            }

            size_t size() const
            {
                unique_lock<std::mutex> e(lock);
                return size_nothreadsafe();
            }

            bool empty() const
            {
                unique_lock<std::mutex> e(lock);
                return empty_nothreadsafe();
            }
        };
    }
}

#endif

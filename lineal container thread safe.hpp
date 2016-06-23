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
        template<class T>
        class lineal_container
        {
            int64_t tam_max = -1;
            std::condition_variable Consumer_Variable;
            std::condition_variable Producer_Variable;

            protected:
            mutable std::mutex Cerrojo;
            virtual bool empty_nothreadsafe() const noexcept = 0;
            virtual void push_nothreadsafe(const T &elemento) = 0;
            virtual void push_nothreadsafe(T && elemento) = 0;
            virtual void pop_nothreadsafe(T & elemento) = 0;
            virtual void top_nothreadsafe(T & elemento) const = 0;
            virtual size_t size_nothreadsafe() const noexcept = 0;

            // Espero a que se cumpla un determinado predicado (por ejemplo, hasta que el contenedor esté vacío
            // El primer argumento es el mutex actual
            // El segundo argumento es la condiction variable que le tiene que despertar
            // El tercero es el predicado que tiene que evaluar, por ejemplo la función  bool empty_nothreadsafe()


            void waitSomething(std::unique_lock<std::mutex> &e,std::condition_variable &Condicion,
                               bool (lineal_container<T>::*predicado)() const)
            {
                while (((*this).*predicado)())
                {
                    Condicion.wait(e);
                }
            }

            // Idem que la anterior pero con un límite de tiempo, lanza excepción si se cumple el tiempo y no ha sido despertado

            template<class Rep,class Period>
            void waitSomething(std::unique_lock<std::mutex> &e,std::condition_variable &Condicion,
                               bool (lineal_container<T>::*predicado)()const,
                               const std::chrono::duration<Rep, Period>& rel_time)
            {
                auto tiempo = std::chrono::steady_clock::now();

                while (((*this).*predicado)())
                {
                    if (Condicion.wait_until(e,tiempo+rel_time) == std::cv_status::timeout)
                    {
                        throw Time_Expired(std::this_thread::get_id());
                    }
                }
            }

            bool limite_activado() const { return tam_max >= 0;}
            bool is_ToUp() const { return size_nothreadsafe() >= tam_max;}

            void producer_enterprotocol(std::unique_lock<std::mutex> &e)
            {
                if (limite_activado() && is_ToUp())
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

            // No es necesario ejecutar si no se ha consumido nada
            void consumer_exitprotocol(std::unique_lock<std::mutex> &e)
            {
                e.unlock();
                Producer_Variable.notify_one();
            }
    public:
            lineal_container() {}
            lineal_container(int64_t e) : tam_max(e) {}
            lineal_container(const lineal_container&e) = delete;

            void push(const T &elemento)
            {
                unique_lock<std::mutex> e(Cerrojo);
                producer_enterprotocol(e);
                push_nothreadsafe(elemento);
                producer_exitprotocol(e);
            }

            void push(T &&elemento)
            {
                unique_lock<std::mutex> e(Cerrojo);
                producer_enterprotocol(e);
                push_nothreadsafe(std::move(elemento));
                producer_exitprotocol(e);
            }

            bool try_pop(T &elemento)
            {
                unique_lock<std::mutex> e(Cerrojo);
                if (empty_nothreadsafe())
                {
                    return false;
                }
                else
                {
                    pop_nothreadsafe(elemento);
                    consumer_exitprotocol(e);
                    return true;
                }
            }

            void wait_pop(T &elemento)
            {
                unique_lock<std::mutex> e(Cerrojo);
                consumer_enterprotocol(e);
                pop_nothreadsafe(elemento);
                consumer_exitprotocol(e);
            }

            template<class Rep,class Period>
            void wait_pop(T &elemento,const std::chrono::duration<Rep, Period>& rel_time)
            {
                unique_lock<std::mutex> e(Cerrojo);
                consumer_enterprotocol(e,rel_time);
                pop_nothreadsafe(elemento);
                consumer_exitprotocol(e);
            }

            bool try_top(T &elemento) const
            {
                unique_lock<std::mutex> e(Cerrojo);
                if (empty_nothreadsafe())
                {
                    return false;
                }
                else
                {
                    top_nothreadsafe(elemento);
                    return true;
                }
            }

            void wait_top(T &elemento)
            {
                unique_lock<std::mutex> e(Cerrojo);

                consumer_enterprotocol(e);
                top_nothreadsafe(elemento);
                //consumer_exitprotocol(e); // No lo ejecutamos p
            }

            template<class Rep,class Period>
            void wait_top(T &elemento,const std::chrono::duration<Rep, Period>& rel_time)
            {
                unique_lock<std::mutex> e(Cerrojo);

                consumer_enterprotocol(e);
                top_nothreadsafe(elemento);
                //consumer_exitprotocol(e);
            }

            void setLimit(int64_t e)
            {
                tam_max = e;
            }

            size_t size() const
            {
                unique_lock<std::mutex> e(Cerrojo);
                return size_nothreadsafe();
            }

            bool empty() const
            {
                unique_lock<std::mutex> e(Cerrojo);
                return empty_nothreadsafe();
            }
        };
    }
}

#endif

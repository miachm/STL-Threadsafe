#ifndef MAP_THREAD_SAFE
#define MAP_THREAD_SAFE

#include <map>
#include <thread>
#include <mutex>
#include <shared_mutex>

namespace std
{
    namespace threadsafe
    {
        /*
        Concurrent Map prototype, it allows concurrent reads and safe writes.

        */
        template<class K,class V>
        class map
        {
            std::map<K,V> internal_map;

            std::shared_mutex mutex;

            public:
                map(){}
                map(const std::map<K,V> &m) : internal_map(m){}
                map(const std::threadsafe::map &m) = delete;

                bool get(const K& key,V& output)
                {
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    auto it = internal_map.find(key);
                    if (it == internal_map.end())
                    {
                        return false;
                    }
                    else{
                        output = it->second;
                    	return true;
		    }
                }
            
                bool insert(const K & key,V & value){
                    std::unique_lock<std::shared_mutex> lock(mutex);
                    return internal_map.insert(std::make_pair(key,value)).second;
                }
            
                size_t erase(const K & key){
                    std::unique_lock<std::shared_mutex> lock(mutex);
                    return internal_map.erase(key);
                }

                size_t size(){
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    return internal_map.size();
                }
                
                bool contains(const K& key){
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    auto it = internal_map.find(key);
                    return it == internal_map.end();
                }
                
                V putIfAbsent(const K& key,const V& value){
			std::unique_lock<std::shared_mutex> lock(mutex);
			auto it = internal_map.find(key);
			if (it == internal_map.end()){
				internal_map.insert(std::make_pair(key,value));
				return value;
			}
			else
				return it->second;
		}

		bool remove(const K& key,const V& value){
			std::unique_lock<std::shared_mutex> lock(mutex);
			auto it = internal_map.find(key);
			if (it == internal_map.end() || it->second != value)
				return false
			else
			{
				internal_map.erase(it);
				return true;
			}
		}

		bool replace(const K& key,const V& value){
			std::unique_lock<std::shared_mutex> lock(mutex);
			
			auto it = internal_map.find(key);
			if (it != internal_map.end()){
				it->second = value;
				return true;
			}
			return false;
		}

		bool replace(const K& key,V& oldValue,V& newValue){
			std::unique_lock<std::shared_mutex> lock(mutex);
			
			auto it = internal_map.find(key);
			if (it != internal_map.end() && it->second == oldValue){
				it->second = newValue;
				return true;
			}
			return false;
		}
        };
    }
}

#endif

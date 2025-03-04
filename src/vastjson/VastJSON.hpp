#ifndef VAST_JSON_HPP
#define VAST_JSON_HPP

#include <sstream>
#include <fstream>
#include <vector>
#include <optional>
//
#include <nlohmann/json.hpp>
//using json = nlohmann::json;

namespace vastjson
{

    class VastJSON
    {
    public:
        // multiple json
        std::map<std::string, nlohmann::json> jsons;
        // read string cache
        std::map<std::string, std::string> cache;

        unsigned size()
        {
            return this->cache.size();
        }

        nlohmann::json &operator[](std::string key)
        {
            return this->getKey(key);
        }

        // get key in json structured format
        nlohmann::json &getKey(std::string key)
        {
            auto it = jsons.find(key);
            if (it != jsons.end())
            {
                return jsons[key];
            }
            auto it2 = cache.find(key);
            if (it2 == cache.end())
            {
                //std::cerr << "BigJSON::getKey() error: key '" << key << "' does not exist!" << std::endl;
            }
            if (cache[key] == "")
            {
                //std::cerr << "BigJSON::getKey() error: key '" << key << "' is empty or has been unloaded!" << std::endl;
            }

            // TODO: continue even with error (or return 'optional' for recovery?)
            jsons[key] = nlohmann::json::parse(std::move(cache[key]));
            cache[key] = "";
            return jsons[key];
        }

        // unload json structure and do not keep string cache
        void unload(std::string key)
        {
            auto it = jsons.find(key);
            if (it == jsons.end())
            {
                //std::cerr << "BigJSON::unload() error: json key '" << key << "' does not exist!" << std::endl;
            }
            jsons.erase(it); // drop json structure
            cache[key] = ""; // mark as empty
        }

        // move json structure back to string cache (since json structured format may be more memory costly)
        void toCache(std::string key)
        {
            auto it = jsons.find(key);
            if (it == jsons.end())
            {
                //std::cerr << "BigJSON::toCache() error: json key '" << key << "' does not exist!" << std::endl;
            }
            //
            std::stringstream ssjson;
            ssjson << jsons[key];
            unload(key);               // unload json structure
            cache[key] = ssjson.str(); // keep string in cache
        }

        VastJSON(std::string& str)
        {
            std::istringstream is(str);
            process(is);
        }

        VastJSON(std::istream &is)
        {
            process(is);
        }

        void process(std::istream &is)
        {
            std::string before;
            std::string content;
            int count_par = 0;
            int target_field = 1;   // starts from 1
            bool save = false;      // true if nested curly brackets
            //
            while (true)
            {
                char c;
                if (!is.get(c))
                    break; // EOF
                if (!save)
                    before += c;
                if (save)
                    content += c;
                if (c == '{')
                {
                    count_par++;
                    if ((count_par == target_field + 1) && !save) // 2?
                    {
                        content += c;
                        save = true;
                    }
                }
                if (c == '}')
                {
                    if ((count_par == target_field + 1) && save) // 2?
                    {
                        //
                        //std::cout << "RESTART = " << sbefore << std::endl;
                        //
                        // 1-get field name
						unsigned keyStart = before.find('\"') + 1;
						unsigned keySize = before.find('\"', keyStart+1) - keyStart;
						std::string field_name = before.substr(keyStart, keySize);
                        before = "";
                        //2-move string to cache
                        cache[field_name] = std::move(content); // <------ IT'S FUNDAMENTAL TO std::move() HERE!
                        //
                        //std::cout << "store = '" << field_name << "'" << std::endl;
                        //
                        //before = std::stringstream();
                        //content = std::stringstream();
                        content = "";
                        //
                        save = false;
                    }
                    count_par--;
                }
            }
        }

        ~VastJSON()
        {
        }
    };

} // namespace vastjson

#endif // VAST_JSON_HPP

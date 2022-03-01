//序列化,反序列化功能
#pragma once


#ifndef __SERIALIZABLE_H__
#define __SERIALIZABLE_H__
#endif

#include "reflectable.hpp"

namespace cppjson {

class Serializable  : public Reflectable {
public:
    
    /**
     * @brief 序列化对象
     */
    template<typename Object>
    inline static std::string dumps(const Object&object){
        if constexpr (has_config_member_function<Object>::value)
            return object.get_config().serialized_to_string();
        else
            return configPair::value_to_string(object);
    }
};
    
} // end namespace cppjson


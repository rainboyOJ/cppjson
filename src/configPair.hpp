#pragma once
#include <string>
#include <typeinfo>
#include <sstream>
#include <cstring>
#include <vector>
#include <cxxabi.h>
#include <unordered_map>
#include <functional>

#include "utils.hpp"
#include "exception.hpp"

#define GET_TYPE_NAME(type) abi::__cxa_demangle(typeid(type).name(),0,0,0)

namespace cppjson {

struct configPair {

public: //函数

    //构造函数
    template<typename T>
    configPair(const std::string&name,const T&object):
        key(name),
        type(GET_TYPE_NAME(T)),
        address(reinterpret_cast<std::size_t>(&object)),
        value(value_to_string(object))
    {}

    //把对应类型的值转成字符串
    template<typename Object>
    static std::string value_to_string(const Object& field);

public: //数据
    std::string key;    //存成员变量的名字
    std::string value;  //存成员变量转成字符串的结果
    std::string type; //字符串表示的成员变量的类型
    std::size_t address; //成员变量的偏移地址
};



template<typename Object>
std::string configPair::value_to_string(const Object&field){
    std::ostringstream oss;
    if constexpr ( std::is_fundamental_v<Object> && (! std::is_same_v<Object, char *>) )
    {
        oss << field;
        return oss.str();
    }
    return "";
}






} // end namespace cppjson

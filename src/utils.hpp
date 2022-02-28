#pragma once

namespace cppjson {
    
struct EmptyClass{}; //在Reflectanle::get_method与Reflectable::classmethod_wrapper中有使用，用于类型转换的"中介"


// ========================== has_config_member_function
template<typename T,typename Enable = void >
struct has_config_member_function_s : public std::false_type {};

template<typename T>
struct has_config_member_function_s< T, decltype(std::declval<T>().get_config()) > : public std::true_type{};

template<typename T>
constexpr bool has_config_member_function(){
    return has_config_member_function_s<T>::value;
}
// ========================== has_config_member_function




} // end namespace cppjson


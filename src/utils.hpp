#pragma once

#include <string>
#include <typeinfo>
#include <sstream>
#include <cstring>
#include <vector>
#include <cxxabi.h>
#include <unordered_map>
#include <functional>

namespace cppjson {

#define GET_TYPE_NAME(type) abi::__cxa_demangle(typeid(type).name(),0,0,0)
    
struct EmptyClass{}; //在Reflectanle::get_method与Reflectable::classmethod_wrapper中有使用，用于类型转换的"中介"

template<typename T,typename... U>
inline bool Any_Of(T&& t,U... args){
    return  ( (t==args)||...);
}

template<typename T,typename... U>
inline bool None_of(T&& t,U... args){
    return !Any_Of(std::forward<T>(t),args...);
}

inline bool isBlankChar(char c){
    return Any_Of(c,'\t','\n','\r',' ');
}




template<typename T>
struct has_config_member_function {
    template<typename U>
    static auto check(int) -> decltype(std::declval<U>().get_config(),std::true_type());

    //sink hole function
    template<typename U>
    static std::false_type check(...);

    static constexpr bool value = decltype(check<T>(0))::value;
};

//使用偏特化
template <typename> struct is_tuple: std::false_type {};
template <typename ...T> struct is_tuple<std::tuple<T...>>: std::true_type {};

template <typename> struct is_pair: std::false_type {};
template <typename T,typename U> struct is_pair<std::pair<T,U>> : std::true_type {};

template <typename> struct is_vector: std::false_type {};
template <typename T> struct is_vector<std::vector<T>>: std::true_type {};

/// ====================== 函数

//对于每一个元素进行遍历
template<typename Object,int index>
inline auto for_each_element(Object&object,auto&&callback) 
{
    callback(std::get<index>(object),index);
    if constexpr(index+1 < std::tuple_size<Object>::value)
        for_each_element<Object,index+1>(object,callback);
}

template<typename T,typename F,std::size_t... idx>
void __for_each(T&& t,F&& f,std::index_sequence<idx...> ){
    (f(std::get<idx>(t),idx == sizeof...(idx)-1 ),...); //fold expression

}

template<typename... Ts,typename F>
void for_each_tuple(std::tuple<Ts...> const & t,F&& f){
    __for_each(t,std::forward<F>(f),std::make_index_sequence<sizeof... (Ts)>{});
}
/// ====================== 函数 end

// value_to_string 利用了偏特化的技巧
template<typename T,typename = void>
struct To_String {
    static std::string to(const T & object) {
        return "unkown";
    }
};

template<typename T>
struct To_String <T,
    std::enable_if_t< std::is_fundamental_v<T> && (! std::is_same_v<T, char *>) >
    >
{
    static std::string to(const T & object) {
        return std::to_string(object);
    }
};

template<typename T>
struct To_String <T,
    std::enable_if_t< std::is_same_v<T, std::string>>
> {
    static std::string to(const T & object) {
        return std::string("\"") + object + std::string("\"");
    }
};

template<typename T>
struct To_String <T,
    std::enable_if_t< std::is_same_v<T, char *>>
> {
    static std::string to(const T & object) {
        return std::string("\"") + std::string(object) + std::string("\"");
    }
};

//可序列化 get_config
template<typename T>
struct To_String <T,
    std::enable_if_t< has_config_member_function<T>::value>
> {
    static std::string to(const T & object) {
        return object.get_config().serialized_to_string();
    }
};

template<typename T>
struct To_String <T,
    std::enable_if_t< is_tuple<T>::value >
> {
    static std::string to(const T & object) {
        std::ostringstream oss;
        oss << "[";
        for_each_tuple(object, [&oss](auto x,bool last){
                //std::cout << "--- " ;
                //std::cout << GET_TYPE_NAME(x) << std::endl;
                //auto ret = typeid(decltype(x)) == typeid(int);
                //std::cout << ret << std::endl;
                    oss << To_String<std::remove_cv_t<decltype(x)>>::to(x);
                    if( !last ) oss << ",";
                });
        oss << "]";
        return oss.str();
    }
};

template<typename T>
struct To_String <T,
    std::enable_if_t< is_pair<T>::value >
> {
    static std::string to(const T & object) {
        std::ostringstream oss;
        oss << "[";
        oss << To_String<std::tuple_element_t<0,T>>::to(object.first);
        oss << ",";
        oss << To_String<std::tuple_element_t<1,T>>::to(object.second);
        oss << "]";
        return oss.str();
    }
};

template<typename T>
struct To_String <T,
    std::enable_if_t< is_vector<T>::value >
> {
    template<typename U>
    static std::string to(const std::vector<U> & object) {
        std::ostringstream oss;
        oss << "[";
        std::size_t idx = 0;
        for (const auto& e : object) {
            oss << To_String<U>::to(e);
            if( ++idx != object.size() )
                oss << ",";
        }
        oss << "]";
        return oss.str();
    }
};




} // end namespace cppjson


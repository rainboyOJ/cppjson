#pragma once

namespace cppjson {
    
struct EmptyClass{}; //在Reflectanle::get_method与Reflectable::classmethod_wrapper中有使用，用于类型转换的"中介"


template<typename T>
struct has_config_member_function {
    template<typename U>
    static auto check(int) -> decltype(std::declval<U>().get_config(),std::true_type());

    //sink hole function
    template<typename U>
    static std::false_type check(...);

    static constexpr bool value = decltype(check<T>(0))::value;
};

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





} // end namespace cppjson


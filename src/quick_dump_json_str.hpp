#pragma once
#include <iostream>
#include <string>
#include <ranges>
#include <type_traits>


template<typename T>
struct is_char_array {
    template<std::size_t N>
    static auto check(char(&) [N]) -> std::true_type;

    template<std::size_t N>
    static auto check(const char(&) [N]) -> std::true_type;
    
    template<std::size_t N>
    static auto check(char(&&) [N]) -> std::true_type;

    template<std::size_t N>
    static auto check(const char(&&) [N]) -> std::true_type;

    static std::false_type check(...);

    static constexpr bool value = decltype(check(std::declval<T>()))::value;
};

//template<typename T>
//struct CHAR_ARRAY_SIZE {
    //template<typename U1,std::size_t N>
    //static constexpr std::size_t __array_size(U1 (&) [N]) { return N; }

    //static constexpr T type{}; // std::declval 不能在 constexpr 中使用,只能在 decltype,
                               //// declval can only be called in unevaluated contexts such as in a decltype or sizeof.
                               //// --> by https://stackoverflow.com/a/66389795
    //static constexpr int value =  __array_size(type);
//};

template<typename T>
struct CHAR_ARRAY_SIZE {
    static constexpr std::size_t value = 0;
};

template<typename T,std::size_t N>
requires std::same_as<std::remove_cvref_t<T>, char>
struct CHAR_ARRAY_SIZE<T [N]> {
    static constexpr std::size_t value = N;
};



//c++ 20 才能使用
//作用: 通过传递的参数来快速创建json字符串
//使用方式
// ("key1",value1,"key2",value2)
// value 支持的类型 : int,long long ,float,double,std::string,std::string_view,char *
// value 支持的类型支持还支持
// 包含元素为上的可能迭代类型,可以 for(const &e : value1)






//能支持的单个类型的元素
template<typename T,typename  U = std::remove_cvref_t<T>>
concept qdj_stringAble = 
    std::same_as<U, std::string> ||
    std::same_as<U, std::string_view> || 
    is_char_array<U>::value;

template<typename T,typename  U = std::remove_cvref_t<T>>
concept qdj_singleValueType = ( std::integral<U> && !std::same_as<char, U>);

//能支持的可迭代的类型的元素
template<typename T>
concept qdj_iterable  = 
    std::ranges::range<T> && (
    qdj_stringAble< std::ranges::range_value_t<T> >|| 
    qdj_singleValueType< std::ranges::range_value_t<T> >
     );


//工具函数,转成字符串
//1. 字符中转字符串
template<typename T>
requires qdj_stringAble<T>
std::string qdj_to_string( T && v) {
    std::string ret{'"'};
    if constexpr (is_char_array<T>::value) {
        for(auto i = 0 ;i < CHAR_ARRAY_SIZE<T>::value; ++i){
            if( v[i] == '"') ret += "\\";
            ret += v[i];
        }
    }
    else
        for (auto& e : v) {
            if( e == '"') ret += "\\";
            ret += e;
        }
    ret += '"';
    return ret;
}

//2. 数字转字符串
template<typename T>
requires qdj_singleValueType<T>
std::string qdj_to_string( T && v) {
    std::string ret = std::to_string(v);
    return ret;
}

//3. 可迭代转字符串
template<typename T>
requires qdj_iterable<T>
std::string qdj_to_string( T && v) {
    std::string ret{'['};
    for (auto& e : v) {
        ret += qdj_to_string(e);
        ret += ',';
    }
    if( ret.back() == ',')
        ret.back() = ']';
    else ret += ']';
    return ret;
}

template<typename T>
concept quick_dump_json_able = requires(T&& v) {
    {qdj_to_string(v)};
};


//https://www.modernescpp.com/index.php/c-20-concepts-the-details
//You can use concepts in variadic templates;
//LoL,还可以这样用 😮
template<quick_dump_json_able... Args>
requires (sizeof...(Args) %2 == 0) //保证参数数量是偶数
std::string quick_dump_json(Args&&... args) {
    //static_assert(sizeof... (Args) % 2 == 0, "quick_dump_json Args size must was even!");
    std::string ret{"{"};

    auto f = [&ret](auto && f,auto&& arg1,auto && arg2,auto&&... other) {
        ret += '"';
        ret += std::string(arg1);
        ret += std::string("\":");

        using type_arg2 = std::remove_cvref_t<decltype(arg2)>;

        ret += qdj_to_string(arg2);

        if constexpr (sizeof... (other) == 0)
            return ;
        ret += ',';
        if constexpr (sizeof... (other) > 0) //这个要保留
            f(f,other...);
    };

    //f(f,std::forward<Args>(args)... ); //递归的调用
    f(f,args...);


    ret += '}';
    return ret;
}

#ifdef  __NERVER_DEFINE____JUST_TEST__
int main(int argc,char * argv[]){
    std::vector<int> v{1,2,3,4};
    std::cout << 
        quick_dump_json("helo","workd","value",100,"arr",v)
        << std::endl;


    return 0;
}
#endif

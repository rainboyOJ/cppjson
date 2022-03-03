//序列化,反序列化功能
#pragma once


#ifndef __SERIALIZABLE_H__
#define __SERIALIZABLE_H__
#endif

#include "reflectable.hpp"

namespace cppjson {

class Serializable  : public Reflectable {
public:

    template<typename T> //注册
    static void Regist(){
        configPair::string_to_value[GET_TYPE_NAME(T)] = [](void * object,const std::string & value)->void
        {
            config __conf = Serializable::parse(value);
            //Serializable::from_config
        };
        Reflectable::Regist<T>();
    }
    
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

    /**
     * @brief 反序列化/加载
     */
    template<typename T>
    static auto loads(const std::string& json){
        std::string class_name=GET_TYPE_NAME(T);
        
    }

    /**
     * @brief 解析一个json 转成config对象,是一个状态机,是整个cppjson的核心
     */
    static config parse(std::string_view json){

        //状态 
        enum State {
            init,
            parse_value,
            parse_struct,
            parse_fundamental,
            parse_iterable,
            parse_string,
            end_parse
        } state = init;
        config __conf; //

        while ( !json.empty() && Any_Of(json.front(), '\t','\r','\n',' ') ) { json.remove_prefix(1); }
        while ( !json.empty() && Any_Of(json.back(),  '\t','\r','\n',' ') ) { json.remove_suffix(1); }

        if( json.front() != '{') throw JsonDecodeDelimiterException('{');
        if( json.back()  != '}') throw JsonDecodeDelimiterException('}');
        std::string key,value;

        int nested_struct_layer=0,nested_iterable_layer=0;

        for(std::size_t i = 0 ;i < json.size(); ++i){
            auto& it = json[i];
            if(state == init){
                if( it == ':') 
                    state = parse_value;
                else if( !isBlankChar(it) && None_of(it, ',','{') )
                    key.push_back(it);
            }
            else if( state == parse_value){
                if( isBlankChar(it) ) continue;
                if( it == '{' ){
                    value.push_back(it);
                    nested_struct_layer++;
                    state = parse_struct;
                }
                else if( it == '[' ){
                    value.push_back(it);
                    nested_iterable_layer++;
                    state = parse_iterable;
                }
                else if( it == '\"' ){
                    value.push_back(it);
                    state = parse_string;
                }
                else if( !isBlankChar(it) ){
                    value.push_back(it);
                    state = parse_fundamental;
                }
            }
            else if( state == parse_string){
                value.push_back(it);
                if( it == '\"' && i > 0 && json[i-1] != '\\' ){  // 结尾
                    state = end_parse;
                    --i;
                }
            }
            else if( state == parse_fundamental ){
                if( Any_Of(it, ',','}', '\"') || isBlankChar(it) ) { //结束
                    if( it == '\"')
                        value.push_back(it);
                    state = end_parse;
                    --i;
                    continue;
                }
                value.push_back(it);
            }
            else if( state == parse_iterable ){
                if( Any_Of(it, '[',']') ){
                    nested_iterable_layer += (it == ']' ? -1 : 1);
                    value.push_back(it);
                    if( nested_iterable_layer == 0){
                        state = end_parse;
                        --i;
                    }
                    continue;
                }
                value.push_back(it);
            }
            else if( state == parse_struct){
                if(Any_Of(it, '{','}') ){
                    nested_struct_layer += ( it == '}' ? -1 : 1);
                    value.push_back(it);
                    if(nested_struct_layer == 0){
                        state = end_parse;
                        --i;
                    }
                    continue;
                }
                value.push_back(it);
            }
            else if ( state == end_parse){
                std::cout << key << " " << value << std::endl;
                state = init;
                __conf[key] = value;
                key.clear();
                value.clear();
            }
        }

        return __conf;
    }
};
    
} // end namespace cppjson


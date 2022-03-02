#include<iostream>
#include<vector>
#include <tuple>

#include "serializable.hpp"

using namespace cppjson;

using namespace std;
struct Node
{
    int x=1;
    float y=5;
    std::string z="sjf";
    char *abc;
    //std::tuple<int,float,std::string> tp = std::make_tuple(1,1.1,"str");
    std::tuple<int,float,std::string> tp = std::make_tuple(1,1.1,"str");
    config get_config() const
    {
        config _conf=Reflectable::get_config(this);
        _conf.update({
            {"x",x},
            {"y",y},
            {"z",z},
            {"abc",abc},
            {"tp",tp},
        });
        return _conf;
    }
};

void func(){
}

int main()
{
    //Serializable::Regist<Node>();
    Node object;
    object.abc = "abc";

    /*序列化与反序列化*/
    
    std::string json=Serializable::dumps(object);                 //序列化
    cout<<json<<endl;
    //Node b=Serializable::loads<Node>(json);                               //反序列化
    //[>正常访问<]
    //cout<<b.x<<endl;                                                      //正常访问成员变量
    //cout<<b.y<<endl;
    //cout<<b.z<<endl;
    //[>成员函数反射<]
    //cout<<Reflectable::get_method<int>(b,"add",5,6)<<endl;                //通过字符串名称访问成员函数
    //cout<<Reflectable::get_method<string>(b,"getName")<<endl;
}

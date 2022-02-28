#pragma once

namespace cppjson {
    
struct NoSuchClassException:public std::exception
{ 
public:
    explicit NoSuchClassException(const std::string&type_name);
    virtual ~NoSuchClassException()throw();
    virtual const char*what()const throw();
protected: 
    std::string message;
};

} // end namespace cppjson


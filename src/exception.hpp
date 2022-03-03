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



class JsonDecodeException:public std::exception
{
public:
    explicit JsonDecodeException():message("JsonDecodeException: ") {};
    virtual ~JsonDecodeException() throw() {};
    virtual const char*what()const throw()=0;
protected:
    std::string message;
};

class JsonDecodeDelimiterException:public JsonDecodeException
{
public:
    explicit JsonDecodeDelimiterException(const char&ch) : JsonDecodeException()
    {
        std::ostringstream oss;
        oss<<"Expecting '"<<ch<<"' delimiter in decoding json data.";
        message += oss.str();
    }

    virtual ~JsonDecodeDelimiterException(){};
    virtual const char*what()const throw() { return this->message.c_str(); }
};

} // end namespace cppjson


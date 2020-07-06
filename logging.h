#ifndef __LOGGING_H
#define __LOGGING_H
#include <string>

class Logger {
public:
    Logger();
    void warn(std::string msg);
    void error(std::string msg);
};

#endif
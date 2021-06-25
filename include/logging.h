#ifndef __LOGGING_H
#define __LOGGING_H
#include <string>

class Logger {
public:
    Logger();
    ~Logger();
    void warn(std::string msg);
    void error(std::string msg);
    void info(std::string msg);
    void debug(std::string msg);
    void tps(double ms, double tps);
    void num_surfaces(uint32_t num);
    void draw();
    uint32_t num;
    double ms, tpss;
    uint32_t mx, my;
};

#endif

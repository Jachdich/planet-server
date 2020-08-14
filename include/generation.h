#ifndef __GENERATION_H  
#define __GENERATION_H
#include <jsoncpp/json/json.h>
extern Json::Value genConf;

void loadConfig();

int rndInt(int min, int max);

double rndDouble(double fMin, double fMax);

class Pixel {
public:
    int r = 0;
    int g = 0;
    int b = 0;
    Pixel();
    Pixel(int r, int g, int b);
    void rand();
    void rand(int min, int max);
    int asInt();
};
#endif
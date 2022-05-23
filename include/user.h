#ifndef __USER_H
#define __USER_H
#include <string>
#include <jsoncpp/json/json.h>
#include "generation.h"
struct UserMetadata {
    UserMetadata(std::string uname, std::string pword);
    inline UserMetadata() {}
    std::string username;
    std::string hashed_password;
    std::string password_salt;
    uint64_t uuid;
    Pixel colour;
    void toJson(Json::Value &val) const;
    void toNetworkSafeJson(Json::Value &val) const;
    void fromJson(Json::Value &val);
    bool isPasswordCorrect(std::string pword);
};
#endif

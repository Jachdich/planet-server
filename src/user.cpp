#include "user.h"

std::string hash(std::string salted) {
    //VERY VERY BAD IDEA LOL
    return salted;
}

UserMetadata::UserMetadata(std::string uname, std::string pword) {
    //Generate hash and salt. NOTE THIS IS A TEST DONT USE PLEASE
    password_salt = "TEST SALT NOT FOR PRODUCTION";
    hashed_password = hash(pword + password_salt);
    username = uname;
    uuid = (((uint64_t)rand()) << 32) | rand(); //note NOT cryptographically secure and MUST BE FIXED
    colour.rand();
}

bool UserMetadata::isPasswordCorrect(std::string pword) {
    std::string hashed_test_password = hash(pword + password_salt);
    return (hashed_test_password == hashed_password);
} 
void UserMetadata::toJson(Json::Value &root) {
    root["name"] = username;
    root["hashed_password"] = hashed_password;
    root["password_salt"] = password_salt;
    root["uuid"] = (Json::UInt64)uuid;
    root["colour"] = colour.asInt();
}

void UserMetadata::toNetworkSafeJson(Json::Value &root) {
    root["name"] = username;
    root["uuid"] = (Json::UInt64)uuid;
    root["colour"] = colour.asInt();
}

void UserMetadata::fromJson(Json::Value &root) {
     username = root["name"].asString();
     hashed_password = root["hashed_password"].asString();
     password_salt = root["password_salt"].asString();
     uuid = root["uuid"].asUInt64();
     colour = Pixel(root["colour"].asUInt());
}

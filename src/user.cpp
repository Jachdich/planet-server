#include "../include/user.h"
#include "../include/server.h"
#include <argon2.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#define SALTLEN 16
#define HASHLEN 32

int argon2_hash(uint8_t *hash, uint8_t *pword, uint8_t *salt) {
    uint8_t *pwd = (uint8_t *)strdup((char *)pword);
    uint32_t pwdlen = strlen((char *)pwd);

    uint32_t t_cost = 2; // 1-pass computation
    uint32_t mem_cost = (1<<16);
    uint32_t num_threads = 1;

    int err = argon2i_hash_raw(t_cost, mem_cost, num_threads, pwd, pwdlen, salt, SALTLEN, hash, HASHLEN);

    free(pwd);
    return err;
}

std::string get_hex(uint8_t *array, size_t len) {
    char hexstr[len * 2 + 1];
    for (size_t i = 0; i < len; i++) {
        sprintf(hexstr + i * 2, "%02x", array[i]);
    }
    hexstr[len * 2] = 0;
    return std::string(hexstr);
}

UserMetadata::UserMetadata(std::string uname, std::string pword) {

    uint8_t hash[HASHLEN];
    uint8_t salt[SALTLEN];
    memset(salt, 0x00, SALTLEN);
    int err = argon2_hash(hash, (uint8_t *)pword.c_str(), salt);
    if (err) {
        printf("This shouldn't happen (password hash failed): %d\n", err);
    }
    password_salt   = get_hex(salt, SALTLEN);
    hashed_password = get_hex(hash, HASHLEN);
    username = uname;
    uuid = (((uint64_t)rand()) << 32) | rand(); //note NOT cryptographically secure and MUST BE FIXED
    colour.rand();
    logger.debug("Hashed password is: " + hashed_password + ", and salt is " + password_salt);
}

bool UserMetadata::isPasswordCorrect(std::string pword) {
    uint8_t hash[HASHLEN];
    uint8_t salt[SALTLEN];
    memcpy(salt, password_salt.c_str(), SALTLEN);
    int err = argon2_hash(hash, (uint8_t *)pword.c_str(), salt);
    if (err) {
        printf("This shouldn't happen (password hash failed): %d\n", err);
    }
    std::string hashed_test_password = get_hex(hash, HASHLEN);
    logger.debug("Hashed password is: " + hashed_password + ", and other one is " + hashed_test_password);
    return true; //DEBUG
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

#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include "utils.h"

std::string readFile(const char* filename) {
    std::stringstream ss;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(filename);
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

void sleepms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
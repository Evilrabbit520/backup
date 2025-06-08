// Config.h
#ifndef PARAMETERMANAGEMENG_H
#define PARAMETERMANAGEMENG_H

#include <string>
#include <unordered_map>

class Parameter {
public:
    /**
     * @brief Simple parameter parser
     * 
     * @param argc 
     * @param argv 
     * @return std::unordered_map<std::string, std::string> 
     */
    static std::unordered_map<std::string, std::string> parseArgs(int argc, char *argv[]);

private:
    /**
     * @brief Displays help information
     * 
     */
    static void displayHelp();
};

#endif  // PARAMETERMANAGEMENG_H
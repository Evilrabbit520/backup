// Config.h
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <filesystem>
#include <string>
#include <unordered_map>

class Tool
{
public:
    /**
     * @brief Get the last time the file was modified (formatted)
     * 
     * @param filePath 
     * @return std::string 
     * 
     * @note Linux system functions
     */
    static std::string getFileCreationTime(const std::filesystem::path &filePath);

public:
    /**
     * @brief Converts formatted datetime strings to timestamps of type time_t
     * 
     * @param timeStr Enter a datetime string in the format "%Y-%m-%d %H:%M:%S" (e.g. "2025-06-05 14:30:00")
     * @return time_t (Returns -1 on failure)
     */
    time_t stringToTimeT(const std::string &timeStr);
    /**
     * @brief Get the File Modification Time object
     * 
     * @param filePath 
     * @return std::string 
     */
    std::string getFileModificationTime(const std::filesystem::path &filePath);
    /**
     * @brief Calculate the SHA256 hash of the file
     * 
     * @param file_path 
     * @return std::string 
     * 
     * @note SHA256 implementation that relies on OpenSSL library (SHA256_Init/Update/Final)
     */
    std::string calculate_sha256(const std::string &file_path);
};

#endif // FILEUTILS_H
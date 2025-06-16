#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <ctime>

/**
 * @brief Get the last time the file was modified (formatted)
 *
 * @param filePath
 * @return std::string
 */
std::string Tool::getFileModificationTime(const std::filesystem::path &filePath)
{
    auto ftime = std::filesystem::last_write_time(filePath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&cftime));
    return std::string(buffer);
}

/**
 * @brief Converts formatted datetime strings to timestamps of type time_t
 * @details Functions automatically handle daylight saving time (via tm_isdst = -1).
 *
 * @param timeStr Enter a datetime string in the format "%Y-%m-%d %H:%M:%S" (e.g. "2025-06-05 14:30:00")
 * @return time_t (Returns -1 on failure)
 */
time_t Tool::stringToTimeT(const std::string &timeStr)
{
    struct tm tm = {0};
    std::istringstream iss(timeStr);
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (iss.fail())
    {
        return -1;
    }
    tm.tm_isdst = -1;
    return mktime(&tm);
}

// Windows
#ifdef _WIN32

// Linux
#elif defined(__linux__)
#include <sys/stat.h>

/**
 * @brief Linux system gitFileCreationTime (There is no test).
 *
 * @param filePath
 * @return std::string
 *
 * @warning ext4 file system needs special handling crtime[8]
 */
std::string Tool::getFileCreationTime(const std::filesystem::path &filePath)
{
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0)
    {
        perror("stat");
        return "Error";
    }

    // Linux uses st_ctime (Time of State Change) as the approximate creation time
    time_t creationTime = fileStat.st_ctime; // Note: ctime is the time when the metadata was modified, not strictly created
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&creationTime));
    return std::string(buffer);

    // Priority is given to birthtime (macOS), Linux needs to check the file system
    time_t time = fileStat.st_ctime; // Or custom logic to handle the crtime of ext4
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return std::string(buffer);
}

// macOS
#elif defined(__APPLE__)
#include <sys/stat.h>

/**
 * @brief macOS system gitFileCreationTime
 *
 * @param filePath
 * @return std::string
 */
std::string Tool::getFileCreationTime(const std::filesystem::path &filePath)
{
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0)
    {
        perror("stat");
        return "Error";
    }

    // macOS uses st_birthtime to get the creation time
    time_t creationTime = fileStat.st_birthtime;
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&creationTime));
    return std::string(buffer);
}

// Other systems
#else
#error "Unsupported operating systems"
#endif

/**
 * @brief Calculate the SHA256 hash of the file
 *
 * @param file_path The path of the file to be hashed (must be absolute)
 * @return std::string A 64-character hexadecimal hash string is returned on success and an empty string is returned on failure
 *
 * @note SHA256 implementation that relies on OpenSSL library (SHA256_Init/Update/Final)
 * @warning Undefined behavior when file size exceeds 2^64 bytes (SHA256 theoretical limit)
 */
std::string Tool::calculate_sha256(const std::string &file_path)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return "";
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    const int buffer_size = 1024;
    std::vector<char> buffer(buffer_size);
    while (file.read(buffer.data(), buffer_size))
    {
        SHA256_Update(&sha256, buffer.data(), file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

/**
 * @brief Calculate the total folder size.
 * 
 * @param fileList 
 * @return uintmax_t 
 */
uintmax_t Tool::calculateFileListSize(const std::vector<std::filesystem::path> &fileList)
{
    uintmax_t totalSize = 0;
    for (const auto &filePath : fileList)
    {
        if (std::filesystem::exists(filePath))
        {
            if (std::filesystem::is_regular_file(filePath))
            {
                totalSize += std::filesystem::file_size(filePath);
            }
            else if (std::filesystem::is_directory(filePath))
            {
                // In the case of a directory, all file sizes under it are recursively calculated
                for (const auto &entry : std::filesystem::recursive_directory_iterator(filePath))
                {
                    if (entry.is_regular_file())
                    {
                        totalSize += entry.file_size();
                    }
                }
            }
        }
    }

    return totalSize;
}

/**
 * @brief Displays the percentage of copies.
 * 
 * @param copied 
 * @param total 
 */
void Tool::showCopyProgress(double copied, double total)
{
    std::cout << "File copied " << std::fixed << std::setprecision(2)
              << (copied / total) * 100.0 << "%\r";
    std::cout.flush();
}
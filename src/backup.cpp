// #include <iostream>
// #include <filesystem>
// #include <fstream>
// #include <chrono>
// #include <vector>
// #include <algorithm>
// #include <ctime>
// #include <iomanip>
// #include <unordered_map>

// #include "../include/nlohmann/json.hpp"

// using json = nlohmann::json;

// /**
//  * @brief simple parameter parser
//  *
//  * @param argc
//  * @param argv
//  * @return std::unordered_map<std::string, std::string>
//  */
// std::unordered_map<std::string, std::string> parse_args(int argc, char *argv[])
// {
//     std::unordered_map<std::string, std::string> args;
//     for (int i = 1; i < argc; ++i)
//     {
//         std::string arg = argv[i];
//         if (arg == "-h" || arg == "--help")
//         {
//             std::cout << "Help:\n"
//                       << "  backup <source_directory> <destination_directory>\n"
//                       << "  backup [--version | -v]\n"
//                       << "  \n"
//                       << "  Usage:\n"
//                       << "  backup <source_directory> <destination_directory> The directory to be backed up is <source_directory>, and the files are backed up to <destination_directory>\n"
//                       << "  \n"
//                       << "  All commands:\n"
//                       << "  --version, --help\n"
//                       << "  \n"
//                       << "  Full backup:\n"
//                       << "  After running, select 1 to perform a full backup. The generated meta file is in the source_directory (you can choose to delete [only perform a full backup next time]) \n"
//                       << "  \n"
//                       << "  Incremental backup:\n"
//                       << "  When performing an incremental backup for the first time, please confirm that the destination_directory exists and is empty. After the first incremental backup, a .btd file will be generated in the source_directory. Please do not delete it, otherwise you will not be able to perform an incremental backup next time.\n"
//                       << "  After running, select 2 to perform an incremental backup. The generated meta file is in the source_directory. If the source_directory does not have a meta file, incremental backup cannot be performed. When the meta file is deleted, a full backup will be performed (note that this is very dangerous. If there is backup data in the destination_directory, it will overwrite the entire destination_directory. This operation is irreversible)\n"
//                       << "  \n"
//                       << "  More:\n"
//                       << "  Please visit the code repository to learn more: https://github.com/Evilrabbit520/backup/\n"
//                       << "  \n";
//             args["help"] = "";

//         }
//         else if (arg == "-v" || arg == "--version")
//         {
//             // backup version
//             args["version"] = "1.0.1";
//         }
//     }
//     return args;
// }

// /**
//  * @brief Obtain the time when the file was last modified
//  *
//  * @param filePath
//  * @return std::string
//  */
// std::string getFileModificationTime(const std::filesystem::path &filePath)
// {
//     auto ftime = std::filesystem::last_write_time(filePath);
//     auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
//         ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
//     std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
//     char buffer[80];
//     std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&cftime));
//     return std::string(buffer);
// }

// /**
//  * @brief Recursively collects all file paths
//  *
//  * @param dir
//  * @param fileList
//  */
// void collectAllFiles(const std::filesystem::path &dir, std::vector<std::filesystem::path> &fileList)
// {
//     for (const auto &entry : std::filesystem::recursive_directory_iterator(dir))
//     {
//         if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
//         {
//             fileList.push_back(entry.path());
//         }
//     }
// }

// /**
//  * @brief Generate a backup metadata file
//  *
//  * @param sourceDir
//  * @param backupDir
//  * @param isIncremental
//  */
// void generateBackupMetadata(
//     const std::filesystem::path &sourceDir,
//     const std::filesystem::path &backupDir,
//     bool isIncremental)
// {
//     json backupData;
//     backupData["location"] = json::array();

//     json locationData;
//     locationData["directory"] = sourceDir.string();
//     locationData["fullReserves"] = isIncremental ? 0 : 1;
//     locationData["incrementalReserves"] = isIncremental ? 1 : 0;
//     locationData["lastBakTime"] = getFileModificationTime(std::filesystem::current_path());

//     int fileCount = 0;
//     json listFiles;

//     /**
//      * @brief Recursively traverses all files
//      *
//      * @param std::filesystem::recursive_directory_iterator
//      */
//     for (const auto &entry : std::filesystem::recursive_directory_iterator(sourceDir))
//     {
//         if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
//         {
//             fileCount++;
//             json fileData;
//             fileData["fileName"] = std::filesystem::relative(entry.path(), sourceDir).string(); // 保存相对路径
//             fileData["fileSize(Byte)"] = entry.file_size();
//             fileData["creation"] = getFileModificationTime(entry.path());
//             fileData["modified"] = getFileModificationTime(entry.path());

//             listFiles[std::filesystem::relative(entry.path(), sourceDir).string()] = fileData;
//         }
//     }

//     locationData["fileCount"] = fileCount;
//     locationData["listFiles"] = listFiles;
//     backupData["location"].push_back(locationData);

//     std::stringstream ss;
//     ss << "backup_timestamp.btd";

//     std::ofstream metadataFile(sourceDir / ss.str());
//     metadataFile << backupData.dump(4);
//     metadataFile.close();
// }

// // Obtaining the List of Files to Be Backed Up (Incremental Backup)
// std::vector<std::filesystem::path> getFilesToBackup(
//     const std::filesystem::path &sourceDir,
//     const std::filesystem::path &backupDir,
//     const std::filesystem::path &metadataFile)
// {
//     std::vector<std::filesystem::path> filesToBackup;

//     // Read the metadata file
//     std::ifstream input(metadataFile);
//     json backupData;
//     input >> backupData;

//     // Calculate the target file path (maintain a relative directory structure)
//     for (const auto &entry : std::filesystem::recursive_directory_iterator(sourceDir))
//     {
//         if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
//         {
//             bool needBackup = true;

//             // Calculate the target file path (maintain a relative directory structure)
//             std::filesystem::path relativePath = std::filesystem::relative(entry.path(), sourceDir);
//             std::filesystem::path destFile = backupDir / relativePath;

//             // Make sure that the target directory exists
//             std::filesystem::create_directories(destFile.parent_path());

//             // Check whether there are any files with the same name in the destination directory
//             if (std::filesystem::exists(destFile))
//             {
//                 // Compare modification times
//                 auto sourceTime = std::filesystem::last_write_time(entry.path());
//                 auto destTime = std::filesystem::last_write_time(destFile);

//                 if (sourceTime <= destTime)
//                 {
//                     needBackup = false;
//                 }
//             }

//             // Check the records in the metadata
//             if (backupData["location"][0]["listFiles"].contains(relativePath.string()))
//             {
//                 auto metadataTimeStr = backupData["location"][0]["listFiles"][relativePath.string()]["modified"];
//                 // Here you need to convert the string time to time_t for comparison
//                 // Simplified processing: Skip if the file exists and there is a record in the metadata
//                 needBackup = false;
//             }

//             if (needBackup)
//             {
//                 filesToBackup.push_back(entry.path());
//             }
//         }
//     }

//     return filesToBackup;
// }

// int main(int argc, char *argv[])
// {
//     auto args = parse_args(argc, argv);
//     if (argc != 3 || argc < 2)
//     {
//         if (args.count("version"))
//         {
//             std::cout << "Version " << args["version"] << std::endl;
//             exit(0);
//         }
//         else if (args.count("help"))
//         {
//             std::cout << "Goodbye~ " << std::endl;
//             exit(0);
//         }
//         std::cerr << "backup usage: backup\n"
//                       <<"                        " << argv[0] << " <source_directory> <destination_directory>\n"
//                       <<"                        [--version | -v | --help | -h]\n";
//                       exit(0);
//         return 1;
//     }

//     std::filesystem::path sourceDir = std::filesystem::absolute(argv[1]);
//     std::filesystem::path backupDir = std::filesystem::absolute(argv[2]);

//     if (!std::filesystem::exists(sourceDir) || !std::filesystem::is_directory(sourceDir))
//     {
//         std::cerr << "Source directory does not exist or is not a directory\n";
//         return 1;
//     }

//     if (!std::filesystem::exists(backupDir))
//     {
//         std::filesystem::create_directories(backupDir);
//     }

//     std::cout << "Select backup type:\n";
//     std::cout << "1. Full backup\n";
//     std::cout << "2. Incremental backup\n";
//     std::cout << "Enter choice (1 / 2): ";

//     int choice;
//     std::cin >> choice;
//     bool isIncremental = (choice == 2);

//     // Find the most recent metadata file (for incremental backups)
//     std::filesystem::path latestMetadata;
//     if (isIncremental)
//     {
//         for (const auto &entry : std::filesystem::directory_iterator(sourceDir))
//         {
//             if (entry.path().extension() == ".btd" &&
//                 (latestMetadata.empty() ||
//                  entry.path().filename().string() >
//                      latestMetadata.filename().string()))
//             {
//                 latestMetadata = entry.path();
//             }
//         }
//     }

//     // Get a list of files that need to be backed up
//     std::vector<std::filesystem::path> filesToBackup;
//     if (isIncremental && !latestMetadata.empty())
//     {
//         filesToBackup = getFilesToBackup(sourceDir, backupDir, latestMetadata);
//     }
//     else
//     {
//         // Full Backup - Recursively back up all files
//         for (const auto &entry : std::filesystem::recursive_directory_iterator(sourceDir))
//         {
//             if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
//             {
//                 filesToBackup.push_back(entry.path());
//             }
//         }
//     }

//     // Displays the files that will be backed up
//     std::cout << "\nFiles to be backed up:\n";
//     for (const auto &file : filesToBackup)
//     {
//         std::cout << " - " << std::filesystem::relative(file, sourceDir) << "\n";
//     }

//     // User confirmation
//     std::cout << "\nProceed with backup? (y/n): ";
//     char confirm;
//     std::string arr = "";
//     std::cin >> confirm;

//     if (confirm != 'y' && confirm != 'Y')
//     {
//         std::cout << "Backup cancelled.\n";
//         return 0;
//     }
//     else
//     {
//         std::cout << "The full backup operation cannot be reversed, so make sure that the destination folder is empty or that you are sure to overwrite it." << std::endl;
//         ;
//         std::cout << "Are you sure? (sure / not sure)" << std::endl;
//         std::cin >> arr;
//         if (arr != "sure")
//         {
//             std::cout << "A full backup is not performed.\n";
//             return 0;
//         }
//     }

//     // Perform backups (maintain directory structure)
//     for (const auto &file : filesToBackup)
//     {
//         std::filesystem::path relativePath = std::filesystem::relative(file, sourceDir);
//         std::filesystem::path destFile = backupDir / relativePath;

//         // Make sure that the target directory exists
//         std::filesystem::create_directories(destFile.parent_path());

//         std::filesystem::copy_file(file, destFile, std::filesystem::copy_options::overwrite_existing);
//         std::cout << "Backed up: " << relativePath << "\n";
//     }

//     // Generate a metadata file
//     generateBackupMetadata(sourceDir, backupDir, isIncremental);

//     std::cout << "\nBackup completed successfully.\n";
//     return 0;
// }

#include "BackupManager.h"



int main(int argc, char *argv[])
{
    BackupManager backupManager;
    return backupManager.run(argc, argv);
}
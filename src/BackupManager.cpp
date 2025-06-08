#include "FileUtils.h"
#include "BackupManager.h"
#include "ParameterManagement.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * @brief Run the backup program
 * @details The whole process of running the main program.
 */
int BackupManager::run(int argc, char *argv[])
{
    auto args = Parameter::parseArgs(argc, argv);
    if (argc != 3 || argc < 2)
    {
        if (args.count("version"))
        {
            std::cout << "Version " << args["version"] << std::endl;
            return 0;
        }
        else if (args.count("help"))
        {
            std::cout << "Goodbye~ " << std::endl;
            return 0;
        }
        std::cerr << "backup usage: backup\n"
                  << "                        " << argv[0] << " <source_directory> <destination_directory>\n"
                  << "                        [--version | -v | --help | -h]\n";
        return 1;
    }

    sourceDir = std::filesystem::absolute(argv[1]);
    backupDir = std::filesystem::absolute(argv[2]);

    if (!validateDirectories())
    {
        return 1;
    }

    if (!getBackupTypeFromUser())
    {
        return 0;
    }

    if (!prepareBackupFiles())
    {
        return 1;
    }

    if (!confirmBackup())
    {
        return 0;
    }

    performBackup();
    generateBackupMetadata();

    std::cout << "\nBackup completed successfully.\n";
    return 0;
}

/**
 * @brief Verify catalog validity
 */
bool BackupManager::validateDirectories()
{
    if (!std::filesystem::exists(sourceDir) || !std::filesystem::is_directory(sourceDir))
    {
        std::cerr << "Source directory does not exist or is not a directory\n";
        return false;
    }

    if (!std::filesystem::exists(backupDir))
    {
        std::filesystem::create_directories(backupDir);
    }
    return true;
}

/**
 * @brief Obtain the backup type from the user operation
 */
bool BackupManager::getBackupTypeFromUser()
{
    std::cout << "Select backup type:\n";
    std::cout << "1. Full backup\n";
    std::cout << "2. Incremental backup\n";
    std::cout << "Enter choice (1 / 2): ";

    int choice;
    std::cin >> choice;
    isIncremental = (choice == 2);
    return true;
}

/**
 * @brief Prepare a list of files to be backed up
 */
bool BackupManager::prepareBackupFiles()
{
    // Find the most recent metadata file (for incremental backups)
    std::filesystem::path latestMetadata;
    if (isIncremental)
    {
        for (const auto &entry : std::filesystem::directory_iterator(sourceDir))
        {
            if (entry.path().extension() == ".btd" &&
                (latestMetadata.empty() ||
                 entry.path().filename().string() >
                     latestMetadata.filename().string()))
            {
                latestMetadata = entry.path();
            }
        }
    }

    if (isIncremental && !latestMetadata.empty())
    {
        filesToBackup = getFilesToBackup(latestMetadata);
    }
    else
    {
        // Full Backup - Recursively back up all your files
        for (const auto &entry : std::filesystem::recursive_directory_iterator(sourceDir))
        {
            if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
            {
                filesToBackup.push_back(entry.path());
            }
        }
    }

    // Statistics on the number of files and modifications
    int FilesCount = getNewFilesCount();
    int ChangeCount = getNewChangeCount();

    std::cout << "\nFiles to be backed up:\n";

    for (const auto &file : filesToBackup)
    {
        std::cout << " + " << std::filesystem::relative(file, sourceDir) << "\n";
    }

    if (FilesCount != 0 || ChangeCount != 0)
    {
        std::cout << "(Since the last backup, " << FilesCount
                  << " new files were detected, " << ChangeCount << " files were updated, totaling "
                  << (FilesCount + ChangeCount) << " files added to the backup list)";
    }

    return true;
}

/**
 * @brief Gets a list of files that need to be backed up (incremental backups), including some operations
 * @details Try to calculate the file modification time first, and then calculate SHA256 if it is inconsistent.
 * 
 * @param metadataFile
 * @return std::vector<std::filesystem::path>
 */
std::vector<std::filesystem::path> BackupManager::getFilesToBackup(const std::filesystem::path &metadataFile)
{
    std::vector<std::filesystem::path> files;

    int FilesCount = 0, ChangeCount = 0;
    try
    {
        // Read the metadata file
        std::ifstream input(metadataFile);
        json backupData;
        input >> backupData;

        for (const auto &entry : std::filesystem::recursive_directory_iterator(sourceDir))
        {

            if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
            {
                std::filesystem::path relativePath = std::filesystem::relative(entry.path(), sourceDir);
                std::filesystem::path destFile = backupDir / relativePath;

                // Files that do not have a target directory or metafile (new additions are also performed through this)
                if (!std::filesystem::exists(destFile) || !backupData["location"][0]["listFiles"].contains(relativePath.string()))
                {
                    FilesCount++;
                    files.push_back(entry.path());
                }
                
                // Both the source directory and the target directory (i.e., the meta file also contains)
                if (std::filesystem::exists(sourceDir) && backupData["location"][0]["listFiles"].contains(relativePath.string()))
                {
                    // The time when the source directory file was modified and SHA-256
                    auto sourceTime = tool.stringToTimeT(tool.getFileModificationTime(entry.path()));
                    std::string currentSHA256 = tool.calculate_sha256(entry.path());

                    // The time when the object was modified and SHA-256
                    auto destTime = tool.stringToTimeT(backupData["location"][0]["listFiles"][relativePath.string()]["modified"]);
                    std::string metadataSHA256 = backupData["location"][0]["listFiles"][relativePath.string()]["sha256"];

                    if (sourceTime > destTime || metadataSHA256 != currentSHA256)
                    {
                        ChangeCount++;
                        files.push_back(entry.path());
                    }
                }
            }
        }
    }
    catch (const json::exception &e)
    {
        std::cerr << "JSON processing error: " << e.what() << std::endl;
    }
    setNewFilesCount(FilesCount);
    setNewChangeCount(ChangeCount);
    return files;
}

/**
 * @brief Confirm the backup operation
 */
bool BackupManager::confirmBackup()
{
    std::cout << "\nProceed with backup? (y/n): ";
    char confirm;
    std::string arr = "";
    std::cin >> confirm;

    if (confirm != 'y' && confirm != 'Y')
    {
        std::cout << "Backup cancelled.\n";
        return false;
    }
    else
    {
        std::cout << "The full backup operation cannot be reversed, so make sure that the destination folder is empty or that you are sure to overwrite it." << std::endl;
        std::cout << "Are you sure? (sure / not sure)" << std::endl;
        std::cin >> arr;
        if (arr != "sure")
        {
            std::cout << "A full backup is not performed.\n";
            return false;
        }
    }
    return true;
}

/**
 * @brief Perform a backup operation
 */
void BackupManager::performBackup()
{
    // Perform backups (maintain directory structure)
    for (const auto &file : filesToBackup)
    {
        std::filesystem::path relativePath = std::filesystem::relative(file, sourceDir);
        std::filesystem::path destFile = backupDir / relativePath;

        // Make sure that the target directory exists
        std::filesystem::create_directories(destFile.parent_path());

        std::filesystem::copy_file(file, destFile, std::filesystem::copy_options::overwrite_existing);
        std::cout << "Backed up: " << relativePath << "\n";
    }
}

/**
 * @brief Generate a backup metadata file
 */
void BackupManager::generateBackupMetadata()
{
    json backupData;
    backupData["location"] = json::array();

    // Check if the file exists, and if so, load the existing data
    std::ifstream inputFile(sourceDir / "backup_timestamp.btd");
    if (inputFile.good())
    {
        try
        {
            backupData = json::parse(inputFile);
        }
        catch (...)
        {
            std::cerr << "The file failed to open or was in an abnormal state!" << std::endl;
        }
        inputFile.close();
    }

    // Check if there are already entries for the same directory
    bool locationExists = false;
    for (auto &location : backupData["location"])
    {
        if (location["directory"] == sourceDir.string())
        {
            // Update an existing entry
            location["fullReserves"] = isIncremental ? 0 : 1;
            location["incrementalReserves"] = isIncremental ? 1 : 0;
            location["lastBakTime"] = tool.getFileModificationTime(std::filesystem::current_path());

            // Get a list of existing files
            json existingFiles = location["listFiles"];
            int existingCount = location["fileCount"];

            // Traverse the directory to find new files
            for (const auto &entry : std::filesystem::recursive_directory_iterator(sourceDir))
            {
                if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
                {
                    std::string relPath = std::filesystem::relative(entry.path(), sourceDir).string();

                    // Add the file if it doesn't exist in the existing list
                    if (existingFiles.find(relPath) == existingFiles.end())
                    {
                        json fileData;

                        fileData["fileName"] = std::filesystem::relative(entry.path(), sourceDir).filename().string();
                        fileData["fileSize(Byte)"] = entry.file_size();
                        fileData["creation"] = tool.getFileCreationTime(entry.path());
                        fileData["modified"] = tool.getFileModificationTime(entry.path());
                        fileData["sha256"] = tool.calculate_sha256(entry.path());

                        existingFiles[relPath] = fileData;
                        existingCount++;
                    }
                    else
                    {
                        auto &fileData = existingFiles[relPath];

                        auto currentModifiedTime = tool.stringToTimeT(tool.getFileModificationTime(entry.path()));
                        auto lastModifiedTime = tool.stringToTimeT(backupData["location"][0]["listFiles"][relPath]["modified"]);

                        if (currentModifiedTime != lastModifiedTime)
                        {
                            // Update time and SHA-256
                            fileData["modified"] = tool.getFileModificationTime(entry.path());
                            ;
                            fileData["sha256"] = tool.calculate_sha256(entry.path());
                        }
                    }
                }
            }

            location["fileCount"] = existingCount;
            location["listFiles"] = existingFiles;
            locationExists = true;
            break;
        }
    }

    if (!locationExists)
    {
        json locationData;
        locationData["directory"] = sourceDir.string();
        locationData["fullReserves"] = isIncremental ? 0 : 1;
        locationData["incrementalReserves"] = isIncremental ? 1 : 0;
        locationData["lastBakTime"] = tool.getFileModificationTime(std::filesystem::current_path());

        int fileCount = 0;
        json listFiles;

        for (const auto &entry : std::filesystem::recursive_directory_iterator(sourceDir))
        {
            if (entry.is_regular_file() && entry.path().filename() != "backup_timestamp.btd")
            {
                fileCount++;
                json fileData;
                fileData["fileName"] = std::filesystem::relative(entry.path(), sourceDir).filename().string();
                fileData["fileSize(Byte)"] = entry.file_size();
                fileData["creation"] = tool.getFileCreationTime(entry.path());
                fileData["modified"] = tool.getFileModificationTime(entry.path());
                fileData["sha256"] = tool.calculate_sha256(entry.path());

                listFiles[std::filesystem::relative(entry.path(), sourceDir).string()] = fileData;
            }
        }

        locationData["fileCount"] = fileCount;
        locationData["listFiles"] = listFiles;
        backupData["location"].push_back(locationData);
    }

    std::ofstream metadataFile(sourceDir / "backup_timestamp.btd");
    metadataFile << backupData.dump(4);
    metadataFile.close();
}
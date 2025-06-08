#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H

#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

class BackupManager
{
public:
    BackupManager() = default;

    void setNewFilesCount(int count) { newFilesCount = count; }     // Set the New Files Count object
    int getNewFilesCount() const { return newFilesCount; }          // Get the New Files Count object

    void setNewChangeCount(int count) { newChangeCount = count; }   // Set the New Change Count object
    int getNewChangeCount() const { return newChangeCount; }        // Get the New Change Count object
    
    /**
     * @brief Run the backup program
     * 
     * @param argc 
     * @param argv 
     * @return int or exit(0)
     */
    int run(int argc, char *argv[]);

private:
    int newFilesCount = 0;  // Count of new files
    int newChangeCount = 0; // Count of new file modifications

    Tool tool;
    std::filesystem::path sourceDir;
    std::filesystem::path backupDir;
    bool isIncremental = false;
    std::vector<std::filesystem::path> filesToBackup;

    bool validateDirectories();
    bool getBackupTypeFromUser();
    bool prepareBackupFiles();
    /**
     * @brief Get the Files To Backup object
     * 
     * @param metadataFile 
     * @return std::vector<std::filesystem::path> 
     */
    std::vector<std::filesystem::path> getFilesToBackup(const std::filesystem::path &metadataFile);
    bool confirmBackup();
    void performBackup();
    void generateBackupMetadata();
};

#endif  // BACKUPMANAGER_H
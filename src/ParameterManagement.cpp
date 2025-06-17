#include "ParameterManagement.h"
#include <iostream>

/**
 * @brief Simple parameter parser
 *
 * @param argc
 * @param argv
 * @return std::unordered_map<std::string, std::string>
 */
std::unordered_map<std::string, std::string> Parameter::parseArgs(int argc, char *argv[])
{
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            displayHelp();
            args["help"] = "";
        }
        else if (arg == "-v" || arg == "--version")
        {
            args["version"] = "1.2.1";
        }
    }
    return args;
}

/**
 * @brief Displays help information
 */
void Parameter::displayHelp()
{
    std::cout << "Help:\n"
              << "  backup <source_directory> <destination_directory>\n"
              << "  backup [--version | -v]\n"
              << "  \n"
              << "  Usage:\n"
              << "  backup <source_directory> <destination_directory> The directory to be backed up is <source_directory>, and the files are backed up to <destination_directory>\n"
              << "  \n"
              << "  All commands:\n"
              << "  --version, --help\n"
              << "  \n"
              << "  Full backup:\n"
              << "  After running, select 1 to perform a full backup. The generated meta file is in the source_directory (you can choose to delete [only perform a full backup next time]) \n"
              << "  \n"
              << "  Incremental backup:\n"
              << "  When performing an incremental backup for the first time, please confirm that the destination_directory exists and is empty. After the first incremental backup, a .btd file will be generated in the source_directory. Please do not delete it, otherwise you will not be able to perform an incremental backup next time.\n"
              << "  After running, select 2 to perform an incremental backup. The generated meta file is in the source_directory. If the source_directory does not have a meta file, incremental backup cannot be performed. When the meta file is deleted, a full backup will be performed (note that this is very dangerous. If there is backup data in the destination_directory, it will overwrite the entire destination_directory. This operation is irreversible)\n"
              << "  \n"
              << "  More:\n"
              << "  Please visit the code repository to learn more: https://github.com/Evilrabbit520/backup/\n"
              << "  \n";
}

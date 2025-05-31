# File Backup Utility / 文件备份工具

这是一个C++命令行工具，用于执行完整或增量的文件备份，并跟踪元数据。

This is a C++ command-line utility for performing full or incremental file backups with metadata tracking.

## 功能特性 / Features

- **完整备份** / **Full Backups**: 从源到目标的完整文件复制(Complete copy of all files from source to destination)
- **增量备份** / **Incremental Backups**: 仅备份自上次备份后更改的文件(Only copies files changed since last backup)
- **元数据跟踪** / **Metadata Tracking**: 
  - 记录文件修改时间(Records file modification times)
  - 以JSON格式维护备份历史(Maintains backup history in JSON format)
  - 跟踪相对路径以保持正确的目录结构(Tracks relative paths for proper directory structure preservation)

## 系统要求 / Requirements
- C++20 兼容编译器(C++20 compatible compiler)
- CMake (用于构建)(CMake (for building))
- nlohmann/json库(nlohmann/json library)

## 安装指南 / Installation
1. 克隆仓库(Clone the repository)

   ```
   git clone https://github.com/Evilrabbit520/backup.git
   ```
2. 构建项目(Build the project)

   ```
   mkdir build && cd build
   cmake ..
   make
   ```

## 使用方法 / Usage

```
# ./backup "源目录" "目标目录"
./backup "source_directory" "destination_directory"
```

程序将：
The program will:

1. 提示选择备份类型(完整或增量)(Prompt to select backup type (full or incremental))
2. 显示将要备份的文件列表(Show the list of files to be backed up)
3. 在执行前请求确认(Request confirmation before proceeding)
4. 执行备份同时保持目录结构(Perform the backup while maintaining directory structure)
5. 在目标目录生成元数据文件(`backup_timestamp.btd`)(Generate metadata file (`backup_timestamp.btd`) in the destination)

## 元数据文件格式 / Metadata File Format

``` json
{
    "location": [
        {
            "directory": "source/path",
            "fullReserves": 0,
            "incrementalReserves": 1,
            "lastBakTime": "2025-05-30 14:30:00",
            "fileCount": 1,
            "listFiles": {
                "relative/path/file.txt": {
                    "fileName": "relative/path/file.txt",
                    "fileSize(Byte)": 1024,
                    "creation": "2025-05-30 09:15:00",
                    "modified": "2025-05-30 16:45:00"
                }
            }
        }
    ]
}
```

## 重要说明 / Important Notes

- 程序会在备份过程中覆盖目标目录中的现有文件(The program will overwrite existing files in the destination during backup)
- 完整备份需要明确确认，因为无法撤销(Full backups require explicit confirmation as they cannot be undone)
- 元数据文件(`backup_timestamp.btd`)会被排除在备份之外(The metadata file (`backup_timestamp.btd`) is excluded from backups)

## 许可证 / License

本项目采用 Apache License 2.0 许可证开源  

[Apache License 2.0](./LICENSE)
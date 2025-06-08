<h1 align="center">File Backup Utility / 文件备份工具</h1>

<p align="center">
  <img src="https://img.shields.io/badge/C++-20-blue.svg?style=flat-square" alt="C++20">
  <img src="https://img.shields.io/badge/License-Apache%202.0-green.svg?style=flat-square" alt="License">
  <img src="https://img.shields.io/badge/Platform-macOS-lightgrey.svg?style=flat-square" alt="macOS Platform">
</p>

> Backup 是一个 C++ 命令行工具，用于执行完整或增量的文件备份，并跟踪元数据。  
> This is a C++ command-line utility for performing full or incremental file backups with metadata tracking.

## 🚀 功能特性 / Features

- **🔍 完整备份** / **Full Backups**  
  ▸ 从源到目标的完整文件复制  
  ▸ *Complete copy of all files from source to destination*

- **🔄 增量备份** / **Incremental Backups**  
  ▸ 仅备份自上次备份后更改的文件  
  ▸ *Only copies files changed since last backup*

- **📊 元数据跟踪** / **Metadata Tracking**  
  ▸ 记录文件修改时间  
  ▸ *Records file modification times*  
  ▸ 以JSON格式维护备份历史  
  ▸ *Maintains backup history in JSON format*  
  ▸ 跟踪相对路径以保持正确的目录结构  
  ▸ *Tracks relative paths for proper directory structure preservation*

## ⚙️ 系统要求 / Requirements

| 组件 | 要求 |
|------|------|
| **编译器** | C++20 兼容编译器 |
| **构建工具** | CMake ≥ 3.10 |
| **依赖库** | nlohmann/json, OpenSSL |

## 📥 安装指南 / Installation

### 1. 克隆仓库
```bash
git clone https://github.com/Evilrabbit520/backup.git
cd backup
```

### 2. 安装依赖
```bash
# Linux (Ubuntu/Debian)
sudo apt update && sudo apt install openssl libssl-dev

# Linux (CentOS/RHEL)
sudo yum install openssl openssl-devel

# macOS (Homebrew)
brew install openssl
echo 'export PATH="/opt/homebrew/opt/openssl@3/bin:$PATH"' >> ~/.zshrc

# Windows
# 从 https://slproweb.com/products/Win32OpenSSL.html 下载安装
```

### 3. 构建项目
```bash
mkdir build && cd build
cmake .. && make
```

## 🖥️ 使用方法 / Usage

```bash
./backup "源目录" "目标目录"
# 或
./backup "source_directory" "destination_directory"
```

**工作流程**:
1. 选择备份类型（完整/增量）
2. 预览待备份文件列表
3. 确认执行操作
4. 自动生成元数据文件 `backup_timestamp.btd`

## 📂 元数据文件示例 / Metadata Example

```json
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
                    "creation": "2025-05-30 14:30:00",
                    "fileName": "file.txt",
                    "fileSize(Byte)": 813,
                    "modified": "2025-05-30 14:35:00",
                    "sha256": "e3b0c44298fc1c149afaf4c8992fb92427ae41e4649b934ca495991b78e2b855"
                }
            }
        }
    ]
}
```

## ⚠️ 重要说明 / Important Notes

- 备份操作会覆盖目标目录中的现有文件
- 完整备份操作不可逆，请谨慎确认
- 元数据文件(`backup_timestamp.btd`)不会被备份

## 📜 许可证 / License

本项目采用 [Apache License 2.0](./LICENSE) 开源授权  

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

#pragma once

#include "Engine/Core/Core.h"
#include "StringId.h"

#define MAX_PATH_LENGTH 255

enum PathError
{
    NONE = 0,
    NO_EXTENSION,
    NO_DIRECTORY,
    NO_FILE,
    INVALID_PATH_CHARS
};

class Path
{
public:
    static PathError GetExtension(const std::string& path, std::string& outExtension);
    static PathError GetDirectory(const std::string& path, std::string& outDirectory);
    static PathError GetBaseName(const std::string& path, std::string& outBaseName);
    static PathError GetBaseNameExtension(const std::string& path, std::string& outBaseNameExtension);

    Path();
    Path(const char* path);
    Path(const char* folder, const char* file);
    Path(const StringId& pathId);

    bool operator==(const Path& Other) const;
    Path operator+(const Path& Other) const;
    Path& operator+=(const Path& Other);

    PathError GetExtension(std::string& outExtension) const;
    PathError GetDirectory(std::string& outDirectory) const;
    PathError GetBaseName(std::string& outBaseName) const;
    PathError GetBaseNameExtension(std::string& outBaseNameExtension) const;
    std::string GetFullPath() const;
    const char* GetFullPathRef() const;

    bool IsFile() const;
    bool IsDirectory() const;
    bool IsValid() const;
    StringId GetPathId() const { return m_PathId; }

private:
    friend std::ostream& operator << (std::ostream& os, const Path& path);

    /* Cleans up invalid tokens from path */
    void Sanitize(const char* path);
private:
    StringId m_PathId;
};
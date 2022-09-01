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
    static PathError GetExtension(const std::string& path, std::string& outExtension, bool bWithDelimeter = false);
    static PathError GetDirectory(const std::string& path, std::string& outDirectory);
    static PathError GetBaseName(const std::string& path, std::string& outBaseName);
    static PathError GetBaseNameExtension(const std::string& path, std::string& outBaseNameExtension);
    static bool IsFile(const char* path);
    static bool IsDirectory(const char* dir);

    static void StaticInit();
    
    Path();
    Path(const char* path);
    Path(const char* folder, const char* file);
    Path(StringId const& pathId);

    bool operator==(const Path& Other) const;
    Path operator+(const Path& Other) const;
    Path& operator+=(const Path& Other);

    PathError GetExtension(std::string& outExtension, bool bWithDelimeter = false) const;
    PathError GetDirectory(std::string& outDirectory) const;
    PathError GetBaseName(std::string& outBaseName) const;
    PathError GetBaseNameExtension(std::string& outBaseNameExtension) const;
    std::string GetFullPath() const;
    const char* GetFullPathRef() const;

    bool IsFile() const;
    bool IsDirectory() const;
    bool IsValid() const;
    StringId GetPathId() const { return StringId(m_pathStr); }

private:
    friend std::ostream& operator << (std::ostream& os, const Path& path);

    /* Cleans up invalid tokens from path */
    void Sanitize(const char* path);
    void InitPath(const char* path, const char* optionalFolder);
private:
    char m_pathStr[MAX_PATH_LENGTH];
    size_t m_pathLength;
};

namespace std {
    template <> struct hash<Path>
    {
        size_t operator()(const Path& x) const
        {
            return hash<StringId>()(x.GetPathId());
        }
    };
}

extern bool INVALID_TOKEN_TABLE[255];
extern bool INVALID_EXT_TOKEN_TABLE[255];
extern bool INVALID_DIRECTORY_TOKEN_TABLE[255];
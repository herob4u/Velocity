#include "vctPCH.h"
#include "Path.h"

static const std::string INVALID_FILE_TOKENS = "\"' ,/.:|&!~\n\r\t@#(){}[]=;^%$`";
static const std::string INVALID_EXT_TOKENS = INVALID_FILE_TOKENS;
static const std::string INVALID_DIRECTORY_TOKENS = "\"' ,|&!~\n\r\t@#(){}[]=;^%$`";

static bool INVALID_FILE_TOKEN_TABLE[255] = {false};
static bool INVALID_EXT_TOKEN_TABLE[255] = {false};
static bool INVALID_DIRECTORY_TOKEN_TABLE[255] = {false};

PathError Path::GetExtension(const std::string& path, std::string& outExtension, bool bWithDelimeter)
{
    ASSERT(path.length(), "Expected non-empty path string");
    for(int i = (int)path.size() - 1; i >= 0; i--)
    {
        if(path[i] == '.')
        {
            if(!bWithDelimeter)
                outExtension.assign(&path[i+1], path.size() - (i+1));
            else
                outExtension.assign(&path[i], path.size() - (i));
            return PathError::NONE;
        }
    }

    return PathError::NO_EXTENSION;
}

PathError Path::GetDirectory(const std::string& path, std::string& outDirectory)
{
    ASSERT(path.length(), "Expected non-empty path string");
    outDirectory.assign(path);
    
    for(int i = (int)path.length() - 1; i > 0; i--)
    {
        if(outDirectory[i] == '/' || outDirectory[i] == '\\')
            return PathError::NONE;
        else
            outDirectory[i] = '\0';
    }

    return PathError::NO_DIRECTORY;
}

PathError Path::GetBaseName(const std::string& path, std::string& outBaseName)
{
    ASSERT(path.length(), "Expected non-empty path string");
    // Begin marks the first character of the basename
    // End marks the last character of the basename
    int begin = 0;
    int end = (int)path.length() - 1;

    for(int i = 0, j = (int)path.length() - 1; i <= j; i++, j--)
    {
        if(path[i] == '/' || path[i] == '\\')
            begin = i + 1;

        if(path[j] == '.')
            end = j - 1;
    }

    // Can only be caused if input string has invalid construction
    if(begin > end
    || begin >= path.length()
    || end < 0)
    {
        return PathError::INVALID_PATH_CHARS;
    }

    outBaseName.assign(&path[begin], &path[end+1]);
    return PathError::NONE;
}

PathError Path::GetBaseNameExtension(const std::string& path, std::string& outBaseNameExtension)
{
    ASSERT(path.length(), "Expected non-empty path string");
    int begin = 0;
    for(int i = (int)path.length() - 1; i >= 0; i--)
    {
        if(path[i] == '/' || path[i] == '\\')
            begin = i+1;
    }

    if(begin >= path.length())
        return PathError::NO_FILE;
    
    outBaseNameExtension.assign(&path[begin]);
    return PathError::NONE;
}

bool Path::IsFile(const char* path)
{
    if(!path)
        return false;


    int i = 0;
    int lastSlash = -1;
    while(i < MAX_PATH_LENGTH && path[i] != '\n')
    {
        if(path[i] == '/') { lastSlash = i; }

        else if(path[i] == '.')
        {
            // Found extension. First verify no weird format like "/.ext"
            // Must be in form of "/a.ext" at least
            if(lastSlash != -1 && (i - lastSlash) <= 1)
            {
                return false;
            }

            ++i;

            // Ensure the end of the extension is found without special characters
            while(i < MAX_PATH_LENGTH && path[i] != '\n')
            {
                if(INVALID_EXT_TOKEN_TABLE[path[i++]])
                {
                    return false;
                }
            }

            return true;
        }

        ++i;
    }

    return false;
}

bool Path::IsDirectory(const char* dir)
{
    if(!dir) { return false; }

    int i = 0;
    while(i < MAX_PATH_LENGTH && dir[i] != '\n')
    {
        if(INVALID_DIRECTORY_TOKEN_TABLE[dir[i]])
        {
            return false;
        }
    }

    // Folders need trailing slash
    return i > 0 && dir[i-1] == '/';
}

void Path::StaticInit()
{
    for(char token : INVALID_FILE_TOKENS)
    {
        INVALID_FILE_TOKEN_TABLE[token] = true;
    }

    for(char token : INVALID_DIRECTORY_TOKENS)
    {
        INVALID_DIRECTORY_TOKEN_TABLE[token] = true;
    }

    for(char token : INVALID_EXT_TOKENS)
    {
        INVALID_EXT_TOKEN_TABLE[token] = true;
    }
}

Path::Path()
    : m_pathStr()
    , m_pathLength(0)
{
    memset(m_pathStr, 0, MAX_PATH_LENGTH);
}

Path::Path(const char* path)
{
    ASSERT(path, "Null path string");
    Sanitize(path);

    InitPath(path, nullptr);
}

Path::Path(const char* folder, const char* file)
{
    ASSERT((folder && file), "Null path strings");
    InitPath(file, folder);
}

Path::Path(StringId const& pathId)
{
    if(pathId != StringId::NONE)
    {
        const char* path = pathId.ToStringRef();
        memcpy(m_pathStr, path, strlen(path));
    }
    
}

bool Path::operator==(const Path& Other) const
{
    return m_pathLength == Other.m_pathLength // trivial reject, speeds up majority of comparisons
    && strcmp(m_pathStr, Other.m_pathStr) == 0;
}

Path Path::operator+(const Path& Other) const
{
    Path newPath = *this;

    if(!Other.IsValid())
    {
        return newPath;
    }

    if(IsFile())
    {
        ASSERT(false, "Invalid Path Concatenation");
        return newPath;
    }

    if(m_pathLength + Other.m_pathLength >= MAX_PATH_LENGTH)
    {
        ASSERT(false, "Concatenated path exceeds maximum length");
        return newPath;
    }

    for(size_t i = 0, j = m_pathLength; i < Other.m_pathLength; ++i, ++j)
    {
        newPath.m_pathStr[j] = Other.m_pathStr[i];
    }

    return newPath;
}

Path& Path::operator+=(const Path& Other)
{
    if(!Other.IsValid())
    {
        return *this;
    }

    if(IsFile())
    {
        ASSERT(false, "Invalid Path Concatenation");
        return *this;
    }

    if(m_pathLength + Other.m_pathLength >= MAX_PATH_LENGTH)
    {
        ASSERT(false, "Concatenated path exceeds maximum length");
        return *this;
    }

    for(size_t i = 0, j = m_pathLength; i < Other.m_pathLength; ++i, ++j)
    {
        m_pathStr[j] = Other.m_pathStr[i];
    }

    return *this;
}

PathError Path::GetExtension(std::string& outExtension, bool bWithDelimeter) const
{
    return Path::GetExtension(m_pathStr, outExtension, bWithDelimeter);
}

PathError Path::GetDirectory(std::string& outDirectory) const
{
    return Path::GetDirectory(m_pathStr, outDirectory);
}

PathError Path::GetBaseName(std::string& outBaseName) const
{
    return Path::GetBaseName(m_pathStr, outBaseName);
}

PathError Path::GetBaseNameExtension(std::string& outBaseNameExtension) const
{
    return Path::GetBaseNameExtension(m_pathStr, outBaseNameExtension);
}

std::string Path::GetFullPath() const
{
    return m_pathStr;
}

const char* Path::GetFullPathRef() const
{
    return m_pathStr;
}

bool Path::IsFile() const
{
    if(!IsValid())
        return false;

    return Path::IsFile(m_pathStr);
}

bool Path::IsDirectory() const
{
    if(!IsValid())
        return false;

    return Path::IsDirectory(m_pathStr);
}

bool Path::IsValid() const
{
    return m_pathLength != 0;
}

void Path::Sanitize(const char* path)
{
}

void Path::InitPath(const char* path, const char* optionalFolder)
{
    // Turn into a function!
    size_t const lenPath = strlen(path);
    size_t const lenFolder = optionalFolder ? strlen(optionalFolder) : 0;

    ASSERT((lenPath + lenFolder)< MAX_PATH_LENGTH, "Invalid path length. Must be less than %i", MAX_PATH_LENGTH);
    m_pathLength = lenPath + lenFolder;

    if(lenFolder > 0)
    {
        memcpy(m_pathStr, optionalFolder, lenFolder);
    }

    memcpy(m_pathStr + lenFolder, path, lenPath);

    if(m_pathStr[MAX_PATH_LENGTH - 1] != '\n' && (m_pathLength) == MAX_PATH_LENGTH)
    {
        ASSERT(false, "Maximum path length reached, but path has no terminating character. This can lead to undefined behavior.");
    }
    else
    {
        m_pathStr[m_pathLength] = '\n';
    }
}

std::ostream& operator<<(std::ostream& os, const Path& path)
{
    os << path.GetFullPathRef();
    return os;
}

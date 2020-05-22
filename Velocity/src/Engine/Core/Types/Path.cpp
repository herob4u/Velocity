#include "vctPCH.h"
#include "Path.h"

static const std::string INVALID_FILE_TOKENS = "\"' ,/.:|&!~\n\r\t@#(){}[]=;^%$`";
static const std::string INVALID_DIRECTORY_TOKENS = "\"' ,|&!~\n\r\t@#(){}[]=;^%$`";

PathError Path::GetExtension(const std::string& path, std::string& outExtension)
{
    ASSERT(path.length(), "Expected non-empty path string");
    for(int i = (int)path.size() - 1; i >= 0; i--)
    {
        if(path[i] == '.')
        {
            outExtension.assign(&path[i+1], path.size() - i);
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

    outBaseName.assign(&path[begin], &path[end]);
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

Path::Path()
    : m_PathId(StringId::NONE)
{
}

Path::Path(const char* path)
{
    ASSERT(path, "Null path string");
    Sanitize(path);
    m_PathId = StringId(path);
}

Path::Path(const char* folder, const char* file)
{
    ASSERT((folder && file), "Null path strings");
    size_t lenFolder = strlen(folder);
    size_t lenFile = strlen(file);

    char* fullpath = (char*)malloc(sizeof(char) * (lenFolder + lenFile + 1));

    /* @TODO: Replace with strcat */
    /* @TODO: Is it not better to decompose PathId to FolderId and FileId? Reduces stale entries in StringIdTable */
    memcpy(fullpath, folder, lenFolder);
    memcpy(&fullpath[lenFolder], file, lenFile);
    fullpath[lenFolder+lenFile] = '\0';

    m_PathId = StringId(fullpath);

    free(fullpath);
}

Path::Path(const StringId& pathId)
    : m_PathId(pathId)
{
}

bool Path::operator==(const Path& Other) const
{
    return m_PathId == Other.m_PathId;
}

Path Path::operator+(const Path& Other) const
{
    ASSERT(!IsFile(), "Invalid Path Concatenation");
    StringId concatId = StringId::Concat(m_PathId, Other.m_PathId);
    return Path(concatId);
}

Path& Path::operator+=(const Path& Other)
{
    ASSERT(!IsFile(), "Invalid Path Concatenation");
    m_PathId = StringId::Concat(m_PathId, Other.m_PathId);
    return *this;
    // TODO: insert return statement here
}

PathError Path::GetExtension(std::string& outExtension) const
{
    std::string path = m_PathId.ToString();
    return Path::GetExtension(path, outExtension);
}

PathError Path::GetDirectory(std::string& outDirectory) const
{
    std::string path = m_PathId.ToString();
    return Path::GetDirectory(path, outDirectory);
}

PathError Path::GetBaseName(std::string& outBaseName) const
{
    std::string path = m_PathId.ToString();
    return Path::GetBaseName(path, outBaseName);
}

PathError Path::GetBaseNameExtension(std::string& outBaseNameExtension) const
{
    std::string path = m_PathId.ToString();
    return Path::GetBaseNameExtension(path, outBaseNameExtension);
}

std::string Path::GetFullPath() const
{
    return m_PathId.ToString();
}

const char* Path::GetFullPathRef() const
{
    return m_PathId.ToStringRef();
}

bool Path::IsFile() const
{
    if(m_PathId == StringId::NONE)
        return false;

    std::string path = m_PathId.ToString();

    if(path[path.length() - 1] == '/')
        return false;

    for(int i = (int)path.length() - 1; i >= 0; i--)
    {
        // Valid file if no strange string such as "Folder/.ext" or ".ext"
        if(path[i] == '.')
        {
            return (i > 0) && (path[i-1] != '/');
        }
    }

    // No delimiters found, points to a base file name.
    return true;
}

bool Path::IsDirectory() const
{
    if(m_PathId == StringId::NONE)
        return false;

    std::string path = m_PathId.ToString();

    return path[path.length() - 1] == '/';
}

bool Path::IsValid() const
{
    return (m_PathId != StringId::NONE);
}

void Path::Sanitize(const char* path)
{
}

std::ostream& operator<<(std::ostream& os, const Path& path)
{
    os << path.GetFullPathRef();
    return os;
}

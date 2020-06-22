#include "vctPCH.h"
#include "ShaderCache.h"

#include "Engine/IO/FileMgr.h"
#include "Engine/IO/FileStream.h"

using namespace Vct;

static const std::string SHADER_CACHE_EXTENSION = "scf";
static const StringId    SHADER_CACHE_EXTENSION_ID = ".scf";

ShaderCache::ShaderCache(const Path& cacheFolder)
    : m_CacheFolder(cacheFolder)
{
}

void ShaderCache::CacheProgram(const Shader& shader)
{
    const StringId& shaderName = shader.GetShaderName();
    
    void* binary = nullptr;
    size_t bytes = 0;

    if(shader.GetBinary(binary, bytes))
    {
        // Update the existing cache, or create a new entry if non-existent.
        auto found = m_BinaryCache.find(shaderName);
        if(found == m_BinaryCache.end())
        {
            m_BinaryCache.emplace(shaderName, ShaderBinary(binary, bytes));
        }
        else
        {
            found->second = ShaderBinary(binary, bytes);
        }

        // Maybe we want saving to be explicit - from an editor or something
        //SaveCacheFile(shaderName, binary, bytes);
    }
}

void ShaderCache::SaveCache()
{
    for(auto it = m_BinaryCache.begin(); it != m_BinaryCache.end(); ++it)
    {
        const void* data    = it->second.Data;
        const size_t bytes  = it->second.Bytes;
        
        SaveCacheFile(it->first, data, bytes);
    }
}

void ShaderCache::SaveCacheFile(const StringId& shaderName, const void* data, size_t bytes)
{
    if(!data || bytes == 0)
        return;

    StringId cacheFilePath = StringId::Concat(m_CacheFolder.GetPathId(), shaderName);     // Absolute path
    cacheFilePath = StringId::Concat(cacheFilePath, SHADER_CACHE_EXTENSION_ID); // .scf extension

    OutputFileStream file;
    file.Open(cacheFilePath.ToStringRef());

    if(file)
    {
        bool success = file.Write(data, bytes);
        if(!success)
        {
            VCT_WARN("Saving cache file failed: Write incomplete");
        }
    }
    else
    {
        VCT_WARN("Saving cache file failed: Cache file does not exist")
    }
}

void ShaderCache::LoadCache(bool bOverwrite)
{
    std::vector<std::string> cacheFiles;
    Scan(cacheFiles);

    // Each entry here is in the form of "filename.scf"
    for(const std::string& cacheFile : cacheFiles)
    {
        std::string cacheFileBase;
        Path::GetBaseName(cacheFile, cacheFileBase);
        
        // By definition, shader cache files are named after their associated Shader name.
        const StringId shaderNameId = cacheFileBase.c_str();
        const StringId cacheFilePath = StringId::Concat(m_CacheFolder.GetPathId(), cacheFile);

        if(!bOverwrite && Contains(shaderNameId))
            continue;

        VCT_INFO("Cache file: {0}", cacheFile);
        InputFileStream file;
        file.Open(cacheFilePath.ToStringRef());

        if(file)
        {
            size_t size = file.Size();
            void* data = malloc(size);

            file.Read(data, size);

            auto found = m_BinaryCache.find(shaderNameId);
            if(found == m_BinaryCache.end())
            {
                m_BinaryCache.emplace(shaderNameId, ShaderBinary(data, size));
            }
            else
            {
                found->second = ShaderBinary(data, size);
            }

            file.Close();
        }
    }
}

const ShaderBinary& ShaderCache::GetPrecompiledShader(const StringId& shaderName)
{
    auto found = m_BinaryCache.find(shaderName);

    if(found != m_BinaryCache.end())
    {
        return found->second;
    }

    return ShaderBinary::Null;
}

void ShaderCache::Scan(std::vector<Path>& outCacheFiles)
{
    FileMgr& fileMgr = FileMgr::Get();
    
    std::vector<std::string> files;
    fileMgr.GetFiles(m_CacheFolder.GetFullPathRef(), files, SHADER_CACHE_EXTENSION);

    if(files.size() == 0)
        return;

    outCacheFiles.reserve(files.size());
    StringId cacheFolder = m_CacheFolder.GetPathId();

    for(const std::string& file : files)
    {
        Path cacheFile = Path(StringId::Concat(cacheFolder, file));
        outCacheFiles.push_back(cacheFile);
    }
}

void ShaderCache::Scan(std::vector<std::string>& outCacheFiles) const
{
    FileMgr& fileMgr = FileMgr::Get();

    fileMgr.GetFiles(m_CacheFolder.GetFullPathRef(), outCacheFiles, "scf");
}

bool ShaderCache::Contains(const StringId& shaderName) const
{
    return (m_BinaryCache.find(shaderName) != m_BinaryCache.end());
}

#pragma once

#include "Engine/Core/Types/Path.h"
#include "Shader.h"

namespace Vct
{
    /*  A struct that wraps around binary data. The structure claims ownership of 
    *   the data, and will automatically free it when destroyed.
    */
    struct ShaderBinary
    {
        ShaderBinary(void* data, size_t bytes, int format)
            : Data(data)
            , Bytes(bytes)
            , Format(format)
        {
        }

        ShaderBinary()
            : Data(nullptr)
            , Bytes(0)
            , Format(0)
        {
        }

        ~ShaderBinary()
        {
            if(Data)
                free(Data);

            Bytes = 0;
            Format = 0;
        }

        FORCEINLINE bool IsEmpty() const { return (!Data || Bytes == 0); }

        static ShaderBinary Null;

        void* Data;
        size_t Bytes;
        int Format;
    };

    /*  The Shader Cache represent a mapping between a shader name and its precompiled binary data.
    *   The cache is initialized upon engine initialization by loading precompiled binary shaders
    *   from disk. Shader loading and compilation is then done rapidly by uploading the binary data
    *   directly instead of compiling from source.
    */
    class ShaderCache
    {
    public:
        ShaderCache(const Path& cacheFolder);
        
        void CacheProgram(const Shader& shader);

        /* Save the contents of the current cache to files */
        void SaveCache();

        /* Load existing cache files into the binary cache if any are available */
        void LoadCache(bool bOverwrite = false);

        /* Retrieves the binary data needed for shaders to be compiled. This is to be retrieved by the shader class when compiling */
        const ShaderBinary& GetPrecompiledShader(const StringId& shaderName);
    private:
        void Scan(std::vector<Path>& outCacheFiles);
        void Scan(std::vector<std::string>& outCacheFiles) const;
        bool Contains(const StringId& shaderName) const;

        void SaveCacheFile(const StringId& shaderName, const void* data, size_t bytes);
    private:
        /* Maps the shader program name to an associated binary buffer */
        std::unordered_map<StringId, ShaderBinary> m_BinaryCache;
        Path m_CacheFolder;
    };
}
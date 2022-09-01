#pragma once

#include "Engine/Core/Types/Path.h"

namespace FilePathUtils
{
    void InitTokenTables();
    bool GetExecutableDirectory();
    bool GetExecutablePath();
}
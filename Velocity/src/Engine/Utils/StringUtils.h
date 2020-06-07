#pragma once

namespace String
{
    void ToUpper(const std::string& source, std::string& dest);
    void ToLower(const std::string& source, std::string& dest);
    void ToUpper(std::string& inOutStr);
    void ToLower(std::string& inOutStr);
    bool Equals(const std::string& A, const std::string& B, bool caseSensitive = true);
}
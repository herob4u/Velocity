#include "vctPCH.h"
#include "StringUtils.h"

#define LOWER(c) ( (c|(1 << 5)) )
#define UPPER(c) ( (c&~(1 << 5)) )

static bool IsLetter(char c)
{
    return (c >= 'A' && c <= 'z');
}

static bool IsUpperCaseLetter(char c)
{
    return (c >= 'A' && c <= 'Z');
}

static bool IsLowerCaseLetter(char c)
{
    return (c >= 'a' && c <= 'z');
}

void String::ToUpper(const std::string& source, std::string& dest)
{
    ASSERT(dest.length() == 0, "Expected empty destination string");
    for(char c : source)
    {
        if(IsLowerCaseLetter(c))
        {
            dest.push_back((c&~(1 << 5)));
        }
        else
        {
            dest.push_back(c);
        }
    }
}

void String::ToLower(const std::string& source, std::string& dest)
{
    ASSERT(dest.length() == 0, "Expected empty destination string");
    for(char c : source)
    {
        if(IsUpperCaseLetter(c))
        {
            dest.push_back((c|(1 << 5)));
        }
        else
        {
            dest.push_back(c);
        }
    }
}

void String::ToUpper(std::string& inOutStr)
{
    for(char& c : inOutStr)
    {
        if(IsLowerCaseLetter(c))
        {
            c &= ~(1 << 5);
        }
    }
}

void String::ToLower(std::string& inOutStr)
{
    for(char& c : inOutStr)
    {
        if(IsUpperCaseLetter(c))
        {
            c |= (1 << 5);
        }
    }
}

bool String::Equals(const std::string& A, const std::string& B, bool caseSensitive)
{
    if(A.length() != B.length())
        return false;

    if(caseSensitive)
    {
        for(int i = 0; i < A.size(); ++i)
        {
            if(A[i] != B[i])
                return false;
        }

        return true;
    }
    else
    {
        for(int i = 0; i < A.size(); ++i)
        {
            if(IsLetter(A[i]) && IsLetter(B[i]))
            {
                if(LOWER(A[i]) != LOWER(B[i]))
                    return false;
            }
            else if(A[i] != B[i])
            {
                return false;
            }
        }

        return true;
    }
}

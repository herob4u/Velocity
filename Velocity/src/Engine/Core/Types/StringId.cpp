#include "vctPCH.h"
#include "StringId.h"

#include "Engine/Core/Core.h"
#include "Engine/Utils/crc32.h"

#include <string>


StringId StringId::NONE = {0, 0};

static StringIdTable s_Table;

StringIdTable::StringIdTable()
{
    Init();
}

void StringIdTable::Init()
{
    RegisterId("None", StringId::NONE.GetIndex());
}

void StringIdTable::RegisterId(const char* displayStr, uint16_t id)
{
    if(IsValidIndex(id))
    {
        //ASSERT(!IsValidElement(id), "A string id is already registered to '{0}'", id);
        StringIdElement& element = Table[id];

        InitElement(element, displayStr, id, nullptr);
    }
}

void StringIdTable::RegisterId(uint16_t id)
{
    RegisterId("", id);
}

void StringIdTable::Add(const char* str, uint16_t& outId, uint16_t& outInstance)
{
    const uint16_t hash = CRC::HashString(str) & (MAX_STRING_IDS - 1);
    ASSERT(IsValidIndex(hash), "Hash index out of range");
    StringIdElement& element = Table[hash];

    outInstance = 0;
    outId = hash;

    // The existing element already matches the new name. Duplicate
    if(strcmp(str, element.DisplayString) == 0)
    {
        outId = element.Id;
        return;
    }

    while(element.Next)
    {
        element = *(element.Next);
        outInstance++;

        // The existing element already matches the new name. Duplicate
        if(strcmp(str, element.DisplayString) == 0)
        {
            outId = element.Id;
            return;
        }
    }

    // No collision, first instance
    if(outInstance == 0)
    {
        InitElement(element, str, hash, nullptr);
    }
    else
    {
        StringIdElement* newElement = new StringIdElement();
        InitElement(*newElement, str, hash, nullptr);
        element.Next = newElement;
        outInstance++;
    }
}

bool StringIdTable::IsValidIndex(uint16_t id) const
{
    return (id >= 0 && id < MAX_STRING_IDS);
}

bool StringIdTable::IsValidElement(uint16_t id) const
{
    return (IsValidIndex(id) && Table[id].DisplayString != nullptr && Table[id].Id == id);
}

const StringIdElement* StringIdTable::Find(uint16_t id, uint16_t instance) const
{
    if(IsValidElement(id))
    {
        const StringIdElement* element = &Table[id];
        for(int i = 0; i < instance; i++)
        {
            if(element->Next == nullptr)
                return nullptr;

            element = element->Next;
        }
        return element;
    }
    return nullptr;
}

void StringIdTable::InitElement(StringIdElement& inOutElement, const char* str, uint16_t id, StringIdElement* next)
{
    ASSERT((strlen(str) < MAX_STRING_LEN), "StrngId exceeds maximum allowable length");
    inOutElement.Id   = id;
    inOutElement.Next = nullptr;
    strcpy(inOutElement.DisplayString, str);
}


/************************************************
**              STRING ID                      **
*************************************************/
StringId::StringId(const std::string& str)
{
    StringIdTable& idTable = GetStringIdTable();
    idTable.Add(str.c_str(), Id, Instance);

#ifdef VCT_DEBUG
    m_DebugString = str;
#endif // VCT_DEBUG

}

StringId::StringId(const char* str)
{
    StringIdTable& table = GetStringIdTable();
    table.Add(str, Id, Instance);

#ifdef VCT_DEBUG
    m_DebugString = str;
#endif // VCT_DEBUG
}

StringId::StringId(const StringId& Other)
    : Id(Other.Id)
    , Instance(Other.Instance)
{
#ifdef VCT_DEBUG
    m_DebugString = Other.m_DebugString;
#endif // VCT_DEBUG
}

StringId StringId::Concat(const StringId& lhs, const StringId& rhs)
{
    StringIdTable& table = GetStringIdTable();

    const StringIdElement* lhs_element = table.Find(lhs.Id, lhs.Instance);
    const StringIdElement* rhs_element = table.Find(rhs.Id, rhs.Instance);

    if(!lhs_element && !rhs_element)
        return StringId::NONE;
    else if(!lhs_element)
        return rhs;
    else if(!rhs_element)
        return lhs;

    const size_t len_lhs = strlen(lhs_element->DisplayString);
    const size_t len_rhs = strlen(rhs_element->DisplayString);

    char concatStr[MAX_STRING_LEN] = {'\0'};
    memcpy(concatStr, lhs_element->DisplayString, len_lhs);
    memcpy(&concatStr[len_lhs], rhs_element->DisplayString, len_rhs);

    return StringId(concatStr);
}

StringId::StringId(uint16_t id, uint16_t instance)
    : Id(id)
    , Instance(instance)
{
}

StringIdTable& StringId::GetStringIdTable()
{
    return s_Table;
}

std::string StringId::ToString() const
{
    const StringIdElement* element = GetStringIdTable().Find(Id, Instance);

    if(element)
    {
        return std::string(element->DisplayString);
    }

    return "";
}

const char* StringId::ToStringRef() const
{
    const StringIdElement* element = GetStringIdTable().Find(Id, Instance);

    if(element)
    {
        return element->DisplayString;
    }

    return nullptr;
}

std::string StringId::DebugDump() const
{
    static std::string titleA = "ID: ";
    static std::string titleB = "Instance: ";
    static std::string titleC = "With String: ";

    return titleA + std::to_string(Id) + '\n'
        +  titleB + std::to_string(Instance) + '\n'
        +  titleC + ToString() + "\n\n";
}

#pragma once

#define MAX_STRING_IDS 65536
#define MAX_STRING_LEN 1024

struct StringIdElement
{
    /* Index of this element in the string id table. This is simply the hash fitted into the size of table */
    uint16_t Id;

    /* Pointer to next element in case of collision */
    StringIdElement* Next;

    /* Internal string representation */
    char DisplayString[MAX_STRING_LEN] = {'\0'};
};

class StringIdTable
{
public:
    StringIdTable();

    void Init();
    void RegisterId(const char* displayStr, uint16_t id);
    void RegisterId(uint16_t id);
    void Add(const char* str, uint16_t& outId, uint16_t& outInstance);

    bool IsValidIndex(uint16_t id) const;
    bool IsValidElement(uint16_t id) const;
    const StringIdElement* Find(uint16_t id, uint16_t instance) const;
    
protected:
    void InitElement(StringIdElement& inOutElement, const char* str, uint16_t id, StringIdElement* next = nullptr);
    StringIdElement Table[MAX_STRING_IDS];
};

/* A hashed representation of a string that allows for rapid comparisons and duplication */
// @TODO: If we want to book-keep the table, can give ids a ref count. When 0, remove from string table.
class StringId
{
public:
    StringId(const std::string& str);
    StringId(const char* str);
    StringId(const StringId& Other = StringId::NONE);

    static StringId NONE;
    static StringId Concat(const StringId& lhs, const StringId& rhs);

    FORCEINLINE uint16_t GetIndex() const { return Id; }
    FORCEINLINE uint16_t GetInstance() const { return Instance; }

    FORCEINLINE bool operator==(const StringId& Other) const;
    FORCEINLINE bool operator!=(const StringId& Other) const;
    FORCEINLINE int Compare(const StringId& Other) const;

    std::string ToString() const;
    const char* ToStringRef() const; // Faster version that returns the reference instead of copying
    std::string DebugDump() const;
protected:
    StringId(uint16_t id, uint16_t instance);

    /* Singleton access */
    static StringIdTable& GetStringIdTable();

    uint16_t Id;
    uint16_t Instance;
private:

};

FORCEINLINE bool StringId::operator==(const StringId& Other) const
{
    return (Id == Other.Id && Instance == Other.Instance);
}

FORCEINLINE bool StringId::operator!=(const StringId& Other) const
{
    return !(Id == Other.Id && Instance == Other.Instance);
}

FORCEINLINE int StringId::Compare(const StringId& Other) const
{
    return (Id - Other.Id) + (Instance - Other.Instance);
}

#include "vctPCH.h"
#include "catch.hpp"

#include "Engine/Core/Types/Path.h"

// Test the static functions too!!!

TEST_CASE("Valid paths test")
{
    REQUIRE(Path("file.ext").IsValid());
    REQUIRE(Path("file").IsValid());
    REQUIRE(Path("folder/file").IsValid());
    REQUIRE(Path("folder/file.ext").IsValid());
}

TEST_CASE("Invalid paths test")
{
    REQUIRE(Path("f$#i%(l)e").IsValid() == false);
    
    // Test maximum length
    char longPath[MAX_PATH_LENGTH+1] = {'a'};
    REQUIRE(Path(longPath).IsValid() == false);
}

TEST_CASE("Get extension test")
{
    std::string ext;

    SECTION("Valid extension")
    {
        Path path("file.ext");
        path.GetExtension(ext, false);
        REQUIRE(ext == "ext");

        path.GetExtension(ext, true);
        REQUIRE(ext == ".ext");
    }

    SECTION("Incorrect file, but valid extension")
    {
        Path path(".file.ext");
        path.GetExtension(ext);

        REQUIRE(ext == ".ext");
    }
}
TEST_CASE("IsFile valid")
{
    Path path = Path("myFile.exe");
    REQUIRE(path.IsFile());
}

TEST_CASE("IsFile invalid")
{
    REQUIRE(Path("folder/.ext").IsFile() == false);
    REQUIRE(Path("folder/.").IsFile() == false);
    REQUIRE(Path(".ext").IsFile() == false);
    REQUIRE(Path("$@!%_.ext").IsFile() == false);
    REQUIRE(Path("fi%$l#e.ext").IsFile() == false);
}

TEST_CASE("IsFolder valid")
{
    REQUIRE(Path("C:/").IsDirectory());
    REQUIRE(Path("C:\\").IsDirectory());
    REQUIRE(Path("folder1/folder2\\").IsDirectory());
}

TEST_CASE("IsFolder invalid")
{
    REQUIRE(Path("folder1/folder2").IsDirectory() == false); // This is a file; all directories end with /
    REQUIRE(Path("folder1/.").IsDirectory() == false);
    REQUIRE(Path("folder1").IsDirectory() == false);
    REQUIRE(Path("f%$#!_()/").IsDirectory() == false); // Invalid tokens
}
#include <Renderer/SharedDeviceTypes.h>
#include <Core/FlagMask.h>

#include <doctest.h>

TEST_CASE("AccessFlags")
{
    REQUIRE(Core::FlagMaskTraits<Renderer::AccessMask>::value == true);
    REQUIRE(Core::FlagMaskTraits<Renderer::AccessMask>::Size() != 0);
}

TEST_CASE("AccessFlags")
{
    REQUIRE(Core::FlagMaskTraits<Renderer::StageMask>::value == true);
    REQUIRE(Core::FlagMaskTraits<Renderer::StageMask>::Size() != 0);
}

TEST_CASE("ImageUsage")
{
    REQUIRE(Core::FlagMaskTraits<Renderer::ImageUsage>::value == true);
    REQUIRE(Core::FlagMaskTraits<Renderer::ImageUsage>::Size() != 0);
}

#include <algorithm>
#include <filesystem>

#include "base-utils/generateRandom.h"
#include "base-utils/tmpFile.h"
#include "config/pathManipulation.h"
#include "unittest/catch.h"

using std::reverse;
using std::string;

using execHelper::test::baseUtils::generateRandomString;
using execHelper::test::baseUtils::TmpFile;

namespace filesystem = std::filesystem;

namespace execHelper::config::test {
SCENARIO("Test listing the parent paths", "[config][path-manipulation]") {
    GIVEN("A list of parent paths ordered from the root directory to the "
          "longest path") {
        const uint8_t NB_OF_SUBDIRECTORIES = 8U;

        Paths pathsUnderTest;
        pathsUnderTest.reserve(NB_OF_SUBDIRECTORIES);
        pathsUnderTest.emplace_back(
            Path("/")); // Make sure to make an absolute path
        for(uint8_t i = 1U; i < NB_OF_SUBDIRECTORIES; ++i) {
            Path additionalPath =
                pathsUnderTest.back() / generateRandomString(i);
            pathsUnderTest.emplace_back(additionalPath);
        }

        WHEN("We request the parent paths for the longest generated path") {
            auto result = getAllParentDirectories(pathsUnderTest.back());

            THEN("We should find the paths under test in reverse order") {
                Paths actualPaths = pathsUnderTest;
                reverse(actualPaths.begin(), actualPaths.end());

                REQUIRE(result == actualPaths);
            }
        }
    }

    GIVEN("The root directory") {
        Path rootPath("/");

        WHEN("We request the parent paths") {
            auto result = getAllParentDirectories(rootPath);

            THEN("We should find the rootPath once") {
                const Paths actualPaths({rootPath});

                REQUIRE(result == actualPaths);
            }
        }
    }

    GIVEN("A relative path") {
        constexpr uint8_t stringLength = 6U;
        Path relativePath(generateRandomString(stringLength));

        WHEN("We request the parent paths") {
            auto result = getAllParentDirectories(relativePath);

            THEN("We should find all the parent directories of the current "
                 "path and the relative path on top of the current path") {
                Paths actualPaths =
                    getAllParentDirectories(filesystem::current_path());
                actualPaths.insert(actualPaths.begin(),
                                   filesystem::absolute(relativePath));

                REQUIRE(result == actualPaths);
            }
        }
    }

    GIVEN("A path to a file") {
        TmpFile file;
        file.create();

        WHEN("We request the parent paths") {
            auto result = getAllParentDirectories(file.getPath());

            THEN("We should find only the parent directories") {
                const Paths actualPaths =
                    getAllParentDirectories(file.getParentDirectory());

                REQUIRE(result == actualPaths);
            }
        }
    }
}

SCENARIO("Test getting the home directory", "[config][path-manipulation]") {
    GIVEN("An environment where the home directory is set") {
        const std::string HOME_KEY("HOME");

        EnvironmentCollection env;
        constexpr uint8_t stringLength = 6U;
        env.emplace(HOME_KEY, generateRandomString(stringLength));

        WHEN("We request the home directory") {
            auto result = getHomeDirectory(env);

            THEN("The result should not be none") {
                REQUIRE(result != std::nullopt);
            }

            THEN("The result should be equal to the chosen home directory") {
                REQUIRE(result.value() == env.at(HOME_KEY));
            }
        }
    }

    GIVEN("An environment where the home directory is not set") {
        EnvironmentCollection env;

        WHEN("We request the home directory") {
            auto result = getHomeDirectory(env);

            THEN("The result should be none") {
                REQUIRE(result == std::nullopt);
            }
        }
    }
}
} // namespace execHelper::config::test

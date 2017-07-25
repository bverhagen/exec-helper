#include <string>
#include <utility>
#include <vector>

#include "unittest/catch.h"
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "config/environment.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "plugins/pluginUtils.h"

#include "utils/utils.h"

using std::make_pair;
using std::string;

using execHelper::config::ENVIRONMENT_KEY;
using execHelper::config::PatternCombinations;
using execHelper::config::VariablesMap;
using execHelper::core::TaskCollection;
using execHelper::config::EnvironmentCollection;
using execHelper::config::EnvironmentValue;

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Test the patterns key", "[plugin-utils]") {
        GIVEN("The correct patterns key") {
            string correctPatternsKey("patterns");

            WHEN("We get the patterns key") {
                const string& actualPatternsKey = getPatternsKey();

                THEN("They should match") {
                    REQUIRE(actualPatternsKey == correctPatternsKey); 
                }
            }
        }
    }

    SCENARIO("Test getting the environment", "[plugin-utils]") {
        GIVEN("Some configured environment variables") {
            const EnvironmentValue ENVIRONMENT_VALUE1 = make_pair("ENVIRONMENT_KEY1", "ENVIRONMENT_VALUE1");
            const EnvironmentValue ENVIRONMENT_VALUE2 = make_pair("ENVIRONMENT_KEY2", "ENVIRONMENT_VALUE2");
            const EnvironmentCollection ENVIRONMENT_VALUES({ENVIRONMENT_VALUE1, ENVIRONMENT_VALUE2});

            VariablesMap variables("test");
            for(const auto& env : ENVIRONMENT_VALUES) {
                variables.add({ENVIRONMENT_KEY, env.first}, env.second);
            }

            WHEN("We get the environment") {
                const EnvironmentCollection result = getEnvironment(variables);

                THEN("We should find the right environment") {
                    REQUIRE(ENVIRONMENT_VALUES == result);
                }
            }
        }
    }

    SCENARIO("Test the working directory key", "[plugin-utils]") {
        GIVEN("The correct working directory key") {
            string correctWorkingDirKey("working-dir");

            WHEN("We get the working directory key") {
                const string& actualWorkingDirKey = getWorkingDirKey();

                THEN("They should match") {
                    REQUIRE(actualWorkingDirKey == correctWorkingDirKey);
                }
            }
        }
    }
} // namespace test
} // namespace plugins
} // namespace execHelper

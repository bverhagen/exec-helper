#ifndef __PLUGIN_UTILS_H__
#define __PLUGIN_UTILS_H__

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>

#include "config/commandLineOptions.h"
#include "config/path.h"
#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/mapPermutator.h"
#include "core/task.h"

namespace execHelper {
namespace core {
class TargetDescriptionElement;
class CompilerDescriptionElement;
} // namespace core
namespace plugins {
class Plugin;
} // namespace plugins
} // namespace execHelper

namespace execHelper {
namespace plugins {
using PatternPermutator =
    core::MapPermutator<config::PatternKey, config::PatternValue>;

const config::PatternKey& getPatternsKey() noexcept;
const std::string& getWorkingDirKey() noexcept;

config::EnvironmentCollection
getEnvironment(const config::VariablesMap& variables) noexcept;

PatternPermutator
makePatternPermutator(const config::Patterns& patterns) noexcept;
core::Task replacePatternCombinations(
    const core::Task& task,
    const config::PatternCombinations& patternCombinations) noexcept;
std::string toString(const config::PatternKeys& values) noexcept;
} // namespace plugins
} // namespace execHelper

#endif /* __PLUGIN_UTILS_H__ */

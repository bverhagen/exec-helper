#include "scons.h"

#include <string>
#include <map>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/patternsHandler.h"

#include "pluginUtils.h"
#include "allTarget.h"

using std::string;
using std::map;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Pattern;
using execHelper::core::PatternKeys;
using execHelper::core::PatternsHandler;

namespace {
    const string SCONS_COMMAND("scons");
    static const string allTargetsKey("all-target");
}

namespace execHelper { namespace plugins {
    bool Scons::apply(const Command& command, Task& task, const Options& options) const noexcept {
        const SettingsNode& rootSettings = options.getSettings({"scons"});

        task.append(SCONS_COMMAND);

        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, getPatternsKey()); 
        PatternKeys patterns; 
        if(patternSettings.contains(getPatternsKey())) {
            patterns = patternSettings[getPatternsKey()].toStringCollection();
        }
        for(const auto& combination : options.makePatternPermutator(patterns)) {
            Task newTask = task;

            if(getMultiThreaded(command, rootSettings, options)) {
                newTask.append(TaskCollection({"--jobs", "8"}));
            }
            newTask.append(getCommandLine(command, rootSettings, combination));
            registerTask(newTask, options);
        }
        return true;
    }
} }

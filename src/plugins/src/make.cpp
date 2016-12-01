#include "make.h"

#include <string>

#include "config/settingsNode.h"
#include "core/task.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::ExecHelperOptions;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescription;
using execHelper::core::Patterns;
using execHelper::config::SettingsNode;

namespace {
    const string MAKE_COMMAND("make");
}

namespace execHelper { namespace plugins {
    bool Make::apply(const std::string& command, Task& task, const ExecHelperOptions& options) const noexcept {
        if(command == "build") {
            return build(task, options);
        } else if(command == "clean") {
            return clean(task, options);
        }
        return false;
    }

    TaskCollection Make::getBuildDir(const SettingsNode& settings, const CompilerDescription& compiler) noexcept {
        TaskCollection commandArguments = settings["build-dir"].toStringCollection();
        Patterns patterns = settings["patterns"].toStringCollection();
        if(commandArguments.size() == 1U) {
            return TaskCollection({replacePatterns(commandArguments[0], patterns, compiler)});
        }
        return TaskCollection({});
    }

    TaskCollection Make::getCommandLine(const SettingsNode& settings, const CompilerDescription& compiler) noexcept {
        TaskCollection commandArguments = settings["command-line"].toStringCollection();
        Patterns patterns = settings["patterns"].toStringCollection();
        if(commandArguments.size() == 1U) {
            return TaskCollection({replacePatterns(commandArguments[0], patterns, compiler)});
        }
        return TaskCollection({});
    }

    TaskCollection Make::getMultiThreaded(const SettingsNode& settings) noexcept {
        TaskCollection commandArguments = settings["single-threaded"].toStringCollection();
        if(commandArguments[0] != "no") {
            return TaskCollection({"-j8"});
        }
        return TaskCollection();
    }

    bool Make::build(core::Task& task, const core::ExecHelperOptions& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"make"});  
        task.append(MAKE_COMMAND);
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(settings));
                newTask.append(getCommandLine(settings, compiler));
                TaskCollection buildTarget = getBuildDir(settings, compiler);
                string targetName = target.getTargets()[0];
                if(targetName != "all") {
                    buildTarget.back() += "/" + targetName;
                }
                newTask.append("-C");
                newTask.append(buildTarget);
                registerTask(newTask, options);
            }
        }
        return true;
    }

    bool Make::clean(core::Task& task, const core::ExecHelperOptions& options) const noexcept {
        task.append(MAKE_COMMAND);
        task.append("clean");
        const SettingsNode& settings = options.getSettings({"make"});  
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(settings));
                newTask.append(getCommandLine(settings, compiler));
                TaskCollection buildTarget = getBuildDir(settings, compiler);
                string targetName = target.getTargets()[0];
                if(targetName != "all") {
                    buildTarget.back() += "/" + targetName;
                }
                newTask.append("-C");
                newTask.append(buildTarget);

                registerTask(newTask, options);
            }
        }
        return true;
    }
} }
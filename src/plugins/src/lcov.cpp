#include "lcov.h"

#include <string>

#include <gsl/string_span>

#include "config/pattern.h"
#include "config/variablesMap.h"
#include "core/task.h"
#include "log/assertions.h"

#include "executePlugin.h"
#include "logger.h"
#include "pluginUtils.h"

using std::string;

using gsl::czstring;

using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::PatternCombinations;
using execHelper::config::Patterns;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::plugins::registerTask;
using execHelper::plugins::replacePatternCombinations;

namespace {
const czstring<> PLUGIN_NAME = "lcov";
using RunCommand = CommandCollection;
const czstring<> RUN_COMMAND = "run-command";
const czstring<> INFO_FILE_KEY = "info-file";
const czstring<> BASE_DIR_KEY = "base-directory";
const czstring<> DIR_KEY = "directory";
using ZeroCounters = bool;
const czstring<> ZERO_COUNTERS_KEY = "zero-counters";
using GenHtml = bool;
const czstring<> GEN_HTML_KEY = "gen-html";
using GenHtmlOutput = Path;
const czstring<> GEN_HTML_OUTPUT_KEY = "gen-html-output";
using GenHtmlTitle = string;
const czstring<> GEN_HTML_TITLE_KEY = "gen-html-title";
using GenHtmlCommandLine = execHelper::plugins::CommandLineArgs;
const czstring<> GEN_HTML_COMMAND_LINE_KEY = "gen-html-command-line";
const czstring<> EXCLUDES_KEY = "excludes";

[[nodiscard]] auto runTask(const Task& task,
                           const PatternCombinations& combination) -> bool {
    Task replacedTask = replacePatternCombinations(task, combination);
    return registerTask(replacedTask);
}
} // namespace

namespace execHelper::plugins {
auto Lcov::getVariablesMap(const FleetingOptionsInterface& /*fleetingOptions*/)
    const noexcept -> VariablesMap {
    VariablesMap defaults(PLUGIN_NAME);
    if(!defaults.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(error) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
    if(!defaults.add(INFO_FILE_KEY, "lcov-plugin.info")) {
        LOG(error) << "Failed to add key '" << INFO_FILE_KEY << "'";
    }
    if(!defaults.add(BASE_DIR_KEY, ".")) {
        LOG(error) << "Failed to add key '" << BASE_DIR_KEY << "'";
    }
    if(!defaults.add(DIR_KEY, ".")) {
        LOG(error) << "Failed to add key '" << DIR_KEY << "'";
    }
    if(!defaults.add(ZERO_COUNTERS_KEY, "no")) {
        LOG(error) << "Failed to add key '" << ZERO_COUNTERS_KEY << "'";
    }
    if(!defaults.add(GEN_HTML_KEY, "no")) {
        LOG(error) << "Failed to add key '" << GEN_HTML_KEY << "'";
    }
    if(!defaults.add(GEN_HTML_OUTPUT_KEY, ".")) {
        LOG(error) << "Failed to add key '" << GEN_HTML_OUTPUT_KEY << "'";
    }
    if(!defaults.add(GEN_HTML_TITLE_KEY, "Hello")) {
        LOG(error) << "Failed to add key '" << GEN_HTML_TITLE_KEY << "'";
    }
    if(!defaults.add(GEN_HTML_COMMAND_LINE_KEY, GenHtmlCommandLine())) {
        LOG(error) << "Failed to add key '" << GEN_HTML_COMMAND_LINE_KEY << "'";
    }
    if(!defaults.add(EXCLUDES_KEY, Excludes())) {
        LOG(error) << "Failed to add key '" << EXCLUDES_KEY << "'";
    }
    return defaults;
}

auto Lcov::apply(Task task, const VariablesMap& variables,
                 const Patterns& patterns) const noexcept -> bool {
    auto runCommandOpt = variables.get<RunCommand>(RUN_COMMAND);
    if(runCommandOpt == std::nullopt) {
        user_feedback_error("Could not find the '"
                            << RUN_COMMAND << "' setting in the '"
                            << PLUGIN_NAME << "' settings");
        return false;
    }
    auto runCommands = *runCommandOpt;

    if(runCommands.empty()) {
        user_feedback_error("The '" << RUN_COMMAND << "' list is empty");
        return false;
    }

    ensures(variables.get<InfoFile>(INFO_FILE_KEY) != std::nullopt);
    auto infoFile = *(variables.get<InfoFile>(INFO_FILE_KEY));

    ensures(variables.get<BaseDir>(BASE_DIR_KEY) != std::nullopt);
    auto baseDirectory = *(variables.get<BaseDir>(BASE_DIR_KEY));

    ensures(variables.get<Dir>(DIR_KEY) != std::nullopt);
    auto directory = *(variables.get<Dir>(DIR_KEY));

    auto commandLine = *(variables.get<CommandLineArgs>(COMMAND_LINE_KEY));

    bool zeroCounters = *(variables.get<ZeroCounters>(ZERO_COUNTERS_KEY));
    Task zeroCountersTask;
    if(zeroCounters) {
        zeroCountersTask = generateZeroCountersTask(baseDirectory, directory,
                                                    commandLine, task);
    }

    bool genHtml = *(variables.get<GenHtml>(GEN_HTML_KEY));
    Task genHtmlTask;
    if(genHtml) {
        genHtmlTask = generateGenHtmlTask(infoFile, variables, task);
    }

    Task captureTask = generateCaptureTask(baseDirectory, directory, infoFile,
                                           commandLine, task);

    auto exclude = *(variables.get<Excludes>(EXCLUDES_KEY));
    Task excludeTask;
    if(!exclude.empty()) {
        excludeTask =
            generateExcludeTask(variables, infoFile, commandLine, task);
    }

    for(const auto& combination : makePatternPermutator(patterns)) {
        if(zeroCounters) {
            if(!runTask(zeroCountersTask, combination)) {
                return false;
            }
        }

        ExecutePlugin execute(runCommands);
        if(!execute.apply(task, variables, patterns)) {
            return false;
        }

        if(!runTask(captureTask, combination)) {
            return false;
        }
        if(!exclude.empty()) {
            if(!runTask(excludeTask, combination)) {
                return false;
            }
        }
        if(genHtml) {
            if(!runTask(genHtmlTask, combination)) {
                return false;
            }
        }
    }
    return true;
}

auto Lcov::summary() const noexcept -> std::string { return "Lcov (internal)"; }

inline auto Lcov::generateGenHtmlTask(const InfoFile& infoFile,
                                      const VariablesMap& variables,
                                      const Task& task) noexcept -> Task {
    Task result = task;
    result.append("genhtml");

    ensures(variables.get<GenHtmlOutput>(GEN_HTML_OUTPUT_KEY) != std::nullopt);
    result.append(
        {"--output-directory",
         variables.get<GenHtmlOutput>(GEN_HTML_OUTPUT_KEY)->string()});

    ensures(variables.get<GenHtmlTitle>(GEN_HTML_TITLE_KEY) != std::nullopt);
    result.append(
        {"--title", *(variables.get<GenHtmlTitle>(GEN_HTML_TITLE_KEY))});

    ensures(variables.get<GenHtmlCommandLine>(GEN_HTML_COMMAND_LINE_KEY) !=
            std::nullopt);
    result.append(
        *(variables.get<GenHtmlCommandLine>(GEN_HTML_COMMAND_LINE_KEY)));

    result.append(infoFile.string());

    return result;
}

inline auto Lcov::generateZeroCountersTask(const BaseDir& baseDirectory,
                                           const Dir& directory,
                                           const CommandLineArgs& commandLine,
                                           const Task& task) noexcept -> Task {
    Task result = task;
    result.append(PLUGIN_NAME);
    result.append({string("--").append(BASE_DIR_KEY), baseDirectory.string()});
    result.append({string("--").append(DIR_KEY), directory.string()});
    result.append("--zerocounters");
    result.append(commandLine);
    return result;
}

inline auto Lcov::getExcludes(const VariablesMap& variables) noexcept
    -> Lcov::Excludes {
    auto excludes = variables.get<Excludes>(EXCLUDES_KEY);
    if(!excludes) {
        return Excludes();
    }

    for(auto& exclude : *excludes) {
        exclude.insert(0, R"(")");
        exclude.append(R"(")");
    }
    return *excludes;
}

inline auto Lcov::generateExcludeTask(const VariablesMap& variables,
                                      const InfoFile& infoFile,
                                      const CommandLineArgs& commandLine,
                                      const Task& task) noexcept -> Task {
    Task result = task;
    auto excludes = getExcludes(variables);
    if(excludes.empty()) {
        return result;
    }
    result.append(PLUGIN_NAME);
    result.append({"--remove", infoFile.string()});
    result.append(excludes);
    result.append({"--output-file", infoFile.string()});
    result.append(commandLine);
    return result;
}

inline auto Lcov::generateCaptureTask(const BaseDir& baseDirectory,
                                      const Dir& directory,
                                      const InfoFile& infoFile,
                                      const CommandLineArgs& commandLine,
                                      const Task& task) noexcept -> Task {
    Task result = task;
    result.append(PLUGIN_NAME);
    result.append({string("--").append(BASE_DIR_KEY), baseDirectory.string()});
    result.append({string("--").append(DIR_KEY), directory.string()});
    result.append("--capture");
    result.append({"--output", infoFile.string()});
    result.append(commandLine);
    return result;
}
} // namespace execHelper::plugins

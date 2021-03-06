#ifndef COMMAND_LINE_OPTIONS_INCLUDE
#define COMMAND_LINE_OPTIONS_INCLUDE

#include <string>
#include <string_view>
#include <vector>

namespace execHelper::config {
const std::string HELP_OPTION_KEY{"help"};
using HelpOption_t = bool;

const std::string VERSION_KEY{"version"};
using VersionOption_t = bool;

const std::string VERBOSE_KEY{"verbose"};
using VerboseOption_t = bool;

const std::string DRY_RUN_KEY{"dry-run"};
using DryRunOption_t = bool;

const std::string KEEP_GOING_KEY{"keep-going"};
using KeepGoingOption_t = bool;

const std::string JOBS_KEY{"jobs"};
using JobsOption_t =
    std::string; // Must be string, since the 'auto' keyword is also supported
using Jobs_t = uint32_t;

const std::string SETTINGS_FILE_KEY{"settings-file"};
using SettingsFileOption_t = std::string;

const std::string COMMAND_KEY{"command"};
using Command = std::string;
using CommandCollection = std::vector<Command>;

const std::string LIST_PLUGINS_KEY{"list-plugins"};
using ListPluginsOption_t = bool;

const std::string APPEND_SEARCH_PATH_KEY{"additional-search-path"};
using AppendSearchPathOption_t = std::vector<std::string>;

const std::string LOG_LEVEL_KEY("debug");
using LogLevelOption_t = std::string;

const std::string_view AUTO_COMPLETE_KEY{"auto-complete"};
using AutoCompleteOption_t = std::string;
} // namespace execHelper::config

#endif /* COMMAND_LINE_OPTIONS_INCLUDE */

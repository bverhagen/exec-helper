#include "fleetingOptions.h"

#include <thread>

#include <boost/lexical_cast.hpp>

#include "logger.h"

using std::thread;

using boost::lexical_cast;

using execHelper::log::LogLevel;

namespace execHelper {
    namespace config {
        FleetingOptions::FleetingOptions(const VariablesMap& optionsMap) :
            m_help(optionsMap.get<HelpOption_t>(HELP_KEY).get()),
            m_verbose(optionsMap.get<VerboseOption_t>(VERBOSE_KEY).get()),
            m_dryRun(optionsMap.get<DryRunOption_t>(DRY_RUN_KEY).get()),
            m_jobs(1U),
            m_logLevel(optionsMap.get<LogLevelOption_t>(LOG_LEVEL_KEY).get()),
            m_commands(optionsMap.get<CommandCollection>(COMMAND_KEY).get())
        {
            auto jobs = optionsMap.get<JobsOption_t>(JOBS_KEY).get();
            if(jobs == "auto") {
                m_jobs = thread::hardware_concurrency();
            } else {
                m_jobs = lexical_cast<Jobs_t>(jobs);
            }
        }

        bool FleetingOptions::operator==(const FleetingOptions& other) {
            return
                m_help == other.m_help &&
                m_verbose == other.m_verbose &&
                m_dryRun == other.m_dryRun &&
                m_jobs == other.m_jobs &&
                m_logLevel == other.m_logLevel &&
                m_commands == other.m_commands;
        }

        bool FleetingOptions::operator!=(const FleetingOptions& other) {
            return !(*this == other);
        }

        HelpOption_t FleetingOptions::getHelp() const noexcept {
            return m_help;
        }

        VerboseOption_t FleetingOptions::getVerbosity() const noexcept {
            return m_verbose;
        }

        DryRunOption_t FleetingOptions::getDryRun() const noexcept {
            return m_dryRun;
        }

        Jobs_t FleetingOptions::getJobs() const noexcept {
            return m_jobs;
        }

        const CommandCollection& FleetingOptions::getCommands() const noexcept {
            return m_commands;
        }

        LogLevel FleetingOptions::getLogLevel() const noexcept {
            return log::toLogLevel(m_logLevel);
        }

        VariablesMap FleetingOptions::getDefault() noexcept {
           VariablesMap defaults("exec-helper"); 
           defaults.add(HELP_KEY, "no");
           defaults.add(VERBOSE_KEY, "no");
           defaults.add(DRY_RUN_KEY, "no");
           defaults.add(JOBS_KEY, "auto");
           defaults.add(SETTINGS_FILE_KEY);
           defaults.add(LOG_LEVEL_KEY, "none");
           defaults.add(COMMAND_KEY);
           return defaults;
        }
    } // namespace config
} // namespace execHelper

#include "execHelperOptions.h"

#include <iostream>
#include <string>
#include <assert.h>

#include "log/log.h"
#include "yaml/yaml.h"
#include "config/settingsNode.h"
#include "compiler.h"
#include "mode.h"
#include "executorInterface.h"
#include "pattern.h"
#include "patternsHandler.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

using boost::program_options::variables_map;

using execHelper::core::CommandCollection;
using execHelper::core::TargetDescription;
using execHelper::core::CompilerDescription;
using execHelper::core::AnalyzeDescription;

using execHelper::config::SettingsNode;

using execHelper::yaml::Yaml;
using execHelper::yaml::YamlFile;

namespace execHelper { namespace core {
    ExecHelperOptions::ExecHelperOptions() noexcept :
        m_help(false),
        m_verbose(false),
        m_singleThreaded(false),
        m_target(new TargetDescription({"all"}, {"all"})),
        m_compiler(new CompilerDescription({Compiler("gcc")}, {Mode("release")}, {Architecture("x64")}, {Distribution("arch-linux")})),
        m_analyze(new AnalyzeDescription({"cppcheck", "clang-static-analyzer"})),
        m_executor(0)
    {
        ;
    }

    ExecHelperOptions::ExecHelperOptions(const ExecHelperOptions& other) noexcept :
        m_help(other.m_help), 
        m_verbose(other.m_verbose),
        m_singleThreaded(other.m_singleThreaded),
        m_target(new TargetDescription(other.getTarget().getTargets(), other.getTarget().getRunTargets())),
        m_compiler(new CompilerDescription(other.getCompiler().getCompilers(), other.getCompiler().getModes(), other.getCompiler().getArchitectures(), other.getCompiler().getDistributions())),
        m_analyze(new AnalyzeDescription(other.getAnalyzeMethods())),
        m_settings(other.m_settings),
        m_executor(other.m_executor)
    {
        ;
    }

    bool ExecHelperOptions::getVerbosity() const noexcept {
        return m_verbose;
    }

    bool ExecHelperOptions::getSingleThreaded() const noexcept {
        return m_singleThreaded;
    }

    const CommandCollection& ExecHelperOptions::getCommands() const noexcept {
        return m_commands;
    }

    const CompilerDescription& ExecHelperOptions::getCompiler() const noexcept {
        assert(m_compiler);
        return *m_compiler;
    }

    const TargetDescription& ExecHelperOptions::getTarget() const noexcept {
        assert(m_target);
        return *m_target;
    }

    const AnalyzeDescription& ExecHelperOptions::getAnalyzeMethods() const noexcept {
        assert(m_analyze);
        return *m_analyze;
    }

    string ExecHelperOptions::getSettingsFile(int argc, const char* const * argv) const noexcept {
        variables_map optionsMap = m_optionsDescriptions.getOptionsMap(argc, argv, true);
        if(optionsMap.count("settings-file")) {
            return optionsMap["settings-file"].as<string>();
        }
        return ".exec-helper";
    }

    bool ExecHelperOptions::parse(int argc, const char* const * argv) {
        m_optionsMap = m_optionsDescriptions.getOptionsMap(argc, argv);
        if(m_optionsMap.count("help")) {
            m_help = true;
            return true;
        }

        if(m_optionsMap.count("verbose")) {
            m_verbose = true;
        }

        if(m_optionsMap.count("single-threaded")) {
            m_singleThreaded = true;
        }

        if(m_optionsMap.count("command")) {
            m_commands.clear();
            m_commands = m_optionsMap["command"].as<CommandCollection>();
        }

        if(m_optionsMap.count("target")) {
            TargetDescription::TargetCollection targets = m_optionsMap["target"].as<TargetDescription::TargetCollection>();
            m_target.reset(new TargetDescription(targets, m_target->getRunTargets()));
        }

        if(m_optionsMap.count("run-target")) {
            TargetDescription::RunTargetCollection runTargets = m_optionsMap["run-target"].as<TargetDescription::RunTargetCollection>();
            m_target.reset(new TargetDescription(m_target->getTargets(), runTargets));
        }

        if(m_optionsMap.count("compiler")) {
            CompilerDescription::CompilerCollection compilers = CompilerDescription::convertToCompilerCollection(m_optionsMap["compiler"].as<CompilerDescription::CompilerNames>());
            m_compiler.reset(new CompilerDescription(compilers, m_compiler->getModes(), m_compiler->getArchitectures(), m_compiler->getDistributions()));
        }

        if(m_optionsMap.count("mode")) {
            CompilerDescription::ModeCollection modes = CompilerDescription::convertToModeCollection(m_optionsMap["mode"].as<CompilerDescription::ModeNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), modes, m_compiler->getArchitectures(), m_compiler->getDistributions()));
        }

        if(m_optionsMap.count("architecture")) {
            CompilerDescription::ArchitectureCollection architectures = CompilerDescription::convertToArchitectureCollection(m_optionsMap["architecture"].as<CompilerDescription::ArchitectureNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), architectures, m_compiler->getDistributions()));
        }

        if(m_optionsMap.count("distribution")) {
            CompilerDescription::DistributionCollection distributions = CompilerDescription::convertToDistributionCollection(m_optionsMap["distribution"].as<CompilerDescription::DistributionNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), m_compiler->getArchitectures(), distributions));
        }

        if(m_optionsMap.count("analyze")) {
            AnalyzeDescription::AnalyzeCollection analyzeMethods = m_optionsMap["analyze"].as<AnalyzeDescription::AnalyzeCollection>();
            m_analyze.reset(new AnalyzeDescription(analyzeMethods));
        }

        return true;
    }

    bool ExecHelperOptions::parseSettingsFile(const std::string& file) noexcept {
        YamlFile yamlFile;
        yamlFile.file = file;
        Yaml yaml(yamlFile);
        if(! yaml.getTree({}, m_settings)) {
            LOG("Could not get settings tree");
            return false;
        }

        static const string patternsKey("patterns");
        if(m_settings.contains(patternsKey)) {
            for(const auto& pattern : m_settings[patternsKey].m_values) {
                PatternKey key = pattern.m_key;
                PatternValues defaultValues;
                for(const auto& defaultValue : pattern["default-values"].toStringCollection()) {
                    defaultValues.push_back(defaultValue);
                }
                static const string shortOptionKey("short-option");
                string shortOptionString;
                if(pattern.contains(shortOptionKey)) {
                    shortOptionString = pattern[shortOptionKey].m_values.back().m_key;
                }
                char shortOption = '\0';
                if(shortOptionString.size() > 0) {
                    shortOption = shortOptionString.at(0);
                }

                static const string longOptionKey("long-option");
                string longOption;
                if(pattern.contains(longOptionKey)) {
                    longOption = pattern[longOptionKey].m_values.back().m_key;
                }

                m_patternsHandler.addPattern(Pattern(key, defaultValues, shortOption, longOption));
                string option = longOption + "," + shortOptionString;
                string explanation = string("Values for pattern '") + key + "'";
                m_optionsDescriptions.addOption<PatternValues>(option, explanation, true);
            }
        }
        return true;
    }

    bool ExecHelperOptions::containsHelp() const noexcept {
        return m_help;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings() const noexcept {
        return m_settings;
    }

    config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) noexcept {
        if(m_settings.contains(key)) {
            return m_settings[key]; 
        }
        return m_settings;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) const noexcept {
        if(m_settings.contains(key)) {
            return m_settings[key]; 
        }
        return m_settings;
    }


    void ExecHelperOptions::setExecutor(ExecutorInterface* const executor) noexcept {
        m_executor = executor;
    }

    ExecutorInterface* ExecHelperOptions::getExecutor() const noexcept {
        assert(m_executor != 0);
        return m_executor;
    }

    void ExecHelperOptions::printHelp() const noexcept {
        user_feedback(m_optionsDescriptions.getOptionDescriptions());
    }

    shared_ptr<Options> ExecHelperOptions::clone() const noexcept {
        return make_shared<ExecHelperOptions>(*this);
    }

    bool ExecHelperOptions::contains(const std::string& longOptions) const noexcept {
        return m_optionsMap.count(longOptions) > 0;
    }

    vector<string> ExecHelperOptions::getLongOption(const std::string& longOptions) const noexcept {
        return m_optionsMap[longOptions].as<vector<string>>();
    }

    const PatternsHandler& ExecHelperOptions::getPatternsHandler() const noexcept {
        return m_patternsHandler;
    }

    PatternValues ExecHelperOptions::getValues(const Pattern& pattern) const noexcept {
        string longOption = pattern.getLongOption();
        if(contains(longOption)) {
            return getLongOption(longOption);
        }
        return PatternValues();
    }

    PatternPermutator ExecHelperOptions::makePatternPermutator(const PatternKeys& patterns) const noexcept {
        std::map<core::PatternKey, core::PatternValues> patternValuesMatrix;
        if(patterns.empty()) {
            // Invent a map so we permutate at least once
            patternValuesMatrix["NOKEY"] = {"NOKEY"};
        } else {
            for(const auto& patternKey : patterns) {
                core::Pattern pattern = m_patternsHandler.getPattern(patternKey);
                PatternValues commandlineValues = getValues(pattern);
                if(commandlineValues.empty()) {
                    patternValuesMatrix.emplace(pattern.getKey(), pattern.getDefaultValues());
                } else {
                    patternValuesMatrix.emplace(pattern.getKey(), commandlineValues);
                }
            }
        }
        return core::PatternPermutator(patternValuesMatrix);
    }
} }

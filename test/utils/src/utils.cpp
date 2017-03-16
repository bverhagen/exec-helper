#include "utils.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "config/settingsNode.h"
#include "core/pattern.h"
#include "core/options.h"
#include "plugins/pluginUtils.h"

using std::map;
using std::vector;
using std::string;
using std::endl;
using std::stringstream;
using std::initializer_list;
using std::reference_wrapper;
using std::ofstream;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::Task;
using execHelper::core::PatternCombinations;
using execHelper::core::Pattern;
using execHelper::core::PatternKey;
using execHelper::core::PatternValue;
using execHelper::core::PatternKeys;
using execHelper::core::PatternValues;
using execHelper::core::PatternPermutator;
using execHelper::plugins::replacePatternCombinations;

using execHelper::core::test::ExecutorStub;

namespace {
    const string YAML_CONFIG_KEY_DELIMITER(": ");
    const string YAML_CONFIG_DELIMITER("\n");
    const string YAML_CONFIG_NESTED_CHARACTER("    ");
    const string YAML_CONFIG_OPTION_CHARACTER(YAML_CONFIG_NESTED_CHARACTER + "- ");

    const string rootPatternKey("patterns");
}

namespace execHelper { namespace test { namespace utils {
   MainVariables::MainVariables(const vector<string>& arguments) {
        argc = static_cast<int>(arguments.size());
        argv.reset(new char*[argc]);

        for(size_t i = 0; i < arguments.size(); ++i) {
            argv.get()[i] = const_cast<char*>(arguments[i].c_str());
        }
    }

    string convertToConfig(const string& key, const vector<string>& values, const string& prepend) {
        string config;
        config += prepend + key + YAML_CONFIG_KEY_DELIMITER + YAML_CONFIG_DELIMITER;
        for(const auto& value : values) {
            config += prepend + YAML_CONFIG_OPTION_CHARACTER + value + YAML_CONFIG_DELIMITER;
        }
        config += YAML_CONFIG_DELIMITER;
        return config;
    }

    string convertToConfig(const SettingsNode& settings, const vector<Pattern>& patterns, const string& prepend) noexcept {
        stringstream stream;   
        const string nestedCharacter(prepend);
        if(patterns.size() > 0) { 
            stream << rootPatternKey << YAML_CONFIG_KEY_DELIMITER << YAML_CONFIG_DELIMITER;
            for(auto& pattern : patterns) {
                string patternNestedCharacter = nestedCharacter + YAML_CONFIG_NESTED_CHARACTER;
                stream << patternNestedCharacter << pattern.getKey() << YAML_CONFIG_KEY_DELIMITER << YAML_CONFIG_DELIMITER;
                
                patternNestedCharacter += YAML_CONFIG_NESTED_CHARACTER;
                stream << convertToConfig("short-option", string(1U, pattern.getShortOption()), patternNestedCharacter);
                stream << convertToConfig("long-option", pattern.getLongOption(), patternNestedCharacter);
                stream << convertToConfig("default-values", pattern.getDefaultValues(), patternNestedCharacter);
            }
        }
        if(settings.m_values.size() == 0 && !settings.m_key.empty()) {
            stream << prepend << YAML_CONFIG_OPTION_CHARACTER << settings.m_key;
        } else {
            string valueNestedCharacter = nestedCharacter;
            if(!settings.m_key.empty()) {
                stream << nestedCharacter << settings.m_key << YAML_CONFIG_KEY_DELIMITER << YAML_CONFIG_DELIMITER;
                valueNestedCharacter += YAML_CONFIG_NESTED_CHARACTER;
            }
            for(const auto& value : settings.m_values) {
                stream << convertToConfig(value, {}, valueNestedCharacter) << YAML_CONFIG_DELIMITER;
            }
            stream << YAML_CONFIG_DELIMITER;
        }
        return stream.str();
    }

    string convertToConfig(string key, string value, const string& prepend) {
        return prepend + key + YAML_CONFIG_KEY_DELIMITER + value + YAML_CONFIG_DELIMITER + YAML_CONFIG_DELIMITER;
    }

    string convertToConfig(const string& key, const std::initializer_list<string>& values, const string& prepend) {
        std::vector<string> vectorValues(values);
        return convertToConfig(key, vectorValues, prepend);
    }

    string basename(const string& file) {
        unsigned int found = file.find_last_of("/\\");
        return file.substr(0,found);
    }

    void addSettings(config::SettingsNode& settings, const std::string& value) noexcept {
        addSettings(settings, value, {});
    }

    void addSettings(SettingsNode& settings, const string& key, const string& value) noexcept {
        addSettings(settings, key, {value});
    }

    void addSettings(SettingsNode& settings, const string& key, const std::initializer_list<string>& values) noexcept {
        addSettings(settings, key, vector<string>(values));
    }

    void addSettings(SettingsNode& settings, const string& key, const std::vector<string>& values) noexcept {
        SettingsNode& settingsToWriteTo = getSetting(settings, key);
        for(const auto& value : values) {
            SettingsNode valueSetting;
            valueSetting.m_key = value;
            settingsToWriteTo.m_values.emplace_back(valueSetting);
        }
    }

    void writeSettingsFile(const string& filename, const SettingsNode& settings, const vector<Pattern>& patterns) noexcept {
        ofstream file;
        file.open(filename, std::ios::out | std::ios::trunc);
        file << convertToConfig(settings, patterns);
        file.close();
    }

    SettingsNode& getSetting(SettingsNode& settings, const string& key) noexcept {
        if(settings.contains(key)) {
            return settings[key];
        }

        settings.m_values.emplace_back(SettingsNode());
        settings.m_values.back().m_key = key;
        return settings.m_values.back();
    }

    PatternCombinations createPatternCombination(const initializer_list<PatternKey>& keys, const initializer_list<PatternValue>& values) noexcept {
        return createPatternCombination(PatternKeys(keys), values);
    }

    PatternCombinations createPatternCombination(const PatternKeys& keys, const PatternValues& values) noexcept {
        PatternCombinations combination;
        if(keys.size() != values.size()) {
            return combination;
        }

        for(size_t i = 0; i < keys.size(); ++i) {
            combination[keys[i]] = values[i];
        }
        return combination;
    }

    TargetUtil::TargetUtil() :
        target("TARGET", {"target1", "target2"}, 't', "target"),
        runTarget("RUNTARGET", {"runTarget1", "runTarget2"}, 'r', "run-target")
    {
        ;
    }

    vector<string> TargetUtil::getKeys() const noexcept {
        return {target.getKey(), runTarget.getKey()};
    }

    vector<Pattern> TargetUtil::getPatterns() const noexcept {
        return {target, runTarget};
    }

    PatternPermutator TargetUtil::makePatternPermutator() const noexcept {
        return makePatternPermutator({target, runTarget});
    }

    PatternPermutator TargetUtil::makePatternPermutator(const vector<Pattern>& patterns) const noexcept {
        map<PatternKey, PatternValues> patternValuesMatrix;
        for(const auto& pattern : patterns) {
            patternValuesMatrix.emplace(make_pair(pattern.getKey(), pattern.getDefaultValues()));
        }
        return patternValuesMatrix;
    }

    TargetUtilNames TargetUtil::toNames(const std::map<PatternKey, PatternValue>& pattern) const noexcept {
        TargetUtilNames names;
        names.target = pattern.at(target.getKey());
        names.runTarget = pattern.at(runTarget.getKey());
        return names;
    }

    CompilerUtil::CompilerUtil() :
        compiler("COMPILER", {"compiler1", "compiler2"}, 'c', "compiler"),
        mode("MODE", {"mode1", "mode2"}, 'm', "mode"),
        architecture("ARCHITECTURE", {"architecture1"}, 'a', "architecture"),
        distribution("DISTRIBUTION", {"distribution1", "distribution2", "distribution3", "distribution4"}, 'd', "distribution")
    {
        ;
    }

    vector<string> CompilerUtil::getKeys() const noexcept {
        return {compiler.getKey(), mode.getKey(), architecture.getKey(), distribution.getKey()};
    }

    vector<Pattern> CompilerUtil::getPatterns() const noexcept {
        return {compiler, mode, architecture, distribution};
    }

    PatternPermutator CompilerUtil::makePatternPermutator() const noexcept {
        return makePatternPermutator({compiler, mode, architecture, distribution});
    }

    PatternPermutator CompilerUtil::makePatternPermutator(const vector<Pattern>& patterns) const noexcept {
        map<PatternKey, PatternValues> patternValuesMatrix;
        for(const auto& pattern : patterns) {
            patternValuesMatrix.emplace(make_pair(pattern.getKey(), pattern.getDefaultValues()));
        }
        return patternValuesMatrix;
    }

    CompilerUtilNames CompilerUtil::toNames(const std::map<PatternKey, PatternValue>& pattern) const noexcept {
        CompilerUtilNames names;
        names.compiler = pattern.at(compiler.getKey());
        names.mode = pattern.at(mode.getKey());
        names.architecture = pattern.at(architecture.getKey());
        names.distribution = pattern.at(distribution.getKey());
        return names;
    }

    PatternKeys getAllPatternKeys(const std::initializer_list<reference_wrapper<const PatternUtil>>& patterns) noexcept {
        PatternKeys patternKeys;
        for(const auto& patternUtil : patterns) {
            for(const auto& pattern : patternUtil.get().getPatterns()) {
                patternKeys.push_back(pattern.getKey());
            }
        }
        return patternKeys;
    }

    Patterns getAllPatterns(const std::initializer_list<reference_wrapper<const PatternUtil>>& patterns) noexcept {
        Patterns allPatterns;
        for(const auto& patternUtil : patterns) {
            for(const auto& pattern : patternUtil.get().getPatterns()) {
                allPatterns.push_back(pattern);
            }
        }
        return allPatterns;
    }

    void addPatterns(const Patterns& patterns, OptionsStub& options) noexcept {
        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }

    ExecutorStub::TaskQueue getExpectedTasks(const Task& expectedTask, const TargetUtil& targetUtil) noexcept {
        ExecutorStub::TaskQueue expectedTasks;
        for(const auto& target : targetUtil.makePatternPermutator()) {
            Task replacedExpectedTask = replacePatternCombinations(expectedTask, target);
            expectedTasks.emplace_back(replacedExpectedTask);
        }
        return expectedTasks;
    }

    ExecutorStub::TaskQueue getExpectedTasks(const Task& expectedTask, const CompilerUtil& compilerUtil, const TargetUtil& targetUtil) noexcept {
        ExecutorStub::TaskQueue expectedTasks;
        for(const auto& compiler : compilerUtil.makePatternPermutator()) {
            for(const auto& target : targetUtil.makePatternPermutator()) {
                Task replacedExpectedTask = replacePatternCombinations(expectedTask, compiler);
                replacedExpectedTask = replacePatternCombinations(replacedExpectedTask, target);
                expectedTasks.emplace_back(replacedExpectedTask);
            }
        }
        return expectedTasks;
    }

    ExecutorStub::TaskQueue getExpectedTasks(const core::test::ExecutorStub::TaskQueue& expectedTask, const CompilerUtil& compilerUtil, const TargetUtil& targetUtil) noexcept {
        ExecutorStub::TaskQueue expectedTasks;
        for(const auto& compiler : compilerUtil.makePatternPermutator()) {
            for(const auto& target : targetUtil.makePatternPermutator()) {
                for(auto task : expectedTask) {
                    task = replacePatternCombinations(task, compiler);
                    task = replacePatternCombinations(task, target);
                    expectedTasks.emplace_back(task);
                }
            }
        }
        return expectedTasks;
    }

    string toString(const SettingsNode& settings, unsigned int nbOfTabs) noexcept {
        string prefix; 
        for(unsigned int i = 0; i < nbOfTabs; ++i) {
            prefix += "  ";
        }

        string result;
        result += prefix + "- " + settings.m_key;
        if(settings.m_values.empty()) {
            result += "\n";
           return result;
        } else {
           result += ":\n";
        }
        for(const auto& value : settings.m_values) {
            result += toString(value, nbOfTabs + 1);
        }
        return result;
    }
} } }

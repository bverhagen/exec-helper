#include "executePlugin.h"

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/options.h"
#include "core/task.h"

#include "plugins/plugin.h"
#include "plugins/commandLineCommand.h"
#include "plugins/scons.h"
#include "plugins/make.h"
#include "plugins/bootstrap.h"
#include "plugins/cppcheck.h"
#include "plugins/clangStaticAnalyzer.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::Options;

namespace execHelper { namespace plugins {
    ExecutePlugin::ExecutePlugin(const vector<string>& commands) noexcept :
        m_commands(commands)
    {
        ;
    }

    bool ExecutePlugin::apply(const Command& command, Task& task, const Options& options) const noexcept {
        for(const auto& pluginName : m_commands) {
            Task newTask = task;
            shared_ptr<Plugin> plugin = getPlugin(pluginName);
            Command commandToPass = command;
            if(!plugin) {
                // Check if it exists as an other target in the settings
                if(options.getSettings(pluginName) != options.getSettings()) {
                    // Then use executeplugin as the plugin 
                    plugin = make_shared<ExecutePlugin>(options.getSettings(pluginName).toStringCollection());
                    commandToPass = pluginName;
                } else {
                    user_feedback("Could not find a command or plugin called " << pluginName);
                    return false;
                }
            }
            if(!plugin || !plugin->apply(commandToPass, newTask, options)) {
                return false;
            }
        }
        return true;
    }

    shared_ptr<Plugin> ExecutePlugin::getPlugin(const string& pluginName) noexcept {
        if(pluginName == "command-line-command") {
            return make_shared<CommandLineCommand>();
        } else if(pluginName == "scons") {
            return make_shared<Scons>(); 
        } else if(pluginName == "make") {
            return make_shared<Make>(); 
        } else if(pluginName == "bootstrap") {
            return make_shared<Bootstrap>(); 
        } else if(pluginName == "cppcheck") {
            return make_shared<Cppcheck>();
        } else if(pluginName == "clang-static-analyzer") {
            return make_shared<ClangStaticAnalyzer>();
        }
        return shared_ptr<Plugin>();
    }
} }

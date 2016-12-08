#ifndef __PLUGIN_UTILS_H__
#define __PLUGIN_UTILS_H__

#include <string>

#include "core/options.h"
#include "core/task.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
    namespace core {
        class CompilerDescriptionElement;
    }
}

namespace execHelper {
    namespace plugins {
        const std::string& getPatternsKey() noexcept;
        const config::SettingsNode& getContainingSettings(const std::string& command, const config::SettingsNode& rootSettings, const std::string& key) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& settings, const core::CompilerDescriptionElement& compiler) noexcept;
    }
}

#endif  /* __PLUGIN_UTILS_H__ */
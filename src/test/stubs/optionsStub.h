#ifndef __OPTIONS_STUB_H__
#define __OPTIONS_STUB_H__

#include <assert.h>

#include "core/options.h"
#include "core/targetDescription.h"
#include "core/compilerDescription.h"
#include "config/settingsNode.h"

namespace execHelper {
    namespace test {
        class OptionsStub : public core::Options {
            public:
                OptionsStub() :
                    Options(),
                    m_verbosity(false),
                    m_targets({}, {}),
                    m_compilers(core::CompilerDescription::CompilerCollection({}), {}, {}, {})
                {
                    ;
                }

                virtual bool parse(int /*argc*/, char** /*argv*/) override {
                    assert(false);  //  This function is currently not supported
                }

                virtual bool parseSettingsFile(const std::string& /*file*/) noexcept override {
                    assert(false);  //  This function is currently not supported
                }

                virtual bool getVerbosity() const noexcept override {
                    return m_verbosity;
                }

                virtual const core::CommandCollection& getCommands() const noexcept override {
                    return m_commands;
                }

                virtual const core::TargetDescription& getTarget() const noexcept override {
                    return m_targets;
                }

                virtual const core::CompilerDescription& getCompiler() const noexcept override {
                    return m_compilers;
                }

                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept override {
                    return m_settings[key];
                }

                virtual bool containsHelp() const noexcept override {
                    return m_containsHelp;
                }

                virtual void setExecutor(core::ExecutorInterface* const executor) noexcept override {
                    m_executor = executor;
                }

                virtual core::ExecutorInterface* getExecutor() const noexcept override {
                    return m_executor;
                }

                bool m_verbosity;
                core::CommandCollection m_commands;
                core::TargetDescription m_targets;
                core::CompilerDescription m_compilers;
                config::SettingsNode m_settings;
                bool m_containsHelp;
                core::ExecutorInterface* m_executor;
        };
    }
}

#endif  /* __OPTIONS_STUB_H__ */
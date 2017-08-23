#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "core/options.h"

namespace execHelper {
    namespace core {
        class Task;
        class Options;
    } // namespace core
} // namespace execHelper

namespace execHelper {
    namespace plugins {
        /**
         * \brief Interface declaration to which plugins should comply
         */
        class Plugin {
            public: 
                Plugin(const Plugin& other) = delete;
                Plugin(Plugin&& other) noexcept = delete;
                virtual ~Plugin() = default;

                Plugin& operator=(const Plugin& other) = delete;
                Plugin& operator=(Plugin&& other) noexcept = delete;

                /**
                 * Apply the plugin
                 *
                 * \param command   The command that is being executed
                 * \param task      The task to extend
                 * \param options   The options that were passed
                 * \returns True    If the application was successful
                 *          False   Otherwise
                 */
                virtual bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept = 0;

            protected:
                Plugin() = default;
        };

        /**
         * Register a finished task to an executor
         *
         * \param task  The finished task
         * \param options   The options that were passed
         * \returns True    If the task was successfully registered
         *          False   Otherwise
         */
        bool registerTask(const core::Task& task, const core::Options& options) noexcept;
    } // namespace plugins
} // namespace execHelper

#endif /* __PLUGIN_H__ */

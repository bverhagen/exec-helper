#ifndef __CONFIG_INPUT_FILE_H__
#define __CONFIG_INPUT_FILE_H__

#include <string>

#include "config/settingsNode.h"

namespace execHelper {
    namespace config {
        /**
         * Interface for configuration files
         */
        class ConfigInputFile {
            public:
                /**
                 * Returns the settings under the node defined by keys
                 *
                 * \param keys  The path to the root node to get the tree from
                 * \param settings  The settings node to fill
                 * \return  True    If the settings node was successfully filled
                 *          False   Otherwise
                 */
                virtual bool getTree(const std::initializer_list<std::string>& keys, SettingsNode& settings) const noexcept = 0;

            protected:
                ConfigInputFile() {}
        };
    }
}

#endif  /* __CONFIG_INPUT_FILE_H__ */

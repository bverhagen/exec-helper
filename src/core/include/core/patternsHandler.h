#ifndef __PATTERNS_HANDLER_H__
#define __PATTERNS_HANDLER_H__

#include <map>

#include "pattern.h"

namespace execHelper {
    namespace core {
        class PatternsHandler {
            private:
                typedef std::map<PatternKey, Pattern> PatternCollection;

            public:
                bool contains(const PatternKey& key) const noexcept;
                void addPattern(const Pattern& pattern) noexcept;
                const Pattern& getPattern(const PatternKey& key) const noexcept;

            private:
                PatternCollection m_patterns;
        };
    }
}

#endif  /* __PATTERNS_HANDLER_H__ */

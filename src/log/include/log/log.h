#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include <ostream>
#include <string>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/expressions.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

BOOST_LOG_ATTRIBUTE_KEYWORD(fileLog, "File", std::string)   // NOLINT(modernize-use-using)
BOOST_LOG_ATTRIBUTE_KEYWORD(lineLog, "Line", unsigned int)  // NOLINT(modernize-use-using)

namespace execHelper {
    namespace log {
        /**
         * \brief   List of the potential log levels
         */
        enum LogLevel
        {
            all,        // Use this to enable all logging
            trace,
            debug,
            info,
            warning,
            error,
            fatal,
            none        // Use this to disable logging
        };

        using Channel = std::string;
        using LoggerType = boost::log::sources::severity_channel_logger_mt<LogLevel, std::string>;

        /**
         * Initialize logging
         *
         * \param   The output stream to log to
         */
        void init(std::ostream& logStream = std::clog) noexcept;

        /**
         * Set the minimum severity of each shown log message for a channel to the given severity
         *
         * \param channel   The channel to set the severity from
         * \param severity  The severity to set the minimal severity to
         * \returns True    If the severity was successfully set
         *          False   Otherwise
         */
        bool setSeverity(const Channel& channel, LogLevel severity) noexcept;

        // Throws InvalidLogLevel
        /**
         * Convert the given log level string to a log level
         *
         * \param level The log level string to convert
         * \returns The associated log level
         * \throws  InvalidLogLevel If the given level string is not associated with any log level
         */
        LogLevel toLogLevel(const std::string& level);

        /**
         * Convert the log level to a string
         *
         * \param level The level to convert
         * \returns The string associated with the log level
         */
        std::string toString(LogLevel level) noexcept;

        std::ostream& operator<<(std::ostream& os, LogLevel level);

        /**
         * \brief   Thrown if an unknown log level is used
         */
        struct InvalidLogLevel : public std::exception {};
    } // namespace log

    namespace color {
        enum ColorCode {
            FG_RED      = 31,
            FG_GREEN    = 32,
            FG_BLUE     = 34,
            FG_DEFAULT  = 39,
            BG_RED      = 41,
            BG_GREEN    = 42,
            BG_BLUE     = 44,
            BG_DEFAULT  = 49
        };

        /**
         * \brief Modifies the command line output format
         */
        class Modifier {
            ColorCode code;
        public:
            explicit Modifier(ColorCode cCode) noexcept : code(cCode) {}
            friend std::ostream&
            operator<<(std::ostream& os, const Modifier& mod) {
                return os << "\033[" << mod.code << "m";
            }

        };

        static Modifier def(FG_DEFAULT);
        static Modifier blue(FG_BLUE);
        static Modifier red(FG_RED);
    } // namespace color
} // namespace execHelper

#define user_feedback(x) (std::cout << x << std::endl)      // NOLINT(misc-macro-parentheses)
#define user_feedback_info(x) (std::cout << execHelper::color::blue << x << execHelper::color::def << std::endl);   // NOLINT(misc-macro-parentheses)
#define user_feedback_error(x) (std::cout << execHelper::color::red << x << execHelper::color::def << std::endl);   // NOLINT(misc-macro-parentheses)

#endif  /* __LOG_H__ */

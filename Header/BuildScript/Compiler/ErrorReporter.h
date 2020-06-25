/*
 * ErrorReporter.h
 * - Report and log errors and warnings.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ERRORREPORTER_H
#define BUILDSCRIPT_COMPILER_ERRORREPORTER_H

#include <string>
#include <functional>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    /**
     * @brief Represent severity of error.
     */
    enum class ErrorLevel {
        Error,      //!< Severe error that not be ignored.
        Warning,    //!< Slight error or mistake that can be ignored.
        Info        //!< Additional information for raised error/warning.
    }; // end enum ErrorLevel

    /**
     * @brief Container for error information.
     */
    struct ErrorEntry {
        ErrorLevel Level;       //!< Severity of the error.
        SourcePosition Where;   //!< A position where the error is raised.
        std::string Message;    //!< An error message.

        /**
         * @brief Constructor for ErrorEntry.
         */
        ErrorEntry(ErrorLevel level, const SourcePosition& pos, std::string message)
                : Level(level), Where(pos), Message(std::move(message)) {}
    }; // end struct ErrorEntry

    /**
     * @brief Callback handler that handles notified errors.
     */
    using ErrorSubscriber = std::function<void(const ErrorEntry&)>;

    /**
     * @brief Report and log errors and warnings.
     */
    class ErrorReporter : NonCopyable {
    private:
        std::vector<ErrorEntry> m_entries;
        std::vector<ErrorSubscriber> m_subscribers;

    public:
        /**
         * @brief Add subscriber.
         * @param subscriber a subscriber.
         * @see ErrorSubscriber
         */
        void Subscribe(const ErrorSubscriber& subscriber) {
            m_subscribers.push_back(subscriber);
        }

        /**
         * @brief Notify reported errors to subscribers.
         */
        void Notify() {
            for (auto& entry : m_entries)
                for (auto& subscriber : m_subscribers)
                    subscriber(entry);
        }

        /**
         * @brief Report an error.
         * @param where a position where error was raised.
         * @param id a ReportID.
         * @param args an additional arguments.
         */
        template <typename... Args>
        void Report(const SourcePosition& pos, ReportID id, Args&&... args) {
            auto msg = fmt::format(ReportIDToString(id), std::forward<Args>(args)...);
            m_entries.emplace_back(ReportIDToErrorLevel(id), pos, msg);
        }

        /**
         * @brief Report an error.
         * @param where a position where error was raised.
         * @param id a ReportID.
         */
        void Report(const SourcePosition& pos, ReportID id) {
            m_entries.emplace_back(ReportIDToErrorLevel(id), pos, ReportIDToString(id));
        }

        /**
         * @brief Check there is an error or not.
         * @return true if there is an error, otherwise false.
         */
        bool HasError() const { return GetErrorCount() > 0; }

        /**
         * @brief Check there is a warning or not.
         * @return true if there is a warning, otherwise false.
         */
        bool HasWarning() const { return GetWarningCount() > 0; }

        /**
         * @brief Get a count of raised errors.
         * @return a count of raised errors.
         */
        int GetErrorCount() const {
            int count = 0;

            for (auto& entry : m_entries)
                if (entry.Level == ErrorLevel::Error) count++;

            return count;
        }

        /**
         * @brief Get a count of raised warnings.
         * @return a count of raised warnings.
         */
        int GetWarningCount() const {
            int count = 0;

            for (auto& entry : m_entries)
                if (entry.Level == ErrorLevel::Warning) count++;

            return count;
        }
    }; // end class ErrorReporter
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ERRORREPORTER_H
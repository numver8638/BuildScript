/*
 * ErrorReporter.h
 * - Collect and notify errors during compile.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ERRORREPORTER_H
#define BUILDSCRIPT_COMPILER_ERRORREPORTER_H

#include <functional>
#include <string>
#include <vector>

#include <fmt/format.h>

#include <BuildScript/Config.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    enum class ReportID;    // Defined in <BuildScript/Compiler/ErrorReporter.ReportID.h>

    /**
     * @brief Represent severity of information.
     */
    enum class Severity {
        Fatal,      //<! Severe error that stop compiling immediately.
        Error,      //<! Severe error that cannot compile correctly.
        Warning,    //<! Error that not severe and can be ignored.
        Note,       //<! Informative message.
    }; // end enum Severity

    /**
     * @brief Container for error information and comments.
     */
    struct ErrorInfo {
        /**
         * @brief Represents kind of the comment.
         */
        enum CommentKind {
            Reference,  //!< Reference in other position of the source.
            Insert,     //!< Insertion of the text.
            Remove      //!< Removal of the text.
        }; // end enum CommentKind

        /**
         * @brief Represents additional information for raised error.
         */
        struct Comment {
            CommentKind Kind;       //!< a kind of the comment.
            SourceRange Range;      //!< a range of the source text that the comment refers.
            std::string Message;    //!< the message.

            /**
             * @warning Intended for internal usage only. DO NOT USE DIRECTLY.
             */
            Comment(CommentKind kind, SourceRange range)
                : Kind(kind), Range(range) {}

            /**
             * @warning Intended for internal usage only. DO NOT USE DIRECTLY.
             */
            Comment(CommentKind kind, SourcePosition pos, std::string text)
                : Kind(kind), Range(pos), Message(std::move(text)) {}
        }; // end struct Comment

        Severity Severity;              //!< a severity of the error.
        SourcePosition Location;        //!< a position where the error caused.
        std::string Message;            //!< the message.
        std::vector<Comment> Comments;  //!< comments.

        /**
         * @warning Intended for internal use only. DO NOT USE DIRECTLY.
         */
        ErrorInfo(enum Severity severity, const SourcePosition& pos, std::string&& msg)
            : Severity(severity), Location(pos), Message(std::move(msg)) {}
    }; // end struct ErrorInfo

    /**
     * @brief Callback function to be notified from @c ErrorReporter.
     */
    using ErrorSubscriber = std::function<void(const ErrorInfo&)>;

    /**
     * @brief Collect and notify errors during compile.
     */
    class EXPORT_API ErrorReporter : NonCopyable {
    public:
        class [[maybe_unused]] Builder {
            friend class ErrorReporter;

        private:
            ErrorInfo& m_info;

            explicit Builder(ErrorInfo& info)
                : m_info(info) {}

        public:
            /**
             * @brief Add reference comment at error.
             * @param pos a position where comment refers to.
             * @return @c this
             */
            Builder& Reference(SourcePosition pos) {
                m_info.Comments.emplace_back(ErrorInfo::Reference, pos, std::string{});
                return *this;
            }

            /**
             * @brief Add insert comment at error.
             * @param pos a position where the comment inserts to.
             * @param text a text that the comment inserts.
             * @return @c this
             */
            Builder& Insert(SourcePosition pos, std::string text) {
                m_info.Comments.emplace_back(ErrorInfo::Insert, pos, std::move(text));
                return *this;
            }

            /**
             * @brief Add remove comment at error.
             * @param range a range of source text to remove.
             * @return @c this
             */
            Builder& Remove(SourceRange range) {
                m_info.Comments.emplace_back(ErrorInfo::Remove, range);
                return *this;
            }
        }; // end class Builder

    private:
        static Severity GetSeverity(ReportID);
        static const char* GetMessage(ReportID);

        std::vector<ErrorInfo> m_entries;
        std::vector<ErrorSubscriber> m_subscribers;

    public:
        ErrorReporter() = default;

        /**
         * @brief Add callback to reporter.
         * @param subscriber the callback function to be notified.
         * @see BuildScript::ErrorSubscriber
         */
        void Subscribe(ErrorSubscriber subscriber) {
            m_subscribers.push_back(std::move(subscriber));
        }

        /**
         * @brief Notify all reported errors to subscribers.
         */
        void Notify() const {
            for (const auto& entry : m_entries) {
                for (const auto& subscriber : m_subscribers) {
                    subscriber(entry);
                }
            }
        }

        /**
         * @brief Report an error.
         * @param pos the position where error was raised.
         * @param id the @c ReportID to report.
         * @return a @c Builder to add comments. Can be ignored.
         * @warning Intended for internal use only. Do not use it out of compile module.
         */
        Builder Report(const SourcePosition& pos, ReportID id) {
            m_entries.emplace_back(GetSeverity(id), pos, GetMessage(id));

            return Builder(m_entries.back());
        }

        /**
         * @brief Report an error with arguments.
         * @tparam Args type of the argument.
         * @param pos the position where error was raised.
         * @param id the @c ReportID to report.
         * @param args arguments for @c id.
         * @return a @c Builder to add comments. Can be ignored.
         * @warning Intended for internal use only. Do not use it out of compile module.
         */
        template <typename... Args>
        Builder Report(const SourcePosition& pos, ReportID id, Args&& ... args) {
            auto msg = fmt::format(GetMessage(id), std::forward<Args&&>(args)...);
            m_entries.emplace_back(GetSeverity(id), pos, std::move(msg));

            return Builder(m_entries.back());
        }

        /**
         * @brief Check there was an error.
         * @return @c true if there was an error, otherwise @c false.
         */
        bool HasError() const {
            return std::count_if(m_entries.begin(), m_entries.end(),
                                 [](const ErrorInfo& info) { return info.Severity <= Severity::Error; }) > 0;
        }
    }; // end class ErrorReporter
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ERRORREPORTER_H
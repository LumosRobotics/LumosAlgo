#ifndef LUMOS_LOGGING_H_
#define LUMOS_LOGGING_H_

#include <sys/time.h>

#include <array>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace lumos
{
  namespace logging
  {
    namespace internal
    {

      // ANSI Color Constants
      namespace colors
      {
        constexpr const char *RESET = "\033[0m";
        constexpr const char *GREEN = "\033[32m";
        constexpr const char *CYAN = "\033[36m";
        constexpr const char *YELLOW = "\033[33m";
        constexpr const char *BLUE = "\033[34m";
        constexpr const char *RED = "\033[31m";
      } // namespace colors
      enum class MessageSeverity
      {
        LOG,
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        TRACE,
        FATAL,
        ASSERTION,
        EXIT
      };

      // Lookup tables for severity strings and colors
      struct SeverityInfo
      {
        const char *color;
        const char *name;
      };

      static constexpr std::array<SeverityInfo, 9> severity_table = {{{colors::RESET, "LOG"},
                                                                      {colors::GREEN, "INFO"},
                                                                      {colors::CYAN, "DEBUG"},
                                                                      {colors::YELLOW, "WARNING"},
                                                                      {colors::RED, "ERROR"},
                                                                      {colors::BLUE, "TRACE"},
                                                                      {colors::RED, "FATAL"},
                                                                      {colors::RED, "ASSERTION FAILED"},
                                                                      {colors::RED, "EXIT"}}};

      // Helper functions
      inline const char *getSeverityColor(const MessageSeverity msg_severity)
      {
        const auto index = static_cast<size_t>(msg_severity);
        if (index < severity_table.size())
        {
          return severity_table[index].color;
        }
        return "UNKNOWNSEVERITYCOLOR";
      }

      inline const char *getSeverityString(const MessageSeverity msg_severity)
      {
        const auto index = static_cast<size_t>(msg_severity);
        if (index < severity_table.size())
        {
          return severity_table[index].name;
        }
        return "UNKNOWNSEVERITY";
      }

      inline const char *getWhiteColorString() { return colors::RESET; }

      inline std::string decNumberAsHexString(const size_t dec_number)
      {
        std::stringstream str_stream;
        str_stream << std::hex << dec_number;
        return str_stream.str();
      }

      // Thread ID caching
      inline size_t getThreadId()
      {
        thread_local size_t cached_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
        return cached_id;
      }

      // Improved Singleton Configuration
      class LoggingConfig
      {
      private:
        enum class LogFileMode
        {
          NONE,
          DIRECTORY_PATTERN, // Option 1: directory + pattern
          FULL_PATH          // Option 2: full path
        };

      public:
        static LoggingConfig &instance()
        {
          static LoggingConfig config;
          return config;
        }

        // Non-copyable, non-movable
        LoggingConfig(const LoggingConfig &) = delete;
        LoggingConfig &operator=(const LoggingConfig &) = delete;
        LoggingConfig(LoggingConfig &&) = delete;
        LoggingConfig &operator=(LoggingConfig &&) = delete;

        // Lockless atomic operations for frequently read settings
        bool getUseColors() const { return use_colors_.load(std::memory_order_relaxed); }
        void setUseColors(bool value) { use_colors_.store(value, std::memory_order_relaxed); }

        bool getShowFile() const { return show_file_.load(std::memory_order_relaxed); }
        void setShowFile(bool value) { show_file_.store(value, std::memory_order_relaxed); }

        bool getShowFunc() const { return show_func_.load(std::memory_order_relaxed); }
        void setShowFunc(bool value) { show_func_.store(value, std::memory_order_relaxed); }

        bool getShowLineNumber() const { return show_line_number_.load(std::memory_order_relaxed); }
        void setShowLineNumber(bool value) { show_line_number_.store(value, std::memory_order_relaxed); }

        bool getShowThreadId() const { return show_thread_id_.load(std::memory_order_relaxed); }
        void setShowThreadId(bool value) { show_thread_id_.store(value, std::memory_order_relaxed); }

        bool getConfigurableAssertion() const { return configurable_assertion_.load(std::memory_order_relaxed); }
        void setConfigurableAssertion(bool value) { configurable_assertion_.store(value, std::memory_order_relaxed); }

        // Thread-safe operations for output stream
        std::shared_ptr<std::ostream> getOutputStream() const
        {
          std::lock_guard<std::mutex> lock(stream_mutex_);
          return output_stream_;
        }

        void setOutputStream(std::ostream *stream)
        {
          if (!stream)
            stream = &std::cout;
          std::lock_guard<std::mutex> lock(stream_mutex_);
          // Use custom deleter that doesn't delete std::cout/std::cerr
          output_stream_ = std::shared_ptr<std::ostream>(stream, [](std::ostream *) {});
        }

        void setOutputStream(std::shared_ptr<std::ostream> stream)
        {
          if (!stream)
          {
            stream = std::shared_ptr<std::ostream>(&std::cout, [](std::ostream *) {});
          }
          std::lock_guard<std::mutex> lock(stream_mutex_);
          output_stream_ = stream;
        }

        // Reset to defaults (useful for testing)
        void resetToDefaults()
        {
          use_colors_.store(true, std::memory_order_relaxed);
          show_file_.store(true, std::memory_order_relaxed);
          show_func_.store(true, std::memory_order_relaxed);
          show_line_number_.store(true, std::memory_order_relaxed);
          show_thread_id_.store(true, std::memory_order_relaxed);
          configurable_assertion_.store(false, std::memory_order_relaxed);
          log_to_both_console_and_file_.store(true, std::memory_order_relaxed);

          std::lock_guard<std::mutex> lock(stream_mutex_);
          output_stream_ = std::shared_ptr<std::ostream>(&std::cout, [](std::ostream *) {});
          log_file_mode_ = LogFileMode::NONE;
          log_file_stream_.reset();
        }

        // Optimized method to read all settings atomically for getPreString
        struct Settings
        {
          bool use_colors;
          bool show_file;
          bool show_func;
          bool show_line_number;
          bool show_thread_id;
          std::shared_ptr<std::ostream> output_stream;
        };

        Settings getAllSettings() const
        {
          Settings settings;
          settings.use_colors = use_colors_.load(std::memory_order_relaxed);
          settings.show_file = show_file_.load(std::memory_order_relaxed);
          settings.show_func = show_func_.load(std::memory_order_relaxed);
          settings.show_line_number = show_line_number_.load(std::memory_order_relaxed);
          settings.show_thread_id = show_thread_id_.load(std::memory_order_relaxed);

          std::lock_guard<std::mutex> lock(stream_mutex_);
          settings.output_stream = output_stream_;

          return settings;
        }

        // File logging setup methods
        // Option 1: Directory + naming pattern
        void setupLogFile(const std::string &log_directory,
                          const std::string &naming_pattern)
        {
          std::lock_guard<std::mutex> lock(stream_mutex_);
          log_file_mode_ = LogFileMode::DIRECTORY_PATTERN;
          log_directory_ = log_directory;
          naming_pattern_ = naming_pattern;
          setupFileStream();
        }

        // Option 2: Full path (overwrites on each run)
        void setupLogFile(const std::string &log_file_full_path)
        {
          std::lock_guard<std::mutex> lock(stream_mutex_);
          log_file_mode_ = LogFileMode::FULL_PATH;
          log_file_path_ = log_file_full_path;
          setupFileStream();
        }

        // Disable file logging
        void disableFileLogging()
        {
          std::lock_guard<std::mutex> lock(stream_mutex_);
          log_file_mode_ = LogFileMode::NONE;
          log_file_stream_.reset();
        }

        // Control whether to log to both console and file, or file only
        void setLogToBothConsoleAndFile(bool both)
        {
          log_to_both_console_and_file_.store(both, std::memory_order_relaxed);
        }

        bool getLogToBothConsoleAndFile() const
        {
          return log_to_both_console_and_file_.load(std::memory_order_relaxed);
        }

        // Get current output streams (may return multiple streams)
        std::vector<std::shared_ptr<std::ostream>> getOutputStreams() const
        {
          std::lock_guard<std::mutex> lock(stream_mutex_);
          std::vector<std::shared_ptr<std::ostream>> streams;

          if (log_to_both_console_and_file_.load(std::memory_order_relaxed) || log_file_mode_ == LogFileMode::NONE)
          {
            streams.push_back(output_stream_); // Console stream
          }

          if (log_file_stream_)
          {
            streams.push_back(log_file_stream_); // File stream
          }

          return streams;
        }

      private:
        LoggingConfig()
            : use_colors_(true), show_file_(true), show_func_(true), show_line_number_(true), show_thread_id_(true), configurable_assertion_(false), log_file_mode_(LogFileMode::NONE), log_to_both_console_and_file_(true), output_stream_(&std::cout, [](std::ostream *) {})
        {
        }

        // Atomic settings for lockless reads
        std::atomic<bool> use_colors_;
        std::atomic<bool> show_file_;
        std::atomic<bool> show_func_;
        std::atomic<bool> show_line_number_;
        std::atomic<bool> show_thread_id_;
        std::atomic<bool> configurable_assertion_;

        // File logging members
        LogFileMode log_file_mode_;
        std::string log_directory_;
        std::string naming_pattern_;
        std::string log_file_path_;
        std::shared_ptr<std::ofstream> log_file_stream_;
        std::atomic<bool> log_to_both_console_and_file_;

        // Helper methods for file logging
        std::string generateLogFileName(const std::string &directory,
                                        const std::string &pattern) const
        {
          std::time_t now = std::time(nullptr);
          std::tm *local_time = std::localtime(&now);

          // Check if pattern contains timestamp specifiers
          if (pattern.find('%') != std::string::npos)
          {
            // Use strftime for custom timestamp formatting
            char buffer[256];
            std::strftime(buffer, sizeof(buffer), pattern.c_str(), local_time);
            std::string result = directory + "/" + std::string(buffer);

            // Ensure .log extension
            if (result.length() < 4 || result.substr(result.length() - 4) != ".log")
            {
              result += ".log";
            }
            return result;
          }
          else
          {
            // Append standard timestamp to prefix
            char timestamp[64];
            std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", local_time);
            return directory + "/" + pattern + "_" + timestamp + ".log";
          }
        }

        void createDirectoriesForPath(const std::string &file_path) const
        {
          size_t pos = file_path.find_last_of('/');
          if (pos != std::string::npos)
          {
            std::string dir = file_path.substr(0, pos);
// Platform-specific directory creation
#ifdef _WIN32
            std::string cmd = "mkdir \"" + dir + "\" 2>nul";
            std::system(cmd.c_str());
#else
            std::string cmd = "mkdir -p \"" + dir + "\"";
            std::system(cmd.c_str());
#endif
          }
        }

        void setupFileStream()
        {
          if (log_file_mode_ == LogFileMode::NONE)
          {
            log_file_stream_.reset();
            return;
          }

          std::string file_path;
          if (log_file_mode_ == LogFileMode::DIRECTORY_PATTERN)
          {
            file_path = generateLogFileName(log_directory_, naming_pattern_);
          }
          else
          {
            file_path = log_file_path_;
          }

          // Create directories if they don't exist
          createDirectoriesForPath(file_path);

          // Open file stream
          auto file_stream = std::make_shared<std::ofstream>(
              file_path,
              log_file_mode_ == LogFileMode::FULL_PATH ? std::ios::trunc : std::ios::app // Overwrite for full path, append for pattern
          );

          if (!file_stream->is_open())
          {
            throw std::runtime_error("Failed to open log file: " + file_path);
          }

          log_file_stream_ = file_stream;
        }

        // Shared pointer with mutex for thread-safe stream management
        mutable std::mutex stream_mutex_;
        std::shared_ptr<std::ostream> output_stream_;
      };

      // Convenience functions for backward compatibility
      inline bool getUseColors()
      {
        return LoggingConfig::instance().getUseColors();
      }

      inline void setUseColors(const bool new_val)
      {
        LoggingConfig::instance().setUseColors(new_val);
      }

      inline bool getShowFunc()
      {
        return LoggingConfig::instance().getShowFunc();
      }

      inline void setShowFunc(const bool new_val)
      {
        LoggingConfig::instance().setShowFunc(new_val);
      }

      inline bool getShowFile()
      {
        return LoggingConfig::instance().getShowFile();
      }

      inline void setShowFile(const bool new_val)
      {
        LoggingConfig::instance().setShowFile(new_val);
      }

      inline bool getShowLineNumber()
      {
        return LoggingConfig::instance().getShowLineNumber();
      }

      inline void setShowLineNumber(const bool new_val)
      {
        LoggingConfig::instance().setShowLineNumber(new_val);
      }

      inline bool getShowThreadId()
      {
        return LoggingConfig::instance().getShowThreadId();
      }

      inline void setShowThreadId(const bool new_val)
      {
        LoggingConfig::instance().setShowThreadId(new_val);
      }

      inline std::shared_ptr<std::ostream> getOutputStream()
      {
        return LoggingConfig::instance().getOutputStream();
      }

      inline void setOutputStream(std::ostream *stream)
      {
        LoggingConfig::instance().setOutputStream(stream);
      }

      inline void setOutputStream(std::shared_ptr<std::ostream> stream)
      {
        LoggingConfig::instance().setOutputStream(stream);
      }

      inline std::vector<std::shared_ptr<std::ostream>> getOutputStreams()
      {
        return LoggingConfig::instance().getOutputStreams();
      }

      inline bool getConfigurableAssertion()
      {
        return LoggingConfig::instance().getConfigurableAssertion();
      }

      inline void setConfigurableAssertion(const bool new_val)
      {
        LoggingConfig::instance().setConfigurableAssertion(new_val);
      }

      inline std::string getPreString(const MessageSeverity msg_severity,
                                      const char *file_name, const char *func_name,
                                      const int line_number)
      {
        // Read all settings atomically with optimized single call
        const auto settings = LoggingConfig::instance().getAllSettings();

        std::ostringstream oss;
        oss.str("");
        oss.str().reserve(256); // Pre-allocate reasonable buffer size

        // Add color if enabled
        if (settings.use_colors)
        {
          oss << getSeverityColor(msg_severity);
        }

        // Add severity
        oss << "[ " << getSeverityString(msg_severity) << " ]";

        // Add thread ID if enabled
        if (settings.show_thread_id)
        {
          oss << "[ 0x" << std::hex << getThreadId() << std::dec << " ]";
        }

        // Add file if enabled
        if (settings.show_file)
        {
          const char *file_start = strrchr(file_name, '/');
          const char *file_display = file_start ? file_start + 1 : file_name;
          oss << "[ " << file_display << " ]";
        }

        // Add function if enabled
        if (settings.show_func)
        {
          oss << "[ " << func_name << " ]";
        }

        // Add line number if enabled
        if (settings.show_line_number)
        {
          oss << "[ " << line_number << " ]";
        }

        oss << ": ";

        // Add reset color if enabled
        if (settings.use_colors)
        {
          oss << getWhiteColorString();
        }

        return oss.str();
      }

      class Log
      {
      public:
        explicit Log(const MessageSeverity msg_severity, const char *file_name,
                     const char *func_name, const int line_number)
            : pre_string_(
                  getPreString(msg_severity, file_name, func_name, line_number)),
              assertion_condition_(true), should_print_(true) {}

        explicit Log(const MessageSeverity msg_severity, const char *file_name,
                     const char *func_name, const int line_number, const bool cond)
            : pre_string_(
                  getPreString(msg_severity, file_name, func_name, line_number)),
              assertion_condition_(cond), should_print_(false) {}

        explicit Log()
            : pre_string_(""), assertion_condition_(true), should_print_(true) {}

        explicit Log(const bool cond)
            : pre_string_(""), assertion_condition_(true), should_print_(cond) {}

        std::ostringstream &getStream() { return string_stream_; }

        // Format string support
        template <typename... Args>
        void format(const char *fmt, Args &&...args)
        {
          constexpr size_t buffer_size = 1024;
          char buffer[buffer_size];
          int result = std::snprintf(buffer, buffer_size, fmt, std::forward<Args>(args)...);
          if (result > 0)
          {
            if (static_cast<size_t>(result) >= buffer_size)
            {
              // Buffer too small, allocate larger buffer
              std::vector<char> larger_buffer(result + 1);
              std::snprintf(larger_buffer.data(), result + 1, fmt, std::forward<Args>(args)...);
              string_stream_ << larger_buffer.data();
            }
            else
            {
              string_stream_ << buffer;
            }
          }
        }

        ~Log()
        {
          if (!assertion_condition_)
          {
            auto streams = LoggingConfig::instance().getOutputStreams();
            const std::string message = pre_string_ + string_stream_.str();

            for (auto &stream : streams)
            {
              *stream << message << std::endl;
              stream->flush(); // Ensure immediate write for assertions
            }

            if (LoggingConfig::instance().getConfigurableAssertion())
            {
              std::terminate();
            }
            else
            {
              raise(SIGABRT);
            }
          }
          else
          {
            if (should_print_)
            {
              auto streams = LoggingConfig::instance().getOutputStreams();
              const std::string message = pre_string_ + string_stream_.str();

              for (auto &stream : streams)
              {
                *stream << message << std::endl;
              }
            }
          }
        }

      private:
        std::ostringstream string_stream_;
        const std::string pre_string_;
        const bool assertion_condition_;
        const bool should_print_;
      };

    } // namespace internal

    inline void useColors(const bool use_colors)
    {
      lumos::logging::internal::setUseColors(use_colors);
    }

    inline void showFile(const bool show_file)
    {
      lumos::logging::internal::setShowFile(show_file);
    }

    inline void showLineNumber(const bool show_line_number)
    {
      lumos::logging::internal::setShowLineNumber(show_line_number);
    }

    inline void showFunction(const bool show_function)
    {
      lumos::logging::internal::setShowFunc(show_function);
    }

    inline void showThreadId(const bool show_thread_id)
    {
      lumos::logging::internal::setShowThreadId(show_thread_id);
    }

    inline void setOutputStream(std::ostream *stream)
    {
      lumos::logging::internal::setOutputStream(stream);
    }

    inline void setConfigurableAssertion(const bool enable)
    {
      lumos::logging::internal::setConfigurableAssertion(enable);
    }

    inline void resetToDefaults()
    {
      lumos::logging::internal::LoggingConfig::instance().resetToDefaults();
    }

    // File logging setup functions
    // Option 1: Directory + pattern
    inline void setupLogFile(const std::string &log_directory,
                             const std::string &naming_pattern)
    {
      lumos::logging::internal::LoggingConfig::instance().setupLogFile(log_directory, naming_pattern);
    }

    // Option 2: Full path
    inline void setupLogFile(const std::string &log_file_full_path)
    {
      lumos::logging::internal::LoggingConfig::instance().setupLogFile(log_file_full_path);
    }

    inline void disableFileLogging()
    {
      lumos::logging::internal::LoggingConfig::instance().disableFileLogging();
    }

    inline void setLogToBothConsoleAndFile(bool both)
    {
      lumos::logging::internal::LoggingConfig::instance().setLogToBothConsoleAndFile(both);
    }

    inline bool getLogToBothConsoleAndFile()
    {
      return lumos::logging::internal::LoggingConfig::instance().getLogToBothConsoleAndFile();
    }

  } // namespace logging
} // namespace lumos

// Conditional compilation support
#ifndef LUMOS_LOG_LEVEL
#define LUMOS_LOG_LEVEL 0 // 0=all, 1=info+, 2=warning+, 3=error+, 4=fatal+
#endif

#define LUMOS_LOG()                                                       \
  lumos::logging::internal::Log(                                          \
      lumos::logging::internal::MessageSeverity::LOG, __FILE__, __func__, \
      __LINE__)                                                           \
      .getStream()

#define LUMOS_LOG_INFO()                                                   \
  lumos::logging::internal::Log(                                           \
      lumos::logging::internal::MessageSeverity::INFO, __FILE__, __func__, \
      __LINE__)                                                            \
      .getStream()

#if LUMOS_LOG_LEVEL <= 0
#define LUMOS_LOG_DEBUG()                                                   \
  lumos::logging::internal::Log(                                            \
      lumos::logging::internal::MessageSeverity::DEBUG, __FILE__, __func__, \
      __LINE__)                                                             \
      .getStream()
#else
#define LUMOS_LOG_DEBUG() lumos::logging::internal::Log(false).getStream()
#endif

#if LUMOS_LOG_LEVEL <= 2
#define LUMOS_LOG_WARNING()                                                   \
  lumos::logging::internal::Log(                                              \
      lumos::logging::internal::MessageSeverity::WARNING, __FILE__, __func__, \
      __LINE__)                                                               \
      .getStream()
#else
#define LUMOS_LOG_WARNING() lumos::logging::internal::Log(false).getStream()
#endif

#define LUMOS_LOG_TRACE()                                                   \
  lumos::logging::internal::Log(                                            \
      lumos::logging::internal::MessageSeverity::TRACE, __FILE__, __func__, \
      __LINE__)                                                             \
      .getStream()

#if LUMOS_LOG_LEVEL <= 3
#define LUMOS_LOG_ERROR()                                                   \
  lumos::logging::internal::Log(                                            \
      lumos::logging::internal::MessageSeverity::ERROR, __FILE__, __func__, \
      __LINE__)                                                             \
      .getStream()
#else
#define LUMOS_LOG_ERROR() lumos::logging::internal::Log(false).getStream()
#endif

#define LUMOS_LOG_FATAL()                                                   \
  lumos::logging::internal::Log(                                            \
      lumos::logging::internal::MessageSeverity::FATAL, __FILE__, __func__, \
      __LINE__)                                                             \
      .getStream()

// Backward compatibility (can be disabled by defining LUMOS_NO_LEGACY_MACROS)
#ifndef LUMOS_NO_LEGACY_MACROS
#define LOG() LUMOS_LOG()
#define LOG_INFO() LUMOS_LOG_INFO()
#define LOG_DEBUG() LUMOS_LOG_DEBUG()
#define LOG_WARNING() LUMOS_LOG_WARNING()
#define LOG_TRACE() LUMOS_LOG_TRACE()
#define LOG_ERROR() LUMOS_LOG_ERROR()
#define LOG_FATAL() LUMOS_LOG_FATAL()
#endif

#define LUMOS_PRINT() lumos::logging::internal::Log().getStream()

#define LUMOS_PRINT_COND(cond) lumos::logging::internal::Log(cond).getStream()

#define LUMOS_ASSERT(cond)                                            \
  lumos::logging::internal::Log(                                      \
      lumos::logging::internal::MessageSeverity::ASSERTION, __FILE__, \
      __func__, __LINE__, cond)                                       \
      .getStream()

#define LUMOS_EXIT(cond)                                                   \
  lumos::logging::internal::Log(                                           \
      lumos::logging::internal::MessageSeverity::EXIT, __FILE__, __func__, \
      __LINE__, false)                                                     \
      .getStream()

// Format string macros
#define LUMOS_LOGF(fmt, ...)                                                \
  do                                                                        \
  {                                                                         \
    lumos::logging::internal::Log log(                                      \
        lumos::logging::internal::MessageSeverity::LOG, __FILE__, __func__, \
        __LINE__);                                                          \
    log.format(fmt, ##__VA_ARGS__);                                         \
  } while (0)

#define LUMOS_LOG_INFOF(fmt, ...)                                            \
  do                                                                         \
  {                                                                          \
    lumos::logging::internal::Log log(                                       \
        lumos::logging::internal::MessageSeverity::INFO, __FILE__, __func__, \
        __LINE__);                                                           \
    log.format(fmt, ##__VA_ARGS__);                                          \
  } while (0)

#define LUMOS_LOG_ERRORF(fmt, ...)                                            \
  do                                                                          \
  {                                                                           \
    lumos::logging::internal::Log log(                                        \
        lumos::logging::internal::MessageSeverity::ERROR, __FILE__, __func__, \
        __LINE__);                                                            \
    log.format(fmt, ##__VA_ARGS__);                                           \
  } while (0)

// Backward compatibility (can be disabled by defining LUMOS_NO_LEGACY_MACROS)
#ifndef LUMOS_NO_LEGACY_MACROS
#define PRINT() LUMOS_PRINT()
#define PRINT_COND(cond) LUMOS_PRINT_COND(cond)
#define ASSERT(cond) LUMOS_ASSERT(cond)
#define EXIT(cond) LUMOS_EXIT(cond)
#define LOGF(fmt, ...) LUMOS_LOGF(fmt, ##__VA_ARGS__)
#define LOG_INFOF(fmt, ...) LUMOS_LOG_INFOF(fmt, ##__VA_ARGS__)
#define LOG_ERRORF(fmt, ...) LUMOS_LOG_ERRORF(fmt, ##__VA_ARGS__)
#endif

#endif // LUMOS_LOGGING_H_

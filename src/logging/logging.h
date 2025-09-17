#ifndef LUMOS_LOGGING_H_
#define LUMOS_LOGGING_H_

#include <sys/time.h>

#include <array>
#include <chrono>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace lumos {
namespace logging {
namespace internal {

// ANSI Color Constants
namespace colors {
constexpr const char* RESET = "\033[0m";
constexpr const char* GREEN = "\033[32m";
constexpr const char* CYAN = "\033[36m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* BLUE = "\033[34m";
constexpr const char* RED = "\033[31m";
} // namespace colors
enum class MessageSeverity {
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
struct SeverityInfo {
  const char* color;
  const char* name;
};

static constexpr std::array<SeverityInfo, 9> severity_table = {{
  {colors::RESET, "LOG"},
  {colors::GREEN, "INFO"},
  {colors::CYAN, "DEBUG"},
  {colors::YELLOW, "WARNING"},
  {colors::RED, "ERROR"},
  {colors::BLUE, "TRACE"},
  {colors::RED, "FATAL"},
  {colors::RED, "ASSERTION FAILED"},
  {colors::RED, "EXIT"}
}};

// Helper functions
inline const char* getSeverityColor(const MessageSeverity msg_severity) {
  const auto index = static_cast<size_t>(msg_severity);
  if (index < severity_table.size()) {
    return severity_table[index].color;
  }
  return "UNKNOWNSEVERITYCOLOR";
}

inline const char* getSeverityString(const MessageSeverity msg_severity) {
  const auto index = static_cast<size_t>(msg_severity);
  if (index < severity_table.size()) {
    return severity_table[index].name;
  }
  return "UNKNOWNSEVERITY";
}

inline const char* getWhiteColorString() { return colors::RESET; }

inline std::string decNumberAsHexString(const size_t dec_number) {
  std::stringstream str_stream;
  str_stream << std::hex << dec_number;
  return str_stream.str();
}

// Thread ID caching
inline size_t getThreadId() {
  thread_local size_t cached_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
  return cached_id;
}

// Global variables

inline std::mutex &Variable_thread_mutex() {
  static std::mutex mtx;
  return mtx;
}

inline bool &Variable_use_colors() {
  static bool use_colors = true;
  return use_colors;
}

inline bool &Variable_show_file() {
  static bool show_file = true;
  return show_file;
}

inline bool &Variable_show_func() {
  static bool show_func = true;
  return show_func;
}

inline bool &Variable_show_line_number() {
  static bool show_line_number = true;
  return show_line_number;
}

inline bool &Variable_show_thread_id() {
  static bool show_thread_id = true;
  return show_thread_id;
}

inline std::ostream* &Variable_output_stream() {
  static std::ostream* output_stream = &std::cout;
  return output_stream;
}

inline bool &Variable_configurable_assertion() {
  static bool configurable_assertion = false;
  return configurable_assertion;
}

// Set get for global variables

inline bool getUseColors() {
  return Variable_use_colors();
}

inline void setUseColors(const bool new_val) {
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  Variable_use_colors() = new_val;
}

inline bool getShowFunc() {
  return Variable_show_func();
}

inline void setShowFunc(const bool new_val) {
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  Variable_show_func() = new_val;
}

inline bool getShowFile() {
  return Variable_show_file();
}

inline void setShowFile(const bool new_val) {
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  Variable_show_file() = new_val;
}

inline bool getShowLineNumber() {
  return Variable_show_line_number();
}

inline void setShowLineNumber(const bool new_val) {
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  Variable_show_line_number() = new_val;
}

inline bool getShowThreadId() {
  return Variable_show_thread_id();
}

inline void setShowThreadId(const bool new_val) {
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  Variable_show_thread_id() = new_val;
}

inline std::ostream* getOutputStream() {
  return Variable_output_stream();
}

inline void setOutputStream(std::ostream* stream) {
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  Variable_output_stream() = stream;
}

inline bool getConfigurableAssertion() {
  return Variable_configurable_assertion();
}

inline void setConfigurableAssertion(const bool new_val) {
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  Variable_configurable_assertion() = new_val;
}

inline std::string getPreString(const MessageSeverity msg_severity,
                                const char *file_name, const char *func_name,
                                const int line_number) {
  // Read all settings atomically to ensure thread safety
  std::lock_guard<std::mutex> guard(Variable_thread_mutex());
  const bool use_colors = Variable_use_colors();
  const bool show_func = Variable_show_func();
  const bool show_file = Variable_show_file();
  const bool show_line_number = Variable_show_line_number();
  const bool show_thread_id = Variable_show_thread_id();
  
  std::ostringstream oss;
  oss.str("");
  oss.str().reserve(256); // Pre-allocate reasonable buffer size
  
  // Add color if enabled
  if (use_colors) {
    oss << getSeverityColor(msg_severity);
  }
  
  // Add severity
  oss << "[ " << getSeverityString(msg_severity) << " ]";
  
  // Add thread ID if enabled
  if (show_thread_id) {
    oss << "[ 0x" << std::hex << getThreadId() << std::dec << " ]";
  }
  
  // Add file if enabled
  if (show_file) {
    const char* file_start = strrchr(file_name, '/');
    const char* file_display = file_start ? file_start + 1 : file_name;
    oss << "[ " << file_display << " ]";
  }
  
  // Add function if enabled
  if (show_func) {
    oss << "[ " << func_name << " ]";
  }
  
  // Add line number if enabled
  if (show_line_number) {
    oss << "[ " << line_number << " ]";
  }
  
  oss << ": ";
  
  // Add reset color if enabled
  if (use_colors) {
    oss << getWhiteColorString();
  }
  
  return oss.str();
}

class Log {
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
  template<typename... Args>
  void format(const char* fmt, Args&&... args) {
    constexpr size_t buffer_size = 1024;
    char buffer[buffer_size];
    int result = std::snprintf(buffer, buffer_size, fmt, std::forward<Args>(args)...);
    if (result > 0) {
      if (static_cast<size_t>(result) >= buffer_size) {
        // Buffer too small, allocate larger buffer
        std::vector<char> larger_buffer(result + 1);
        std::snprintf(larger_buffer.data(), result + 1, fmt, std::forward<Args>(args)...);
        string_stream_ << larger_buffer.data();
      } else {
        string_stream_ << buffer;
      }
    }
  }

  ~Log() {
    if (!assertion_condition_) {
      auto* output = getOutputStream();
      *output << pre_string_ + string_stream_.str() << std::endl;
      if (getConfigurableAssertion()) {
        std::terminate();
      } else {
        raise(SIGABRT);
      }
    } else {
      if (should_print_) {
        auto* output = getOutputStream();
        *output << pre_string_ + string_stream_.str() << std::endl;
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

inline void useColors(const bool use_colors) {
  lumos::logging::internal::setUseColors(use_colors);
}

inline void showFile(const bool show_file) {
  lumos::logging::internal::setShowFile(show_file);
}

inline void showLineNumber(const bool show_line_number) {
  lumos::logging::internal::setShowLineNumber(show_line_number);
}

inline void showFunction(const bool show_function) {
  lumos::logging::internal::setShowFunc(show_function);
}

inline void showThreadId(const bool show_thread_id) {
  lumos::logging::internal::setShowThreadId(show_thread_id);
}

inline void setOutputStream(std::ostream* stream) {
  lumos::logging::internal::setOutputStream(stream);
}

inline void setConfigurableAssertion(const bool enable) {
  lumos::logging::internal::setConfigurableAssertion(enable);
}

} // namespace logging
} // namespace lumos

// Conditional compilation support
#ifndef LUMOS_LOG_LEVEL
#define LUMOS_LOG_LEVEL 0  // 0=all, 1=info+, 2=warning+, 3=error+, 4=fatal+
#endif

#define LUMOS_LOG()                                                            \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::LOG, __FILE__, __func__,      \
      __LINE__)                                                                \
      .getStream()
      
#define LUMOS_LOG_INFO()                                                       \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::INFO, __FILE__, __func__,     \
      __LINE__)                                                                \
      .getStream()
      
#if LUMOS_LOG_LEVEL <= 0
#define LUMOS_LOG_DEBUG()                                                      \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::DEBUG, __FILE__, __func__,    \
      __LINE__)                                                                \
      .getStream()
#else
#define LUMOS_LOG_DEBUG() lumos::logging::internal::Log(false).getStream()
#endif

#if LUMOS_LOG_LEVEL <= 2
#define LUMOS_LOG_WARNING()                                                    \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::WARNING, __FILE__, __func__,  \
      __LINE__)                                                                \
      .getStream()
#else
#define LUMOS_LOG_WARNING() lumos::logging::internal::Log(false).getStream()
#endif

#define LUMOS_LOG_TRACE()                                                      \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::TRACE, __FILE__, __func__,    \
      __LINE__)                                                                \
      .getStream()
      
#if LUMOS_LOG_LEVEL <= 3
#define LUMOS_LOG_ERROR()                                                      \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::ERROR, __FILE__, __func__,    \
      __LINE__)                                                                \
      .getStream()
#else
#define LUMOS_LOG_ERROR() lumos::logging::internal::Log(false).getStream()
#endif

#define LUMOS_LOG_FATAL()                                                      \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::FATAL, __FILE__, __func__,    \
      __LINE__)                                                                \
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

#define LUMOS_ASSERT(cond)                                                     \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::ASSERTION, __FILE__,          \
      __func__, __LINE__, cond)                                                \
      .getStream()

#define LUMOS_EXIT(cond)                                                       \
  lumos::logging::internal::Log(                                               \
      lumos::logging::internal::MessageSeverity::EXIT, __FILE__, __func__,     \
      __LINE__, false)                                                         \
      .getStream()

// Format string macros
#define LUMOS_LOGF(fmt, ...)                                                   \
  do {                                                                         \
    lumos::logging::internal::Log log(                                  \
        lumos::logging::internal::MessageSeverity::LOG, __FILE__, __func__,    \
        __LINE__);                                                             \
    log.format(fmt, ##__VA_ARGS__);                                           \
  } while(0)

#define LUMOS_LOG_INFOF(fmt, ...)                                              \
  do {                                                                         \
    lumos::logging::internal::Log log(                                  \
        lumos::logging::internal::MessageSeverity::INFO, __FILE__, __func__,   \
        __LINE__);                                                             \
    log.format(fmt, ##__VA_ARGS__);                                           \
  } while(0)

#define LUMOS_LOG_ERRORF(fmt, ...)                                             \
  do {                                                                         \
    lumos::logging::internal::Log log(                                  \
        lumos::logging::internal::MessageSeverity::ERROR, __FILE__, __func__,  \
        __LINE__);                                                             \
    log.format(fmt, ##__VA_ARGS__);                                           \
  } while(0)

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

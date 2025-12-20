// Lightweight logging shim used when glog is not available.
#pragma once

#ifdef HAVE_GLOG
#include <glog/logging.h>
#else
#include <iostream>
#include <sstream>

// Simple severity levels so code can use LOG(ERROR), LOG(FATAL), LOG(DEBUG), etc.
enum { INFO = 0, DEBUG = 0, WARNING = 1, ERROR = 2, FATAL = 3 };

// Default minimum severity to emit (ERROR and above).
static constexpr int MINI_LOG_MIN_SEVERITY = ERROR;

struct _MiniLog {
    int sev;
    std::ostringstream s;
    explicit _MiniLog(int severity = INFO) : sev(severity) {}
    template<typename T>
    _MiniLog& operator<<(T const& v) { s << v; return *this; }
    ~_MiniLog() {
        if (sev >= MINI_LOG_MIN_SEVERITY) {
            std::cerr << s.str() << std::endl;
            if (sev == FATAL) std::abort();
        }
    }
};

// Preserve the LOG(FOO) and VLOG(x) usage from the codebase.
#define VLOG(x) _MiniLog(DEBUG)
#define LOG(x) _MiniLog(x)

#endif

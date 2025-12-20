// Lightweight logging shim used when glog is not available.
#pragma once

#ifdef HAVE_GLOG
#include <glog/logging.h>
#else
#include <iostream>
#include <sstream>

struct _MiniLog {
    std::ostringstream s;
    template<typename T>
    _MiniLog& operator<<(T const& v) { s << v; return *this; }
    ~_MiniLog() { std::cerr << s.str() << std::endl; }
};

#define VLOG(x) _MiniLog()
#define LOG(x) _MiniLog()

#endif

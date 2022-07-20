#pragma once
#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#define LOGS PLOG_DEBUG_(0) 
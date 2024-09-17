/*
 *  mkCommon.h
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#ifndef __mkCommon_h__
#define __mkCommon_h__

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <stdint.h>
#include <cmath>
#if defined(ANDROID)
#include <android/log.h>
#endif

#if defined(ANDROID)
#define MK_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MonkVG", __VA_ARGS__)
#else
#define MK_LOG(...) printf(__VA_ARGS__)
#endif

#if defined(NDEBUG)
#define MK_ASSERT(x) ((void)0)
#else
#define MK_ASSERT(x) assert(x)
#endif

#endif // __mkCommon_h__

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// All static variables go here, to control initialization and
// destruction order in the library.

#include <private/binder/Static.h>

#include <binder/BufferedTextOutput.h>
#include <binder/IPCThreadState.h>
#include <utils/Log.h>

namespace android {

// ------------ Text output streams

Vector<int32_t> gTextBuffers;

class LogTextOutput : public BufferedTextOutput
{
public:
    LogTextOutput() : BufferedTextOutput(MULTITHREADED) { }
    virtual ~LogTextOutput() { };

protected:
    virtual status_t writeLines(const struct iovec& vec, size_t N)
    {
        //android_writevLog(&vec, N);       <-- this is now a no-op
        if (N != 1) ALOGI("WARNING: writeLines N=%zu\n", N);
        ALOGI("%.*s", (int)vec.iov_len, (const char*) vec.iov_base);
        return NO_ERROR;
    }
};

class FdTextOutput : public BufferedTextOutput
{
public:
    explicit FdTextOutput(int fd) : BufferedTextOutput(MULTITHREADED), mFD(fd) { }
    virtual ~FdTextOutput() { };

protected:
    virtual status_t writeLines(const struct iovec& vec, size_t N)
    {
        writev(mFD, &vec, N);
        return NO_ERROR;
    }

private:
    int mFD;
};

static LogTextOutput gLogTextOutput;
static FdTextOutput gStdoutTextOutput(STDOUT_FILENO);
static FdTextOutput gStderrTextOutput(STDERR_FILENO);

TextOutput& alog(gLogTextOutput);
TextOutput& aout(gStdoutTextOutput);
TextOutput& aerr(gStderrTextOutput);

// ------------ IServiceManager.cpp

Mutex gDefaultServiceManagerLock;
sp<IServiceManager> gDefaultServiceManager;
#ifndef __ANDROID_VNDK__
sp<IPermissionController> gPermissionController;
#endif
bool gSystemBootCompleted = false;

}   // namespace android

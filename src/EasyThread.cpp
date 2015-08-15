/*
 * EasyThread.cpp
 * EasyThread
 *
 * Created by Josh Kennedy on 10 August 2015
 * Copyright (C) 2015 Sirkles LLC. All rights reserved.
 */

// DESCRIPTION: Object-Oriented wrapper around POSIX threads.

/*
 EasyThread
 Copyright (c) 2015 Sirkles LLC.

 Authors:
	* Josh Kennedy (Original C++ version)
 
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgement in the product documentation would be
 appreciated but is not required.

 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.

 3. This notice may not be removed or altered from any source distribution.
*/

#include "EasyThread.hpp"

#include <signal.h>
#include <errno.h>

#include <sstream>
#include <chrono>
#include <thread>

EasyThread::~EasyThread()
{
	// Wait for the thread to finish before destructing.
	// We don't want a dangling thread!
	pthread_join(this->threadID, NULL);
}

void EasyThread::setFunction(void*(func)(void*))
{
	this->function = func;
}

void EasyThread::setArgument(void* param)
{
	this->param = param;
}

void* EasyThread::getArgument() const
{
	return this->param;
}

void EasyThread::run()
{
	int val = pthread_create(&this->threadID, NULL, function, param);

	if (val != 0)
	{
		throw new ThreadCreationError(val);
	}

	this->started = true;
}

void EasyThread::terminate()
{
	// TODO: Ensure thread is active.
	pthread_cancel(this->threadID);
}

void EasyThread::wait()
{
	// TODO: Ensure thread is still active.
	pthread_join(this->threadID, nullptr);
}

void EasyThread::setName(std::string name)
{
	this->threadName = name;

	// TODO: Ensure thread is active.

#if _WIN32
	// Windows doesn't have thread names natively, but the VS Debugger does though!
	BOOL remoteDebug = false;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDebug);
	if (IsDebuggerPresent() || (remoteDebug ? true : false)) return;
	
	// I thought pthreads_win32 2.10 was supposed to implement pthread_setname_np, oh well.
	HANDLE threadHandle = pthread_getw32threadhandle_np(this->threadID);

	SetThreadName(GetThreadId(threadHandle), name.c_str());
#else
	pthread_setname_np(this->threadID, name.c_str());
#endif
}

std::string EasyThread::getName() const
{
	return this->threadName;
}

pthread_t EasyThread::getThreadID() const
{
	return this->threadID;
}

bool EasyThread::isActive() const
{
	return (this->started && (pthread_tryjoin_np(this->threadID, nullptr) == EBUSY));
}

bool EasyThread::hasStarted() const
{
	return this->started;
}

void EasyThread::sleep(unsigned long milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

#if _WIN32
void SetThreadName(DWORD dwThreadID, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}
#endif

// Exceptions

ThreadCreationError::ThreadCreationError(int errorCode)
{
	this->errorCode = errorCode;

	std::stringstream messageBuilder;

	messageBuilder << "Unable to create thread! pthread error code: " << this->errorCode;

	this->message = messageBuilder.str().c_str();
}

ThreadCreationError::ThreadCreationError(int errorCode, const char* message)
{
	this->errorCode = errorCode;
	this->message = message;
}

const char* ThreadCreationError::what() const
{
	return this->message;
}

int ThreadCreationError::getErrorCode() const
{
	return this->errorCode;
}

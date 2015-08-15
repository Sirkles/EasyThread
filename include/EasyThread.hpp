/*
 * EasyThread.hpp
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

#pragma once

#ifndef EASY_THREAD_H_
#define EASY_THREAD_H_

// noexcept isn't supported on Visual Studio 2013 and earlier.
#if _MSC_VER >= 1900 || !_WIN32
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

// I <3 Preprocessor! /s
#ifdef EASYTHREAD_STATIC
#define EASYTHREAD_API
#define EASYTHREAD_C_API extern "C"
#else
#if _WIN32
#if LIBEASYTHREAD_EXPORTS
#define EASYTHREAD_API __declspec(dllexport)
#define EASYTHREAD_C_API extern "C" EASYTHREAD_API
#else
#define EASYTHREAD_API __declspec(dllimport)
#define EASYTHREAD_C_API extern "C" EASYTHREAD_API
#endif
#elif defined(__APPLE__) && defined(__MACH__) // Mac OS X
// I don't know. :(
// I'll just assume that the GCC attribute will work 
#elif defined(_GCC) && defined(__linux__) // Linux GCC (maybe clang as well, but not sure.)
// Not sure if it compiles correctly. :/
#if LIBEASYTHREAD_EXPORTS
#define EASYTHREAD_API __attribute__((visibility("default")))
#define EASYTHREAD_C_API extern "C" EASYTHREAD_API
#else
#define EASYTHREAD_API
#define EASYTHREAD_C_API extern "C"
#endif
#else
// Other platforms: DIY! :)
#if LIBEASYTHREAD_EXPORTS
#define EASYTHREAD_API
#define EASYTHREAD_C_API extern "C"
#else
#define EASYTHREAD_API
#define EASYTHREAD_C_API extern "C"
#endif
#endif
#endif

#include <pthread.h>

#include <string>
#include <exception>

class EASYTHREAD_API ThreadCreationError : public std::exception
{
public:
	ThreadCreationError(int errorCode);
	ThreadCreationError(int errorCode, const char* message);

	const char* what() const NOEXCEPT;

	int getErrorCode() const NOEXCEPT;
	
private:
	int errorCode;
	const char* message;
};

class EASYTHREAD_API EasyThread final
{
public:
	/*
	 * Constructs a new EasyThread with no function passed in.
	 * NOTE: A function can be added in by using setFunction().
	 */
	EasyThread()
	{
		this->function = nullptr;
		this->param = nullptr;
		this->started = false;
	}

	/*
	 * Constructs a new EasyThread with a function and argument passed in.
	 */
	EasyThread(void*(func)(void*), void* arg = nullptr)
	{
		this->function = func;
		this->param = arg;
		this->started = false;
	}

	/*
	 * Destroys the thread and sends a SIGTERM signal to this thread.
	 */
	~EasyThread();

	/*
	 * Sets the function that the thread will execute.
	 *
	 * Functions need to be in this format:
	 * void* functionName(void* param)
	 */
	void setFunction(void*(func)(void*));

	//void(*getFunction())(void*);

	void setArgument(void* param);

	void* getArgument() const;

	/*
	 * Begins the execution of this thread.
	 */
	void run();

	/*
	 * Cancels this thread.
	 * NOTE: Please don't use this! Use cross-thread communication (like a bool or something) instead. :-)
	 */
	void terminate();

	/*
	 * Waits for this thread to complete it's execution, and then resumes the parent thread.
	 * run() needs to be called before calling this.
	 */
	void wait();

	/*
	 * Sets the name of the thread.
	 * run() needs to be called before calling this.
	 */
	void setName(std::string name);

	/*
	 * Gets the name of the thread.
	 */
	std::string getName() const;

	/*
	 * Gets the TID of this thread.
	 * This will return 0 if the thread is not currently running.
	 */
	pthread_t getThreadID() const;

	/*
	 * Returns true if the thread is active.
	 */
	bool isActive() const;

	/*
	 * Returns true if the thread has been started.
	 */
	bool hasStarted() const;

	/*
	 * Sleeps the current thread for a specified time (in milliseconds).
	 */
	static void sleep(unsigned long milliseconds);

private:
	pthread_t threadID;

	void* (__cdecl *function)(void*);

	std::string threadName;

	void* param;

	bool started;
};

// Windows helper code.
// Shoutout to Microsoft for the code! https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
// Modded a little bit to allow threadName to be passed in as a const char*
#if _WIN32
#include <Windows.h>

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

EASYTHREAD_C_API void SetThreadName(DWORD dwThreadID, const char* threadName);
#endif

#endif

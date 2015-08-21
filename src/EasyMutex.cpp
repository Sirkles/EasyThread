/*
 * EasyMutex.hpp
 * EasyThread
 *
 * Created by Josh Kennedy on 21 August 2015
 * Copyright (C) 2015 Sirkles LLC. All rights reserved.
 */

// DESCRIPTION: Object-Oriented wrapper around POSIX mutexes.

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

#include "EasyMutex.hpp"

EasyMutex::EasyMutex()
{
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init(&attributes);
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(&this->mutex, &attributes);
}

EasyMutex::~EasyMutex()
{
	pthread_mutex_destroy(&this->mutex);
}

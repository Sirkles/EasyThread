/*
 * Main.cpp
 * Created by Josh Kennedy on 14 August 2015.
 *
 * EasyThread
 * Copyright (C) 2015 Sirkles LLC. All rights reserved.
 */

// DESCRIPTION: Example code for EasyThread.

/*
 * License:
 * This example program and all it's encompassing code are released into the
 * public domain. All copyright associated with this example program are 
 * relinquished. You are free to use this anyway you want, commercial or not.
 *
 * This does not mean that the EasyThread library itself is public domain.
 * It is still licensed under the very permissive zlib license which is
 * included with EasyThread.
 *
 * Enjoy! :-)
 */

#include "EasyThread.hpp"

#include <iostream>

void* testFunction(void* param)
{
	unsigned long milliseconds = (unsigned long)(param);
	std::cout << "Sleeping this thread for " << milliseconds << " milliseconds...\n";

	EasyThread::sleep(milliseconds);

	return nullptr;
}

int main(int argc, char* argv[])
{
	EasyThread* easyThread = new EasyThread();

	easyThread->setFunction(testFunction);
	easyThread->setArgument((void*)3000U);

	std::cout << "Is the thread active? " << (easyThread->isActive() ? "Yes, it is!" : "No, it's not.") << std::endl;

	easyThread->run();

	easyThread->setName("testFunction");

	std::cout << "Is the thread active now? " << (easyThread->isActive() ? "Yes, it is!" : "No, it's not.") << std::endl;

	// Wait until the thread ends.
	while (easyThread->isActive())
	{
		std::cout << "ayy lmao" << std::endl;
		EasyThread::sleep(1000);
	}

	delete easyThread;

	// More examples coming soon.

	return 0;
}

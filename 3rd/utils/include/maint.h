#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <string>

using namespace std;

class maintenance
{
	public:		
	static void assemble();

	/* return whether maint. mode is enbaled
	 * enabled if true; otherwise,false
	 */
	static bool enabled();

	static void log(string &content);

	private:
	static 	void handle_sig(int signo,siginfo_t *si,void *ucontext);

	static void enable();

	static void disable();

	private:
	/* 
	 * enter maintenamce mode if true; otherwise, normal mode.
	 * false by default
	 */
	static 	bool enabled_;

};

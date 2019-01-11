#pragma once

#ifndef TIMER_API
#	ifdef _NOEXPORT
#		define TIMER_API
#	elif _TIMER_EXPORT
#		ifdef _WIN32
#			define TIMER_API __declspec(dllexport)
#		else
#			define TIMER_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define TIMER_API __declspec(dllimport)
#		else
#			define TIMER_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

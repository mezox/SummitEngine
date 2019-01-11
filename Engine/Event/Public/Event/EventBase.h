#pragma once

#ifndef EVENT_API
#	ifdef _NOEXPORT
#		define EVENT_API
#	elif _EVENT_EXPORT
#		ifdef _WIN32
#			define EVENT_API __declspec(dllexport)
#		else
#			define EVENT_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define EVENT_API __declspec(dllimport)
#		else
#			define EVENT_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

#pragma once

#ifndef LOGGING_API
#	ifdef _NOEXPORT
#		define LOGGING_API
#	elif _LOGGING_EXPORT
#		ifdef _WIN32
#			define LOGGING_API __declspec(dllexport)
#		else
#			define LOGGING_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define LOGGING_API __declspec(dllimport)
#		else
#			define LOGGING_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

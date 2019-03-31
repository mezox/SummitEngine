#pragma once

#ifndef CORE_API
#	ifdef _NOEXPORT
#		define CORE_API
#	elif _CORE_EXPORT
#		ifdef _WIN32
#			define CORE_API __declspec(dllexport)
#		else
#			define CORE_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define CORE_API __declspec(dllimport)
#		else
#			define CORE_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

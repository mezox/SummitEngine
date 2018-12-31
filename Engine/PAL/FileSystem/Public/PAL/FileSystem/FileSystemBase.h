#pragma once

#ifndef FILESYSTEM_API
#	ifdef _NOEXPORT
#		define FILESYSTEM_API
#	elif _FILESYSTEM_EXPORT
#		ifdef _WIN32
#			define FILESYSTEM_API __declspec(dllexport)
#		else
#			define FILESYSTEM_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define FILESYSTEM_API __declspec(dllimport)
#		else
#			define FILESYSTEM_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

#pragma once

#ifndef MICROPROFILE_API
#	ifdef _NOEXPORT
#		define MICROPROFILE_API
#	elif _MICROPROFILE_EXPORT
#		ifdef _WIN32
#			define MICROPROFILE_API __declspec(dllexport)
#		else
#			define MICROPROFILE_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define MICROPROFILE_API __declspec(dllimport)
#		else
#			define MICROPROFILE_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

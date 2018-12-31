#pragma once

#ifndef ENGINE_API
#	ifdef _NOEXPORT
#		define ENGINE_API
#	elif _ENGINE_EXPORT
#		ifdef _WIN32
#			define ENGINE_API __declspec(dllexport)
#		else
#			define ENGINE_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define ENGINE_API __declspec(dllimport)
#		else
#			define ENGINE_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

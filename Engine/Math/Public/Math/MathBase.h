#pragma once

#ifndef MATH_API
#	ifdef _NOEXPORT
#		define MATH_API
#	elif _MATH_EXPORT
#		ifdef _WIN32
#			define MATH_API __declspec(dllexport)
#		else
#			define MATH_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define MATH_API __declspec(dllimport)
#		else
#			define MATH_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

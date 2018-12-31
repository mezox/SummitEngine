#pragma once

#ifndef RENDERAPI_API
#	ifdef _NOEXPORT
#		define RENDERAPI_API
#	elif _RENDERAPI_EXPORT
#		ifdef _WIN32
#			define RENDERAPI_API __declspec(dllexport)
#		else
#			define RENDERAPI_API __attribute__((__visibility__("default")))
#		endif
#	else
#		ifdef _WIN32
#			define RENDERAPI_API __declspec(dllimport)
#		else
#			define RENDERAPI_API __attribute__((__visibility__("default")))
#		endif
#	endif
#endif

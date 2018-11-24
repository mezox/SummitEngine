#pragma once

#ifndef LOGGING_API
	#ifdef _NOEXPORT
		#define LOGGING_API
	#elif _LOGGING_EXPORT
		#define LOGGING_API __attribute__((__visibility__("default")))
	#else
		#define LOGGING_API __attribute__((__visibility__("default")))
	#endif
#endif

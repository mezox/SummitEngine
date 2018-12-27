#pragma once

#ifndef RENDERAPI_API
    #ifdef _NOEXPORT
        #define RENDERAPI_API
    #elif _RENDERAPI_EXPORT
        #define RENDERAPI_API __attribute__((__visibility__("default")))
    #else
        #define RENDERAPI_API __attribute__((__visibility__("default")))
    #endif
#endif

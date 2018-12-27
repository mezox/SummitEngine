#pragma once

#ifndef FILESYSTEM_API
    #ifdef _NOEXPORT
        #define FILESYSTEM_API
    #elif _FILESYSTEM_EXPORT
        #define FILESYSTEM_API __attribute__((__visibility__("default")))
    #else
        #define FILESYSTEM_API __attribute__((__visibility__("default")))
    #endif
#endif

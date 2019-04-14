#pragma once

#ifndef	_ASSERT
#    include <assert.h>
#    include <signal.h>
#    include <pthread.h>
#
#    define _ASSERT(exp) (void)((!!(exp)) || (fprintf( stderr, "ASSERT - %s line %d\n", __FILE__, __LINE__ ), pthread_kill(pthread_self(), SIGSTOP)))
#endif

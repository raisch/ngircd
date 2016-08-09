#ifndef __monitor_h__
#define __monitor_h__

#include "portab.h"

#include <bson.h>
#include <mongoc.h>

GLOBAL int Monitor_Init PARAMS(( void ));

GLOBAL int Monitor_Write PARAMS(( bson_t *doc ));

GLOBAL void Monitor_Close PARAMS(( void ));

#endif

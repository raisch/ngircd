#ifndef __monitor_h__
#define __monitor_h__

#include "portab.h"
#include "parse.h"

#include <bson.h>
#include <mongoc.h>

GLOBAL int Monitor_Init PARAMS(( void ));

GLOBAL int Monitor_Write PARAMS(( int conn, char* dir, char* msg ));

GLOBAL int Monitor_WriteRequest PARAMS(( int conn, REQUEST* req ));

GLOBAL void Monitor_Close PARAMS(( void ));

#endif

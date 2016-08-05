#ifndef __monitor_h__
#define __monitor_h__

#include "portab.h"

//#include <stdio.h>

//#include <time.h>

#include <bson.h>
#include <mongoc.h>

#define MAX_LINE_LEN 1024

/** Last Monitor Error */
GLOBAL char MonitorLastError[MAX_LINE_LEN];

/** MongoDb connection uri */
GLOBAL char MonitorConnectionStr[MAX_LINE_LEN] = "mongodb://localhost:27017/";

/** MongoDb database name */
GLOBAL char MonitorDatabaseName[MAX_LINE_LEN] = "ngircd";

/** MongoDb collection name */
GLOBAL char MonitorCollectionName[MAX_LINE_LEN] = "events";

/** MongoDb client handle */
GLOBAL mongoc_client_t *MonitorClient;

/** MongoDb collection handle */
GLOBAL mongoc_collection_t *MonitorCollection;

/** Initialize Monitor */
GLOBAL int Monitor_Init PARAMS(( void ));

GLOBAL int Monitor_Write PARAMS(( bson_t *doc ));

GLOBAL void Monitor_Close PARAMS(( void ));

#endif

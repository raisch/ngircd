
#include "portab.h"

#include <bson.h>
#include <mongoc.h>

#include "monitor.h"

#define clrErrorMsg() MonitorLastError[0]='\0'

#define setErrorMsg(S) strlcpy(MonitorLastError, S, sizeof(MonitorLastError))

/**
 * Create and initialize monitor database connection.
 * Returns -1 on error, with reason in MonitorLastError
 */
GLOBAL int 
Monitor_Init() {

  clrErrorMsg();

  if(!MonitorConnectionStr[0]) {
    setErrorMsg("requires a MonitorConnectionStr");
    return -1;
  }

  if(!MonitorDatabaseName[0]) {
    setErrorMsg("requires a MonitorDatabaseName");
    return -1;
  }

  if(!MonitorCollectionName[0]) {
    setErrorMsg("requires a MonitorCollectionName");
    return -1;
  }

  mongoc_init();

  MonitorClient = mongoc_client_new (MonitorConnectionStr);

  if(NULL==MonitorClient){
    setErrorMsg("failed to create a new mongoc client");
    return -1;
  }

  MonitorCollection = mongoc_client_get_collection (
    MonitorClient, 
    MonitorDatabaseName, 
    MonitorCollectionName
  );

  if(NULL==MonitorClient){
    setErrorMsg("failed to get mongoc collection");
    return -1;
  }

  return 0;
}

/**
 * Close (and cleanup) monitor database connection.
 */
GLOBAL void 
Monitor_Close() {

  if(NULL!=MonitorCollection) {
    mongoc_collection_destroy (MonitorCollection);
  }
  if(NULL!=MonitorClient) {
    mongoc_client_destroy (MonitorClient);
  }

  mongoc_cleanup();
}

/**
 * Write document to monitor database.
 */
GLOBAL int 
Monitor_Write( bson_t *doc ) {

  bson_error_t error;

  clrErrorMsg();

  if(NULL==doc){
    setErrorMsg("requires a bson_t *doc");
    return -1;
  }

  if (!mongoc_collection_insert (MonitorCollection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
    setErrorMsg(error.message);
    fprintf (stderr, "%s\n", error.message);
    return -1;
  }

  return 0;
}



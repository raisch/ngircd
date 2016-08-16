
#include "portab.h"
#include "log.h"

#include <bson.h>
#include <mongoc.h>

#include "parse.h"
#include "monitor.h"

#define MAX_LINE_LEN 1024

/** Last Monitor Error */
GLOBAL char MonitorLastError[MAX_LINE_LEN];

/** MongoDb connection uri */
GLOBAL char MonitorConnectionStr[MAX_LINE_LEN] = "mongodb://127.0.0.1:27017/";

/** MongoDb database name */
GLOBAL char MonitorDatabaseName[MAX_LINE_LEN] = "ngircd";

/** MongoDb collection name */
GLOBAL char MonitorCollectionName[MAX_LINE_LEN] = "events";

/** MongoDb client handle */
GLOBAL mongoc_client_t *MonitorClient;

/** MongoDb collection handle */
GLOBAL mongoc_collection_t *MonitorCollection;

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
Monitor_Write( int conn, char* dir, char* msg ) {

  bson_error_t error;
  bson_oid_t oid;
  bson_t *doc;

  Log(LOG_DEBUG, "%s/%d: writing msg '%s' to db", dir, conn, msg);

  doc = bson_new ();
  bson_oid_init (&oid, NULL);
  BSON_APPEND_OID (doc, "_id", &oid);
  BSON_APPEND_INT32(doc, "conn", conn);
  BSON_APPEND_UTF8(doc, "dir", dir);
  BSON_APPEND_UTF8 (doc, "msg", msg);

  clrErrorMsg();

  if(NULL==doc){
    setErrorMsg("requires a bson_t *doc");
    return -1;
  }

  if (!mongoc_collection_insert (MonitorCollection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
    setErrorMsg(error.message);
    Log(LOG_ERR,"MONITOR ERROR: %s\n", error.message);
    return -1;
  }

  bson_destroy(doc);

  return 0;
}

GLOBAL int
Monitor_WriteRequest( int conn, REQUEST* req) {

  bson_error_t error;
  bson_oid_t oid;
  bson_t *doc;
  int i;
  char str[MAX_LINE_LEN];

  Log(LOG_INFO, "%d: writing request command '%s' to db", conn, req->command);

  doc = bson_new ();
  bson_oid_init (&oid, NULL);
  BSON_APPEND_OID (doc, "_id", &oid);

  BSON_APPEND_INT32(doc, "conn", conn);

  if(req->prefix != NULL && strlen(req->prefix) > 0) {
    BSON_APPEND_UTF8 (doc, "prefix", req->prefix);
  }

  if(req->command != NULL && strlen(req->command) > 0) {
    BSON_APPEND_UTF8(doc, "cmd", req->command);
  }

  for (i = 0; i < req->argc; i++) {
    if (i > 0) strlcat(str, " ", sizeof(str));
    strlcat(str, req->argv[i], sizeof(str));
  }

  if(strlen(str) > 0) {
    BSON_APPEND_UTF8(doc, "args", str);
  }

  clrErrorMsg();

  if(NULL==doc){
    setErrorMsg("requires a bson_t *doc");
    return -1;
  }

  if (!mongoc_collection_insert (MonitorCollection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
    setErrorMsg(error.message);
    Log(LOG_ERR,"MONITOR ERROR: %s\n", error.message);
    return -1;
  }

  bson_destroy(doc);

  return 0;
}



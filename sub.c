#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
#include <mysql.h>

// #define ADDRESS     "ws://broker.emqx.io:8083"
#define ADDRESS     "tcp://broker.emqx.io:1883"
#define CLIENTID    "Sub_data"
#define SUB_TOPIC   "test/pubParameter"

// Connect to database 
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

char *server = "127.0.0.1";
char *user = "tuanpham";
char *password = "016365"; /* set me first */
char *database = "FINAL_EMBEDDED_IOT";


int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload; // data gui sang MQTT la danng string 
    printf("Received message: %s\n", payload);
    char *token =strtok(payload,"/");
    float data[4] ; 
    int index =0;
    while( token != NULL){
        data[index] = atof(token);
        token = strtok(NULL,"/");
        index++;
    }
    // Read data from pi and send to database of PC
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }  
    char sql[200];
    sprintf(sql,"insert into RPM_VEL(RPMLeft,VelLeft,RPMRight,VelRight) values(%d,%.4f,%d,%.4f)",(int)data[0],data[1],(int)data[2],data[3]);
    // printf("%f,%.4f", (float)data[0],data[1]);
    mysql_query(conn,sql);
    mysql_close(conn);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName); 
    return 1;
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    } 
    //listen for operation
    MQTTClient_subscribe(client, SUB_TOPIC, 0);
    while(1) {
    }
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}
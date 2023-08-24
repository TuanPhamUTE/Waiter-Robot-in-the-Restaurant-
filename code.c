#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <mysql.h>
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
#include "MRFC522.H"	


#define S2 11
#define S3 13
#define S4 15
#define S5 18
#define S1 16
#define pwm0FOR 33
#define pwm0REV 32
#define pwm1FOR 12
#define pwm1REV 35

#define encoder1 40
#define encoder2 38


#define ADDRESS     "ws://broker.emqx.io:8083"
#define CLIENTID    "TTH_IOT"
#define PUB_TOPIC   "test/pubParameter"
#define SUB_TOPIC   "test/configMotor"
#define SUB_TOPIC1  "test/userRequest"

int value, V1, V2 ,V3 ,V4 ,V5;
// RFID
unsigned char data1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 	
unsigned char g_ucTempbuf[20];	
int t=0;
///////////////////////////////////////////
// Config 
uint8_t motorValue[1];

// Encoder  1 
int count1 = 0,rpm1 = 0;
float tocdo1 = 0;
int timecho1 = 1000;
unsigned long thoigian1;
unsigned long hientai1;
// Enocder 2 
int count2 = 0,rpm2 = 0;
float tocdo2 = 0;
int timecho2 = 1000;
unsigned long thoigian2;
unsigned long hientai2;

#define encoderLeft 40
#define encoderRight 38
uint8_t temp, tempMoterValue;

void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    // printf("Message '%s' with delivery token %d delivered\n", payload, token);
}
// Sub from Managers
int on_messageManagers(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload; // data gui sang MQTT la dang string 
    printf("Received message: %s\n", payload);
    char *token =strtok(payload,"");
    int index =0;
    while( token != NULL){
        motorValue[index] = atof(token);
        token = strtok(NULL,"");
        index++;
    }
	if ( motorValue[0] == 11) temp =11;
	else if ( motorValue[0] == 22) temp =22;
	else if ( motorValue[0] == 33){
		temp =33;
		motorValue[0] = tempMoterValue;
		t=0;
	} 
    //printf("%d\n",motorValue[0]);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName); 
    return 1;
}
// uint8_t subUser[1];

// // Sub from User1 and User2
// int on_messageUsers(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
//     char* payload = message->payload; // data gui sang MQTT la dang string 
//     printf("RFID1 and RDIF2 Received request from User1 and User2: %s\n", payload);
//     char *token =strtok(payload,"");
//     int index =0;
//     while( token != NULL){
//         subUser[index] = atof(token);
//         token = strtok(NULL,"");
//         index++;
//     }t

//     MQTTClient_freeMessage(&message);
//     MQTTClient_free(topicName); 
//     return 1;
// }

void read_encoderLeft(void){
	count1++;
}
void read_encoderRight(void){
	count2++;
}
void Cal_EncoderLeft(){
	thoigian1 = millis();	
	if((thoigian1 - hientai1) >= timecho1){
		hientai1 = thoigian1;
		rpm1 = (count1/20)*60;
		tocdo1 = (float)(count1/20*(float)(0.025*3.14));	
		count1=0;
		printf("RPMLeft: %d , VelLeft: %.4f \n", rpm1, tocdo1);	
	}}
void Cal_EncoderRight(){
	thoigian2 = millis();	
	if((thoigian2 - hientai2) >= timecho2){
		hientai2 = thoigian2;
		rpm2 = (count2/20)*60;
		tocdo2 = (float)(count2/20*(float)(0.025*3.14));	
		count2=0;
		printf("RPMRight: %d , VelRight: %.4f \n", rpm2, tocdo2);	
	}}

// void public_RPMVEL(){
// 	char msg [25];
// 	sprintf(msg,"%d,%.4f",rpm, tocdo);
// 	publish(client, PUB_TOPIC, msg);
// 	printf("RPM: %d , tocdo: %.4f \n", rpm, tocdo);	
// 	sleep(0.5);
// }

void Tien(uint8_t temp){
	pwmWrite(pwm0FOR, temp);
	pwmWrite(pwm1FOR, temp);
	digitalWrite(pwm0REV, 0);
	digitalWrite(pwm1REV, 0);
}

void Trai(){
	pwmWrite(pwm0FOR, 70);
	pwmWrite(pwm1FOR, 20);
	digitalWrite(pwm0REV, 0);
	digitalWrite(pwm1REV, 0);
}

void Phai(){
	pwmWrite(pwm0FOR, 20);
	pwmWrite(pwm1FOR, 70);
	digitalWrite(pwm0REV, 0);
	digitalWrite(pwm1REV, 0);
}
void Dung(){
	pwmWrite(pwm0FOR, 0);
	pwmWrite(pwm1FOR, 0);
	digitalWrite(pwm0REV, 0);
	digitalWrite(pwm1REV, 0);
}

void robotmove(){
	if(t==0)
	{
		if(digitalRead(S3)==0){
			Tien(tempMoterValue);
		}
		else if(digitalRead(S4)==0 || digitalRead(S5)==0){
			Trai();
		}
		else if(digitalRead(S2)==0 || digitalRead(S1)==0){
			Phai();
		}
	}
	else
	{
		Dung();
	}
}
void robot_RFID(uint8_t a){
	if  (t==a){  // rfid
		// t = 5;
		// robotmove();
		Dung();

	}
	else if( motorValue[0] == 40 || motorValue[0] == 45 || motorValue[0] == 50){
		t =0;
		tempMoterValue = motorValue[0];
		robotmove();
	}
	else if(motorValue[0] == 0)
	{
		t =4;
		robotmove();
	} 
}
int main (void)
{
	// Set up I/O
	wiringPiSetupPhys();
	pinMode(S4, INPUT);
	pinMode(S2, INPUT);
	pinMode(S3, INPUT);
	#define encoderLeft 40
	pinMode(encoderLeft, INPUT);
	pinMode(encoderRight, INPUT);
	
	// set pwm hardware
	pinMode(pwm1FOR, PWM_OUTPUT);
	pinMode(pwm0FOR, PWM_OUTPUT);
	// pinMode(pwm1REV, PWM_OUTPUT);
	// pinMode(pwm0REV, PWM_OUTPUT);
	pinMode(pwm1REV, OUTPUT);
	pinMode(pwm0REV, OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(15);
	pwmSetRange(128);
	wiringPiISR(encoderLeft,INT_EDGE_RISING,&read_encoderLeft);
	wiringPiISR(encoderRight,INT_EDGE_RISING,&read_encoderRight);
	// RFID ////////////////////////////////////////
	// wiringPiSetup();					   
	wiringPiSPISetup(channel, speed);		
	printf ("Raspberry Pi RFID\n") ;
	motorValue[0] = 0;

	unsigned char status, i;

	delay(100);
	PcdReset();		
	PcdAntennaOff();	
	delay(10);		
	PcdAntennaOn();  
	////////////////////////////////////////////////
	
	// Set gia tri ban dau cho Robot

	// MQTT Sub Config
	MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_setCallbacks(client, NULL, NULL, on_messageManagers, NULL);
	// MQTTClient_setCallbacks(client, NULL, NULL, on_messageUsers, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    MQTTClient_subscribe(client, SUB_TOPIC, 0);
    MQTTClient_subscribe(client, SUB_TOPIC1, 0);


	while(TRUE){

	
		///////////////////////////START DONG CO - Do Line/////////////////////////////////////////////
		Cal_EncoderLeft();
		Cal_EncoderRight();
		// temp = motorValue[0];

		if (temp ==33) {robot_RFID(3);}
		else if(temp == 11) robot_RFID(1);
		else if (temp ==22) robot_RFID(2);

		
		
		char msg [25];
		sprintf(msg,"%d/%.4f/%d/%.4f",rpm1, tocdo1, rpm2, tocdo2);
		publish(client, PUB_TOPIC, msg);
		//printf("RPM: %d , tocdo: %.4f \n", rpm, tocdo);	
		sleep(0.5);
		// //////////////////////////END DONG CO - Do Line//////////////////////////////////////////////////////

		// /////////////////////START RFID_CARD////////////////////////////////////////////////////
		status = PcdRequest(PICC_REQALL, g_ucTempbuf);	
		if (status != MI_OK)
		{    
			continue;
		}
		
		status = PcdAnticoll(g_ucTempbuf);			
		if (status != MI_OK)
		{    
			continue;
		}
		
		printf("\ncard SN:");	
		for(i=0;i<4;i++)
		{
			if(g_ucTempbuf[i] < 0x10)    printf("0%X \n",g_ucTempbuf[i]);
			else    printf("%X \n",g_ucTempbuf[i]);	

		}
		
		if(g_ucTempbuf[1] == 0x53) t=1;
		else if(g_ucTempbuf[1] == 0x7E) t=2;
		else if(g_ucTempbuf[1] == 0xBD) t=3;
		
		printf("RFID: %d \n", t);
		
		
		// status = PcdSelect(g_ucTempbuf);			
		// if (status != MI_OK)
		// {
		// 	continue;
		// }
		
		// status = PcdAuthState(PICC_AUTHENT1A, 1, DefaultKey, g_ucTempbuf);	
		// if (status != MI_OK)
		// {
		// 	continue;
		// }
		
		// status = PcdWrite(1, data1);						
		// if (status != MI_OK)
		// {
		// 	continue;
		// }
		
		// status = PcdBakValue(1, 2);						
		// if (status != MI_OK)
		// {
		// 	continue;
		// }
		
		// PcdHalt();		
		// delay(1000);

		//////////////////////////END RFID_CARD/////////////////////////////////////////////////

		
		}
	MQTTClient_disconnect(client, 1000);
	MQTTClient_destroy(&client);
	return rc;	

}
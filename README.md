# AT command Library for GSM module in Linux

It is a library for using GSM features asynchronously in Linux.

# How to use the library?

## Getting Start


Simply Add the Address of your serial port and period of polling in seconds and, that's it !!!

```C
char portAddress[]="/dev/ttyUSB0";
int pollingSMSPeriod=60;//Seconds
GSM_t *gsm;

int main(){
    gsm=gsmSetup(portAddress,pollingSMSPeriod);
    while(1){
        sleep(1);
    }
    return 0;
}
```

## How to execute a command?

Executing command can be either synchronously and asynchronously.

### **ASynchronous**

To execute a command Asynchronously, first, instantiate an instance of that command from the heap, then initialize it with its constructors then send it to the program Command Queue.

```C
Cmd_t *command=calloc(1,sizeof(Cmd_t));// allocate command from heap
Command_ctor(command);//initialize the command.
addToQueue(command,&gsm->mq);// add command to Command Queue
```

For some commands that have additional parameters, you should initialize the parameter outside the constructor. for example, to send an SMS

```C
/*Command structure for sending an SMS
 * Notice that Cmd_t must be the first element of the struct so that this struct is a child of Cmd_t
 */

typedef struct{
    Cmd_t super;
    char phoneNumber[20];//the phone number is not more than 20 character
    char message[512];//sim800 can not support more than 512 character
}SMSPacket_T;


```

```C
//custom value for additoinal parameter
char message[]="Ahmad.Baneshi@gmail.com\n My Github github.com/AhmadBan\n My LinkedIn linkedin.com/in/ahmad-banshee/";
char phone[]="989350542618";
//Allocate and initialize base variable 
SMSPacket_T *sendSms=malloc(sizeof(SMSPacket_T));
SMSSend_ctor((Cmd_t*)sendSms);

//initialize additional parameter with custom value
memcpy(sendSms->message,message,strlen(message));
memcpy(sendSms->phoneNumber,phone,strlen(phone));

addToQueue((Cmd_t*)sendSms,mq);
```

Finally, to add your custom callback function to executes in case of an error or a success just, initialize "fpCallBackOnSuccess" and "fpCallBackOnError" with your custom function. for example

```C
//declare and implementing your callback function
int yourCallbackFuncOnSuccess(Cmd_t* me){
    //Your Code here
    printf("SMS sent sucessfully");
}

int yourCallbackFuncOnError(Cmd_t* me){
    //Your code here
    printf("Could not Send SMS");
}


...
//somewhere in your code
SMSPacket_T *sendSms=malloc(sizeof(SMSPacket_T));
SMSSend_ctor((Cmd_t*)sendSms);
sendSms->super.fpCallBackOnSuccess=yourCallbackFuncOnSuccess;
sendSms->super.fpCallBackOnError=yourCallbackFuncOnError;
addToQueue((Cmd_t*)sendSms,mq);

```

Notice that by default, if a command executes successfully, it is automatically removed from Command Queue. Then, it freed from the heap. Otherwise, the command stays in Command Queue and executes again.

### **Synchronous**

Executing a command synchronously is similar to the above, but instead of executing the command in another thread, you should call the "fpProc" function of that command in your program and wait for it to finish. However, you should initialize the port parameter while you are instantiating the command, and also you are responsible to free your allocated memory yourself.

```C
//sucessful callback
int getOperator(Cmd_t* operator){
    GetOperator_t* me=(GetOperator_t*)operator;
    if(me->operator[0]!=0){
        printf("operator is %s\n",me->operator);
        gsmReady=1;
    }
    else{
        gsmReady=0;
    }
    return 0;
}

....
//somewhere in yourcode
GetOperator_t *getOp=calloc(1,sizeof(GetOperator_t));
getOperator_ctor((Cmd_t*)getOp);
//you must introduce the port to your command instance
getOp->super.port=gsm->port;

getOp->super.fpCallBackOnSuccess=getOperator;
do{
    //call fpProc function to process command synchronously in your program
    getOp->super.fpProc((Cmd_t*)getOp);
}
while(gsmReady!=1);//wait until GSM module find an operator
//you must free your allocated memory yourself
free(operator);
```

Notice you can get an instance of communicating port from GSM_t struct that its reference is returned from "gsmSetup" function

```C
typedef struct {
    MyQueue_t mq;//Queue used for buffer command
    int port;
    int inputSMSPollingPeriod;
}GSM_t;

GSM_t *gsm;


int main(){
    gsm=gsmSetup(portAddress,pollingSMSPeriod);
    //getting port refrence from gsm
    cmd->super.port=gsm->port;

}


```
# How this library structured?

Everything is working around a thread and a queue that holds all commands that are going to be executed in the GSM module. You fill the queue inside your code and the library does everything for you and, by declaring a callback function, you can be informed about the result(Either an OK or ERROR or more).

### commands

All commands follow a struct interface "Cmd_t". This interface contains all the basic properties and methods that are necessary for a specific command. For example, it includes some properties that keep command characteristics such as id and delays that are needed for command in each stage as well as methods that are used mainly to execute the command and get the result as a function pointer such as fpRequest, fpResponse, and fpProc

```C
struct Cmd_t{

    uint8_t id;//unique number for every command
    uint8_t procId;//reserved
    uint8_t priority;//reserved
    char command[50]; //contains the command string of the request
    char finishParam[20];//contains parameter for command
    int32_t (*fpRequest)(struct Cmd_t* me);//function pointer for method that send request command
    uint16_t reqDelayMs;//delay for executing command in GSM module
    int32_t (*fpResponse)(struct Cmd_t* me);//function pointer for method that process response from gsm
    uint16_t respDelayMs;//delay for GSM to send responses
    char expectedAnswerOnSucessCommand[10];//part of expected answer if GSM processed request successfully
    char expectedAnswerOnError[10];//part of expected answer if GSM processed request unsuccessfully
    int32_t (*fpProc)(struct Cmd_t* me);//function pointer for method that manages request , responses
    int8_t retry;//number of attempts if request fails
    int32_t (*fpReset)(struct Cmd_t* me);//function pointer for method that reset GSM
    int port;//serial port file Descriptor
    void (*fpCallBackOnSuccess)(Cmd_t* me);//function pointer that executes if request return successful (it is assigned by user)
    void (*fpCallBackOnError)(Cmd_t* me);//function pointer that executes if request return unsuccessful (it is assigned by user)
};
 ```

**id** : it is a unique number for each command

**command[50]**: it is a string that contains a command string for that command for example for reading SMS it is "AT+CMGL="

**finishParam[20]**: it is a string that contains a parameter for that string to finish command for example for reading SMS it is "\"ALL\"\r".

**fpRequest**: this is a function that is responsible to send the request command.

**fpResponse**: this is a function that is responsible to process the response from the command.

**fpProc**: this is a function that is responsible to process requests and response commands and return the result to callbacks.

**xxxdelays**: provide delay that is needed for every command in different stages.

 These properties and methods are initialized in constructors for each command.

 There are basic methods in "basicCommand.C" module file that can be used for simple commands but if needed one can override its own method for more complex commands. Also, it is possible to add more property and method to Cmd_t and make a new structure with the help of derivation of a struct in C by inserting "Cmd_t" struct as the first element in the struct for example,

```C

typedef struct{
    int msgId;
    char date[15];
    char hour[15];
    char status[15];
    char phoneNumber[20];//the phone number is not more than 20 character
    char message[512];//sim800 can not support more than 512 character
}SMS_t;

typedef struct{
    Cmd_t super;
    SMS_t  sms;
}ReadSMS_t;

```

In this example, ReadSMS_t is a type derived from Cmd_t and it can be easily upcasted to and downcasted from it.



# Add a new command to the library

AT library has a lot of commands and I design architecture so it can be really easy to add a new command to it.

If you want to add a simple two-part command ( commands that send a command and receives ok or an error as responses) to it just create a .C file in the src/commands folder and make a constructor file to initialize the Cmd_t structure for that command such as command string and delays needed. for example


```C
void disableCall_ctor(Cmd_t* me){
    Cmd_t base={
        DISABLE_CALL_SIG,//id
        0,//procid
        0,//priority
        "AT+GSMBUSY=1",//command[50]
        "\r",//finishParam[2];
        0,  //fpSend
        1,//sendDelayMs
        0,//fpReceive
        1,//receiveDelayMs
        "\r\nOK\r\n",//expectedAnswerOnSucess
        "\r\nERROR\r\n",//expectedAnswerOnError
        0,//fpProc
        10,//retry
        0,//    port
        0,//user callback on success
        0//user callback on error
    };
    *me=base;
    
    base_ctor(me);

}
```

in the case of a two-part command using this constructor is sufficient, however, in the case of the more complex command, you would better override your own fpProc,fpRequest, and fpResponses. Moreover, if you need more properties than in the Cmd_t struct you need to define your custom struct as explained above to be derived from Cmd_t. for example, in the case of reading All SMS I need an extra parameter to maintain SMS information so making the struct **ReadSMS_t** like below.

```C
typedef struct{
    int msgId;
    char date[15];
    char hour[15];
    char status[15];
    char phoneNumber[20];//the phone number is not more than 20 character
    char message[512];//sim800 can not support more than 512 character
}SMS_t;

typedef struct{
    Cmd_t super;
    SMS_t  sms;
}ReadSMS_t;

```

I need to implement two custom processes and response methods other than the default function in basicCommand.c file so I override them as below.


```C
int32_t readSMSResponse(Cmd_t* me){
    ...
    
}


int32_t readSMSProc(Cmd_t* me){
    ...
}
```
and then I used them in the constructor to initialize the function pointer for that command.

```C
void readSMS_ctor(Cmd_t* me){


    Cmd_t base={
            READ_SMS_SIG,//id
            0,//procid
            0,//priority
            "AT+CMGL=",//command[50]
            "\"ALL\"\r",//finishParam[10];
            0,  //fpSend
            1,//sendDelayMs
            0,//fpReceive
            1,//receiveDelayMs
            "+CMGL:",//expectedAnswerOnSucess
            "\r\nERROR\r\n",//expectedAnswerOnError
            0,//fpProc
            1,//retry
            0,//    port
            0,//user callback on success
            0//user callback on error
    };
    *me=base;
    //using default functions for fpReset an fpRequest
    me->fpReset=baseReset;
    me->fpRequest=baseReq;
    //using custom function to override fpResponse and fpProc functions
    me->fpResponse=readSMSResponse;
    me->fpProc=readSMSProc;
}
```


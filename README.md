# Async AT command Library for GSM module in Linux

This is a library for using GSM features asynchronously in linux.

## how this library structured ?

Everything is working around a thread and a queue that holds all commands that is going to be executed in GSM module. You fill the queue inside your code and the library do everything for you and by declaring a callback function you can be informed about the result(Either an OK or ERROR or more).

### commands

All commands follow a struct interface "Cmd_t". This interface contains some properties and methods that is needed for a specific command. For example it includes some properties that keeps command charactristics such as id, and delays that is needed after sending command in each stage as well as methods that are used mainly to execute a command and getting the result as a function pointer such as fpInit, fpSend and fpReceive

```C
struct Cmd_t{

    uint8_t id;//unique number for every command
    uint8_t procId;//reserved
    uint8_t priority;//reserved
    char command[50]; //I think maximum length of a command must not be more than 50 character
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

 These properies and methods are initialized in a constructors for each command. 

 There are basic methods in basicCommand module file that can be used for simple command but if needed one can override its own method for more complex command. Also it is possible to add more property and method to Cmd_t and make a new structure with the help of derievation of a struct in C by inserting "Cmd_t" struct as the first element in struct for example,

```C

typedef struct{
    Cmd_t super;
    void (*SMSReceivedCallBack)(SMS_t* sms);
}ReadSMS_t;

```

In this example ReadSMS_t is a type derived from Cmd_t and it can be easily upcasted to it and whenever needed its pointer can be upcasted again.
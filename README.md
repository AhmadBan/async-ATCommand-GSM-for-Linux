# Async AT command Library for GSM module in Linux
This is a library for using GSM features asynchronously in linux.

## how this library structured ?
Everything is working around a thread and a queue that holds all commands that is going to be executed in GSM module. You fill the queue inside your code and the library do everything for you and by declaring a callback function you can be informed about the result(Either an OK or ERROR or more).

### commands
All commands follow an interface "Cmd_t". This interface contains some properties and methods that is needed for a specific command. For example it includes some property that keep command charactristics such as id, priority and delays that is needed after sending command in each stage as well as methods that are used mainly to execute a command and getting the result as a function pointer such as fpInit, fpSend and fpReceive

    struct Cmd_t{

	uint8_t id;
	uint8_t procId;
	uint8_t priority;
	char initCommand[50];
	char command[50]; //I think all command is not more than 50 character
	char finishParam[10];
	int32_t (*fpInit)(struct Cmd_t* me);
	uint16_t initDelayMs;
	int32_t (*fpSend)(struct Cmd_t* me);
	uint16_t sendDelayMs;
	int32_t (*fpReceive)(struct Cmd_t* me);
	char expectedAnswerOnSucessCommand[10];
	char expectedAnswerOnError[10];
	uint16_t receiveDelayMs;
	int32_t (*fpProc)(struct Cmd_t* me);
	int8_t retry;
	int32_t (*fpReset)(struct Cmd_t* me);
	int port;
	int32_t (*fpCtor)(struct Cmd_t* me);

    };
 
 These properies and methods are initialized in a constructors for each command. 

 There are basic methods in basicCommand module file that can be used for simple command but if needed one can override its own method for more complex command. Also it is possible to add more property and method to Cmd_t and make a new structure with the help of derievation of a struct in C by inserting "Cmd_t" struct as the first element in struct for example,

    typedef struct{
	    Cmd_t super;
	    void (*SMSReceivedCallBack)(SMS_t* sms);
    }ReadSMS_t;
In this example ReadSMS_t is a type derived from Cmd_t and it can be easily upcasted to it and whenever needed its pointer can be upcasted again.
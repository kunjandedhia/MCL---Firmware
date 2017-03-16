20th/August Copy

/*change1*/
HMI Code
adding parameter for UTC OFFSET
implemented TIME zone and UTC part 
tested rigourous testing is remaining
UTC TIME TESTING RELATED
/*change2*/
fnGr_PutString(190, 380, HMI_STRING, RGB(0,0,0),LIGHT_GREY_CLR, ORIENTATION_90,0);
HMI DIAGNOSTIC DISPLAY RELATED
/*change3*/
#define SCREEN_ETH_IP_DETAILS		(uint8_t)104
#define SCREEN_WIFI_IP_DETAILS		(uint8_t)105
ETHERNET AND WIFI CONFIGURATION RELATED
/*change4*/
fnLCDbmp(230,5,50,50,ucrgError);
related to 80% memory alarm

/*change5*/
TIME STAMP ADDITION ON ALARMS AND NOTIFICATION SCREEN

/*change6*/
last joint metric unit as ml
others in litres
Imperial is in oz

/*change7*/
addition of Job Id
/*change8*/
addition of apply settings for wifi
/*change9*/
/* Command from HMI to Clear all Error */	

/*change10*/
liquid level calculation first tank height was in cm and current value in mm hence current value was divided by 10.
after changes in units now tank height is also in mm now divide by 10 is removed.

/*change11*/
Upstroke ==0x01
Downstroke 0x02
Up threshold 0x01
down th 0x02				


/***************************************/
HMI
version1.5 release
/***************************************/

/*change12*/

Recirculation valve /stall valve changes 
a) fault message
b) ON/OFF status message
c) recirc valve connected diagnostic status message
d) recirc valve status   diagnostic message
e) SCU diagnostic message changes.
f) Config message for PORT number and Bypass

/*change13*/
Diagnostic status display of NVSRAM1 and NVSRAM2 of SCU 
Diagnostic status display of Recirc valve connected and status
In Status Valve Status screen display of Recirc Valve.

/*change14*/
Send command to turn on Recirc Valve

/*change15*/
Config Recirc valve port and Bypass

/*change16*/
addition of Spray Process Fail reason

/***************************************/
HMI
version1.6 released on 6nov2016
/***************************************/
Bug in Display String

Previous Alarm1 for screen 1,2,3,4
modified to Alarm1 for screen 1
Alarm2 for screen 2
Alarm3 for screen 3
Alarm4 for screen 4

/***************************************/
HMI
version1.7 released on 29/nov/2016
/***************************************/
/*change17*/
Current Joint No parameter changed to KP
JOB ID change to JOINT ID
 
/*change18*/
Memory acknowledgement*/


/***************************************/
HMI
version1.8 on 9/dec2016
/***************************************/
/*change19*/
Added Remote Switch Status message for Debugging.

/***************************************/
HMI
version1.9 on 12/dec2016
/***************************************/
/*change20*/
Added Remote Switch command message to do process.

/***************************************/
HMI
version2.0 released on 15/dec2016
/***************************************/
/*change21*/
Area of cylinder mm2 in metric and inch2 in imperial

from metric to imperial mm2 to inch2 multiplying factor 0.00155

from imperial to metric inch2 to mm2 multiplying factor 645.16

/***************************************/
HMI
version2.1 released on 10/Jan/2017
/***************************************/
/*change22*/
Bug resolved for joint number message sending after spray
/*change23*/
Material usage current joint will be in litres


/***************************************/
HMI
version2.2 released on 25/Jan/2017
/***************************************/
/*change24*/
When downstroke fails display result for upstroke as fail

/***************************************/
HMI
version2.3 released on 31/Jan/2017
/***************************************/
Display fo Stall result modified

/***************************************/
HMI
version2.4 released on 2/Feb/2017
/***************************************/
Error warning LED display resolved
/***************************************/
HMI
version2.5 released on 3/Feb/2017
/***************************************/
/*change25*/
VBAT TIME Should not be less than 15 sec
/*change26*/
Blue Led on for warning

/***************************************/
HMI
version2.6 released on 7/Feb/2017
/***************************************/
VBAT TIME can be set less than 15 sec
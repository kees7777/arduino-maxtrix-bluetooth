// Use the MD_MAX72XX library to scroll text on the display
// enter text on the serial monitor or BLUETOOTH MODULE

#include <MD_MAX72xx.h>
#include <SoftwareSerial.h>

SoftwareSerial BT(7, 8); // RX, TX

// Turn on debug statements to the serial output
#define  DEBUG  1

#if  DEBUG
#define	PR(s, x)	{ Serial.print(s); Serial.println(x); }
#define	PP(x)	        Serial.print(x)
#define	PN(x)	        Serial.println(x)
#else
#define	PR(s, x)
#define PP(x)
#define	PN(x)	
#endif

#define	PR1(s, x)	{ BT.print(s); BT.println(x); }

#define	MAX_DEVICES	16

#define	CLK_PIN		13  // or SCK
#define	DATA_PIN	11  // or MOSI
#define	CS_PIN		10  // or SS

MD_MAX72XX mx = MD_MAX72XX(CS_PIN, MAX_DEVICES);

#define	CHAR_SPACING	1	// pixels between characters
#define	END_SPACING	10	// pixels na bericht

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
char curMessage[BUF_SIZE];
char newMessage[BUF_SIZE];
bool newMessageAvailable = false;
uint8_t  helderheid   = 2 ;
uint16_t  scrollDelay   = 25;	// in milliseconds


void testcode(void) {
 char code  ;  
     if (newMessage[0] =='#') 
      {  newMessageAvailable = false;
       
        code = newMessage[1] ;
        switch (code) {
          case 'a' : scrollDelay = constrain( (scrollDelay * 1.3 ) , 4 , 255 ) ; PR1("speed " , scrollDelay); break;
          case 'b' : scrollDelay = constrain( (scrollDelay / 1.3 ) , 4 , 255 ) ; PR1("speed " , scrollDelay); break;
          case 'c' : helderheid  = constrain( (helderheid + 1 )  , 0 , 8 ) ; mx.control(MD_MAX72XX::INTENSITY, helderheid ) ;PR1("licht " , helderheid); break;  //  0 tot 8 
          case 'd' : helderheid  = constrain( (helderheid - 1 )  , 0 , 8 ) ; mx.control(MD_MAX72XX::INTENSITY, helderheid ) ;PR1("licht " , helderheid); break;
          case '\n': break;
          case '\r': break;
          default : PP("not " ); break;
         }
      }
     else  newMessageAvailable = true; 
     if (newMessage[2] =='\r') newMessageAvailable = false; // disable blootooth disconnect message 
}

void readSerial(void)
{  static uint8_t	putIndex = 0;  
   while (Serial.available())
  {    newMessage[putIndex] = (char)Serial.read();    BT.write(newMessage[putIndex]);
      if ((newMessage[putIndex] == '\n') ||(putIndex >= BUF_SIZE-3))	
       {  for (int tel = 0; tel < END_SPACING; tel++) {  newMessage[putIndex++] = ' '; }
          newMessage[putIndex] = '\0';    putIndex = 0;
          testcode();
       }
      else      newMessage[putIndex++];   
  }
 }

void readBT(void)
{  static uint8_t	putIndex = 0;
  while (BT.available())  {
    newMessage[putIndex] = (char)BT.read();    Serial.write(newMessage[putIndex]);
    if ( (newMessage[putIndex] == '\n')  ||(putIndex >= BUF_SIZE-3)) 
      { for (int tel = 0; tel < END_SPACING; tel++) {  newMessage[putIndex++] = ' '; }	
      newMessage[putIndex] = '\0';      putIndex = 0;   
      testcode();
      }
    else  newMessage[putIndex++];  
  }
}

uint8_t scrollDataSource(uint8_t dev, MD_MAX72XX::transformType_t t)
// Callback function for data that is required for scrolling into the display
{
  static char		*p = curMessage;
  static uint8_t	state = 0;
  static uint8_t	curLen, showLen;
  static uint8_t	cBuf[8];
  uint8_t colData;

  // finite state machine to control what we do on the callback
  switch(state)
  {
    case 0:	// Load the next character from the font table
      showLen = mx.getChar(*p++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state++;

      // if we reached end of message, reset the message pointer
      if (*p == '\0')
      {
        p = curMessage;			// reset the pointer to start of message
        if (newMessageAvailable)	// there is a new message waiting
        {
          strcpy(curMessage, newMessage);	// copy it in
          newMessageAvailable = false;
        }
      }
      // !! deliberately fall through to next state to start displaying

    case 1:	// display the next part of the character
      colData = cBuf[curLen++];
      if (curLen == showLen)
      {
        showLen = CHAR_SPACING;
        curLen = 0;
        state = 2;
      }
      break;

    case 2:	// display inter-character spacing (blank column)
      colData = 0;
      curLen++;
      if (curLen == showLen)
        state = 0;
      break;

    default:
      state = 0;
  }

  return(colData);
}

 void scrollText(void)
{  static uint32_t	prevTime = 0;

  // Is it time to scroll the text?
  if (millis()-prevTime >= scrollDelay)
  {
    mx.transform(MD_MAX72XX::TSL);	// scroll along - the callback will load all the data
    prevTime = millis();			// starting point for next time
  }
}

void setup()
{  mx.begin();
  delay(100);
  mx.setShiftDataInCallback(scrollDataSource);
  
   strcpy(curMessage, "Bt text + newline        ");
  newMessage[0] = '\0';

  Serial.begin(9600);
  BT.begin(9600);
  Serial.println("Message Display , End message line with a newline");
 delay(500);
}

void loop() 
{ readSerial(); 
  readBT() ;
  scrollText();
 
 }

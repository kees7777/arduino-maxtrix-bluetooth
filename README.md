# arduino-maxtrix-bluetooth
Controlling MAX7219 matrix bar with bluetooth module HC-06.
Maybe RTC module for time/date control by I2C or extracting from telephone by bluetooth connection.

Hardware SPI for maxtrix modules.

With arduino mega using Hardware serial on Serial1 port.
A Tx1(18) > 4k7 -10k divider > Rx HC-06
A Rx1(19) >                    Tx HC-06

With arduino Uno  using Hardware serial on combined port or Software Serial .

Goal is : bluetooth control for :
Brightness / scrolling speed / On Off / time display / countdown for event / adding and deleting message in message array. Some with time component in them.

Commands starting with '#'
Second caracter "letter" for simple comm ands and alphanumeric for time related message.
   Example : #21:15textmessage




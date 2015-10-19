# Tunnel_Racer
##Scroller video game in C implemented on an Atmega 1284 microcontroller. 

https://www.youtube.com/watch?v=3bsM5Pr-qks

The game allows the user to control a “racer” (green LED) who can only move left and right. He must avoid the walls (blue LEDS) of the tunnel he is travelling through. As time goes on, the walls curve more sporadically and the tunnel becomes harder to navigate. The longer the player stays alive, the more points he gets. If the racer touches a wall, the game ends and the player can restart. Anytime during the game, the player can click the reset button which will restart the game. All scores are recorded and regardless of soft-reset, the high score is always displayed

###Hardware:
* ATMEGA1284 Microcontroller Chip
* LCD Display
* Multiplexed Keypad
* 4 8-bit Shift Registers
* 1 8x8 RBG LED Matrix

###Software:
* AVR Studio 6

###Files:
* SPEED_RACER5.0.c : Contains the majority of the source code. There are 5 concurrent state machines.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* TickFct_Keypad: SM that continuously checks for Keypad input. If any of the 4 main buttons are 
pressed, their flags are adjusted accordingly.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* TickFct_Matrix: SM that continuously outputs the required values for red, green, and blue 
LEDS. This SM also polls for “lose” since it has the
fastest period.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* TickFct_Matrix2: SM very similar to Matrix. The key difference is that this SM takes the input 
flags from all other SM’s and modifies the red, blue, and green variables. These variables are sent 
to the first Matrix function for output.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* TickFct_LCD: Sends output to the LCD Display.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* TickFct_LevelGen: This function ticks every 10th of a second. It modifies the tunnel to “move” 
towards the player. It also contains a random number generator that makes the incoming parts of the 
tunnel. As the game goes on, the tunnel moves more spontaneously and is harder to navigate.

* SuperShiftFunction.c
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* A modified shift function that allows the user to output 2 ports to 4 shift registers instead of 
1 to 1.

###How to Play:
Once the system is powered, the LCD will display a greeting and the player character is outputted 
on the LED Matrix.

* Use ‘*’ to move left and ‘D’ to move right. You can move the player in the menu to gauge the 
movement speed.
* Click ‘B’ once to make the mouth of the tunnel appear. This allows you to line your character 
with the opening.
* Click ‘B’ again to initiate travel. Use left and right to avoid the walls.
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* Click ‘A’ anytime in the game to reset back to the menu

The LCD displays the high score in the menu. Once the game starts, a meter appears that tracks your 
progress in the tunnel. If you hit a wall, “CRASH!” appears and your score is outputted.

###Known Bugs:

* In the menu, the character LED flashes every second or so. It is stable during the game, but 
only flashes during the menu.
* When the player hits a wall, the LCD displays random characters for a split second before going 
back to normal. (Happens ¼ of the time.)
* Almost all LED’s ghost. The character ghost can be utilized as a “target” that the player is 
moving towards.
* Out of about 20 run through, 3 to 4 of them resume from where the player crashed instead of 
restarting completely. The score still resets, but the player position in the tunnel does not. This 
happens very occasionally.

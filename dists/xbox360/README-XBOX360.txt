ScummVM 1.4.1 ported by A600

This is a ScummVM port for the Xbox 360, possible thanks to the hard work of
the ScummVM team, the SDLx360 libs by Lantus and the XBDM plugin by Natelx.

It features all the 1.4.1 supported engines, MP3, Vorbis, Flac, AAC, 
FluidSynth (tested with up to 250 MB soundfonts) and the MT-32 emu.

The file xbox.patch is the diff patch against the 1.4.1 branch
https://github.com/scummvm/scummvm.git


IMPORTANT NOTE
--------------

Don't use the Guide button because the Xbox 360 will probably hang.



CONTROLS
--------

Left Analog 		-> Move Cursor
Dpad 			-> Arrow keys. In the select game dialog, left=pgup, rigth=pgdown
A			-> Left Button
B 			-> Right Button
X 			-> Escape
Y 			-> Enter. In Lands of Lore, attack button for all party members (F1+F2+F3)
Back			-> R (Rest party in Lands of Lore)
Start			-> ScummVM Menu
Left Trigger 		-> Increases cursor speed while pressed. In Lands of Lore, turn left
Right Trigger 		-> Decreases cursor speed while pressed. In Lands of Lore, turn right
Left Shoulder		-> F5
Right Shoulder		-> Virtual Keyboard
Right Analog 		-> Numeric keypad as shown below
Right Analog Thumb	-> KP5


7   8   9
  \ | /
4 - 5 - 6
  / | \
1   2   3



SCREEN
------

The xResizer.xex included allows to resize the screen for those with overscan problems.
It generates an xbox.cfg with these default settings:

xpos=0
ypos=0
xstretch=0
ystretch=0

NOTE: for a pixel perfect screen with correct aspect ratio these values
and the 3x* scalers should be used:

xpos=160
ypos=0
xstretch=-320
ystretch=0 


-A600-
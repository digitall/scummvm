ScummVM 1.4.1 ported by A600

This is a ScummVM port for the Xbox1, possible thanks to the hard work of
the ScummVM team and the SDLx libs by Lantus.

It features all the 1.4.1 supported engines, MP3, Vorbis, Flac, AAC 
and FluidSynth (the MT-32 emu isn't included because the Xbox lacks
the power to handle it). All HQ scalers should work.

The file xbox.patch is the diff patch against the 1.4.1 branch
https://github.com/scummvm/scummvm.git


What's new:

- Updated to ScummVM 1.4.1 (check the NEWS file for the changes)
- Source code clean-up. Xbox specific code moved to its own files.
- Xbox 360 port (check README-XBOX360.txt for more info)
- Added command line support. This is a .cut example for XBMC:

<shortcut>
   <path>f:\Emu\ScummVM\default.xbe</path>
   <label>Lands of Lore</label>
   <thumb>Lands of Lore.png</thumb>   
      <custom>
         <game>lol-cd</game>
      </custom>
</shortcut>

<game></game> must contain the game-id for the game you want to launch.
The game-id is the name between brackets from the scummvm.ini file.



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
White 			-> F5
Black 			-> Virtual Keyboard
Right Analog 		-> Numeric keypad as shown below
Right Analog Thumb	-> KP5


7   8   9
  \ | /
4 - 5 - 6
  / | \
1   2   3



SCREEN
------

The xResizer.xbe included allows to resize the screen for those with overscan problems.
It generates an xbox.cfg with these default settings:

xpos=0
ypos=0
xstretch=0
ystretch=0
flickerfilter=1
720p=1

The flickerfilter option can be a number between 0 and 5.

NOTE: for a pixel perfect screen with correct aspect ratio these values
should be used (for the 720 mode, use the 3x* scalers):


480p:
-----
xpos=40
ypos=0
xstretch=-80
ystretch=0

720p:
-----
xpos=160
ypos=0
xstretch=-320
ystretch=0 


1.4.0 Changelog
---------------

- Unlike previous versions, only games officially supported are included.
- Changed compiling options to gain 3 MB of memory.
- Added numeric keypad "emulation" with the right analog needed by, for example,
  the "Indiana Jones and the Last Crusade" fights (big thanks to zx81 for the
  analog joystick source code)
- Digital pad used for cursor keys and Y button for the ENTER key.
- Fixed a pretty big memory leak when using fluidsynth and returning to the launcher.
- Fixed a bug where F5 key wasn't released after returning to the launcher.
- Now screen options are loaded from xbox.cfg so there is no need to edit the
  scummvm.ini anymore.
  IMPORTANT: If you have an xbox.cfg from the 1.3.0git Update1, delete it because
  it may hang the Xbox.
- Added the libfaad2 lib needed by The 7th Guest iOS (untested)
- Added keys needed by Lands of Lore (check the controls list for more details)
  With this controller configuration, LOL is more than playable. I finished the game
  without problems and, in fact, I prefer the gamepad to the keyboard + mouse combo.


-A600-
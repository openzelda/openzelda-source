===================================================================
  Open Zelda v7 alpha - Release notes
  
  Written by GD & Luke
  13 March 2012
  website: http://classic.openzelda.net
===================================================================

What is Open Zelda Classic Edition?

The classic edition of Open Zelda is a legacy build to support older
quest that not supported by the modern edition. All the Windows 
related has been removed from the code allow for the code to ported 
other systems. 

The follow was written by GD when the source code was release in 2003

So to use Open Zelda you will need to get hold of a Quest file to
play, these are .QST files, they contain all the games media,
sound files, scripts, map and storyline in one package. Now, this
release comes with a demo quest, but that will get boring very 
quickley, pretty soon there will be lots of quests on the internet
to download, but the real fun is making them yourself!

I mentioned the scripting system in Open Zelda, everything in the game
is treated as an "Entity", an entity is just an interactive object.
Every entity has a script which defines what it does, the scripts are
written (by anyone) in a scripting language called "Small" which is
very similar to C, but not nearly as hard. This means that the
only thing which sets, say, the player apart from a door is the
script that entity runs, it's an interesting system which works
extremely well.

The most important thing is that all the entity scripts are available
to anyone who wants them, which means that Open Zelda is Open source,
(in the way that matters).


---------------------------------------------------------------------
Default Game Controls:

Move the Player - Arrow Keys
Talk/Interact   - Q Key
Use Weapon A    - A Key
Use Weapon B    - S Key
Menu Screen	- Enter


---------------------------------------------------------------------
Technical Info:

I will begin by stating that Open Zelda is techincally a 3D game, it
may look 2D, but all those images you see are made from flat polygons.
This has good and bad points, the good points are that Open Zelda can
do some amazing special effects, it can scale, rotate and alpha blend
sprites as big as the screen itself without dropping a frame. You can
do cool things like realtime day and night, shadow and lighting effects
and a whole lot more.

The bad news I guess is that Open Zelda wont run on older machines, it
requires that you have a video card with some kind of 3D acceleration, 
although most computers have them, certainly every new PC will have a
card capable of running Open Zelda extremely well. And 3D cards are 
dirt cheap anyway. Open Zelda might not run on every 3D card, I can't
test it on every card, but it has been developed to work with the
most popular card set, the Geforce cards. Open Zelda will run fine on 
any Geforce card, like a Geforce2.


Minimum System Requirements:

Pentium 300 or higher
64 mb RAM
a 3D card capable of OpenGL acceleration (Geforce card recommended).
16 bit desktop colour highly recommended


---------------------------------------------------------------------



# Rocket-League-Chroma-Control

##Purpose 
Uses Razer's Chroma SDK to control the Razer Bblackwidow Chroma Keyboard according to ingame events 
from the game Rocket League by Psyonix.

##Usage
Before this application can work, you **MUST** have Chroma Apps enabled in Razer Synapse. If you
don't, this is how it is done: 

1. Start Razer Synapse. 
2. On the topmost navbar, select Chroma Apps. 
3. On the top of the following page, toggle Chroma Apps "on". 
 
In order to detect the ingame events, RLCC or the user has to start Rocket League with the -log flag. This flag will open an **additional console window** while Rocket League is running.  **Leave this window allone**, if you close it, both Rocket League and RLCC will stop running. After closing Rocket League, RLCC will end, out of neccessity at this point.


##Potential problems
If for some reason your Rocket League installation is not entered in your registry, RLCC can't launch Rocket League for you, in that case: 

1. In your Steam library, righ-click Rocket League and select properties. 
2. Set Launch Options.
3. Enter -log in the dialog and hit ok after.
4. You can now start Rocket League from Steam and it should be detected by RLCC.

##Download
If you just got here and don't know this whole github thing, a zip download is available by clicking on "releases" on the navbar on top. Or follow this link: https://github.com/d-karl/Rocket-League-Chroma-Control/releases

##Licensing
QT version 5.5 used under LGPL v3.

libnoise version 1.0.0 used under LGPL v2.1.

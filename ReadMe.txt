MS4 features:
WAD wall orientation.
wall trigger volume events functioning.
Raycasting for slipstreaming and FRAG gun.
Reset ability to get back on the track if you go off.
Driving model tuning.
Springy Camera
Rearview camera
Wall Deletion (buggy, not included)
Fps capped
New wall rendering system
In-game UI 
player bike/wall colors differentiated
SFX for engine, collisions, ability usage (all present, although mixing makes some hard to hear)
Randomized soundtrack 

Known issues:
The following are basically all one issue with aligning walls physically and graphically:
	Graphical walls and physical walls aren't exactly 1:1 and drift over time, thus:
	Walls aren't deleted on collision or upon being shot with the Frag gun due to a bug that would otherwise delete the wrong walls.
	WAD trap-walls are deployed physically, but exacerbate the wall offset and are set to not currently render.

How to Play:
"W"/"S" to accelerate/decelerate.
"A"/"D" to turn left/right.
"Left Shift" to brake.
"Up arrow" to "Jump" on a 1 second cooldown (costs Structural Integrity (health)).
"Left arrow"/"Right arrow" to strafe left/right on a 1 second cooldown(costs Structural Integrity (health)).
"R" to reset position of the bike to starting position (costs Structural Integrity (health)).
"Spacebar" to shoot the FRAG gun (costs Structural Integrity (health)).
Hold "Down Arrow" to charge the WAD, release to deploy a perpindicular wall-segment behind you (currently invisible) (costs Structural Integrity (health)).
"V" for view change to rear view.
Slipstreaming: Driving adjacent to walls will, after a brief windup, increase acceleration and begin regenerating Structural Integrity (health).
Colliding with walls reduces bike velocities and damages it (due to not being deleted, excess walls accrue over time and can be "sticky").
If your Structural Integrity (health) is depleted, your bike is disabled for the duration of the race.
UI updates current lap, race position, your health, and will indicate the race winner, at which point bikes are disabled.

Future developments:
Layering a Menu system on top of the game.
Fixing the graphical-physical wall discrepancy to allow wall collisions, WAD graphics, & FRAG gun collisions to function properly.
Expanding AI behaviour.
Audio mix balancing.
Refining the driving model.
Additional rendering features.
bug fixes.
optimizations.
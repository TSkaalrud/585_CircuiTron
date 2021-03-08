MS3 features:
Skybox with ambient lighting
Functional (though not yet competitive AI) which will carefully make its way around the track on a programmatic waypoint path.
Improvements to the driving model
physical track can now be driven on
Pilot ability to Jump and/or Strafe 
Fully spawning wall segments based on distance (graphically renders only a simplified static mesh still)
Trigger Volume detection for wall segments (though not yet hooked up into other features)


Known issues:
aWall nullptr: reproducible via various sharp changes of heading, wall objects are unable to render due to insane quaternions
	- avoid by driving somewhat conservatively.
"A$$ over tea kettle": if the chassis of the bike becomes inverted you can fall through terrain, ignoring collision, and falling through the "abyss"
***Audio: requires OpenAL installed? Not packaged in the build atm?


How to Play:
"W"/"S" to accelerate/decelerate
"A"/"D" to turn left/right
"Up arrow" to "Jump" on a 1 second cooldown
"Left arrow"/"Right arrow" to strafe left/right on a 1 second cooldown
Waypoints and laps update in the console currently to inform the player on their game progress

Future developments:
Ability to shoot with the FPC 
ability to drop walls behind the player with the WAD
Slipstreaming ability to utilize walls to heal and accelerate
further improvements to AI and driving model
falling off the track will result in a game over
destructible walls and bikes
walls will slow and damage any bike that triggers them
Walls rendered consistently to the physics walls
skybox specular lighting
improved camera motion
Sound effects to be added to appropriate gameplay features
UI/menus
bug fixes
optimizations


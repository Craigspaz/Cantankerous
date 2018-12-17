# Cantankerous

Created by Craig Ferris

### Videos

*Click on the images of the videos to go to the video*

- Final Video 
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/4qI8E6tPJX0/maxresdefault.jpg)](https://www.youtube.com/watch?v=4qI8E6tPJX0)
- Second Status Update Video
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/g9OkHOtbZXs/maxresdefault.jpg)](https://www.youtube.com/watch?v=-LqgKJLrDDk)
- First Status Update Video
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/wH3U3SDuZtM/maxresdefault.jpg)](https://www.youtube.com/watch?v=wH3U3SDuZtM)

### Gameplay

This is a multiplayer based Real Time Strategy (RTS) game. One player will assume the role as host. The other players will join the game as clients. The clients will need to know the server's IP address. Port 1234 will need to be allowed through firewalls if you are having connection issues. Once connected the game starts. Each player has a factory building which gives them the ability to create tanks. 

Clicking Spawn Tank adds the tank to the build queue of the factory. This is done to balance the game a little bit so players can't spawn units immediately. Once the tanks have spawned they will appear in the factory. You can click on them and then click anywhere in the map other than Orange tiles and they will find a path there.

If you select a tank you control and then click on a tank another player controls your tank will go up to the enemy tank and if it is in range fire projectiles at it to destroy it. The same thing occurs if you click on a building controlled by another player.

You win the game if you destroy all other players' buildings and tanks.

### Technical Component

This game was made using Ogre3D, and WinSockets on Windows. 
The techincal component was network multiplayer. Every instance of the game can be either the server or a client. If the user clicks Host Game on the main menu they will host a game. Their game will start listening on port 1234 for connections. Clients will need to click Join Game and type the servers IP address in to connect. 
Once the client establishes the connection with the server it will send a Hello message to the server to get information such as which level to load and what player number it is.
The client acts as a dumb terminal in this model. The server is the brains behind the entire game. If the clients want to do something with a short list of exceptions it will need to ask the server.
For example, the client can move the camera around freely without asking for the server to manage that. The client can also determine what the user clicks on. If the client selects a tank on the battlefield the client will determine that the tank was selected and it will keep track of this and not tell the server. This is because the server does not need to know about what the client has selected.
If the client wants to move a tank it must ask the server to move it. The client will send a message to the server asking for it to move the tank to a certain destination. The server will then use A\* pathfinding to determine the path the tank must take. Then it will simulate the tanks movement. The server will send the clients update about where the tank is and the direction it is facing. 
When a client receives one of these messages its job is to update the world based on what the server said. The server will send data about each item even if it is not needed just in case the client lost a packet. *(Note this should not happend since the game uses TCP protocal for the network packets.)*

Every item in the game has a unique identifier which the client and server use to communicate which items they are talking about.
If a client wants a tank to attack another tank or a building it must ask the server to do this. For example, every unit and building has an ID and every tile has a (x,y) coordinate. 

###### How a message is formatted.

A message is a string of XML. For example, the following may be a response to the client who just sent a hello message.

```XML
<Level>
	TestLevel2.txt
</Level>
<PlayerID>
	2
</PlayerID>
```

The XML is expanded depending on what needs to be sent. The receiving end parses the XML.


### Controls

Arrow keys on keyboard move camera around.

The rest is done by clicking on buttons/objects on screen.

Left click selects units and buildings.

Right click deselects whatever you have selected.

### Install

You will need to install the [Ogre3D SDK](https://www.ogre3d.org/) on your computer.

Inside the resources directory in this repository there is a directory called "media". Copy that folder over your Ogre3d SDK's media directory. This will add models and textures this game uses.

Then using CMake set the source files in this repository as your source directory and created a build directory somewhere and build to that directory. Once you configure and generate files based on your system you should be able to compile and run the game. **NOTE: This game uses WinSockets so it is only available at the moment on Windows**.
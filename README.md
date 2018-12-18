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
My techincal component was network multiplayer. Every instance of the game can be either the server or a client. If the user clicks Host Game on the main menu they will host a game. Their game will start listening on port 1234 for connections. A client will also get started on the same instance to allow them to play. They will be player 1. Clients will need to click Join Game and type the servers IP address in to connect. 
Once the client establishes the connection with the server it will send a Hello message to the server to get information such as which level to load and what player number it is.

The client acts as a dumb terminal in this model. The server is the brains behind the entire game. If the clients want to do something with a short list of exceptions it will need to ask the server.
For example, the client can move the camera around freely without asking for the server to manage that. The client can also determine what the user clicks on. If the client selects a tank on the battlefield, the client will determine that the tank was selected. The client will keep track of what its player has selected and not tell the server. This is because the server does not need to know about what the client has selected.
If the client wants to move a tank it must ask the server to move it. The client will send a message to the server asking for it to move the tank to a certain destination. The server will then use A\* pathfinding to determine the path the tank must take. Then it will simulate the tanks movement. The server will send the clients updates about where the tank is and the direction it is facing. 
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

The client and server have multiple threads. The server has an instance of a Server and an instance of a Client running at the same time. The main thread updates both the client and server 60 times a second. The server also has a thread that listens for connections. If a connection is established another thread is created which will just listen for messages from the client that started the connection.
The client also has a thread that listens for messages from the server. 

The use of multiple threads allows the client and server to listen for messages and process them while other things are happening in the game. This increases performance. The only issue with this design is the Ogre3D does not support multithreading. So data received in a thread that is not the main thread is stored in shared memory. The main thread will then see the shared memory has changed and update the game accordingly.

Below is a diagram showing where the threads are created for both the client and server. The diagram shows where threads are created and which ones run in parallel. Time is shown as you read down the diagram.

```
Server:

    |
    Main Thread (Created when the game launches)
    |\
    | \
    |  \
    |   \
    |    \
    |     \
    |      \
    |       \
    |        \
    |         \
    |          \
    |           \
    |            \
    |             \
    |              \
    |               \
    |                \    
    |                 \
    |                  \
    |                   \
    |                    \
    |                     \
    |                      \
    |                       \
    |                        \
    |                         \
    |                          \
    |                           \
    |                            \
    |                             \
    |                              \
    |                               \
    |                                \
    |                                 \
    |                                  \
    |                                   \ 
    |                                    \ 
    |                                     \ 
    |                                      \
    |                                       \
    |                                        \ 
    |                                         \ 
    |                                          \ 
    |                                           \ 
    |                                            \ 
    |                                             \ 
    |                                              \
    |                                               \ 
    |                                                \ 
    |                                                 \
    |                                                  \ 
    |                                                   \
    |                                                    \
    |                                                     \
    |                                                      \ 
    |                                                       \
    |                                                        \ 
    |                                                         \
    |                                                          \
    |                                                           \
    |                                                            \
    |                                                             \
    |                                                              \
    |                                                               \
    |                                                                Creates thread listening for connections
    |                                                                |
    |                                                                |
    |                                                                |
    |                                                                |
    |                                                                If a connection from a client is established
    |                                                                |\
    |                                                                | \
    |                                                                |  \
    |                                                                |   \
    |                                                                |    \
    |                                                                |     \
    |\                                                               |      \
    | \                                                              |       |\
    |  \                                                             |       | \
    |   \                                                            |       |  \
    |    \                                                           |       |   (?) The number of threads depends on the number of clients.
    |     Client creates thread to listen for messages from server   |       |
    |      |                                                         |       |
    |      |                                                         |       |
    |      |                                                         |       |
    |      |                                                         |       |
    |      |                                                         |       |
    |      |                                                         |       |
    |      |                                                         |       |
    |      |                                                         |       |
    |      |                                                         |       |
    V      V                                                         V        V


Client:

    |
    Main Thread (Created when the game launches)
    |\
    | \
    |  \
    |   \
    |    \
    |     \
    |      \
    |       \
    |        \
    |         \
    |          \
    |           \
    |            \
    |             Client creates thread to listen for messages from server
    |             |
    |             |
    |             |
    |             |
    |             |
    |             |
    V             V
```

#### How the network multiplayer was implemented in more details.

I used WinSockets which is like C Sockets but made for Windows.

I wrote a wrapper around the core functions. I did this to try to encapuslate the socket code from the rest of the game. The wrapper functions also handle some basic error checking to make sure the socket is working. This helps keep the rest of the game cleaner.

WinSockets uses the following core functions.

``` 
	int send(socket, message, length, flags);
	int recv(socket, buffer, sizeOfBuffer, flags);
```

I wrote a wrapper functions which look similar but eliminate the flags parameter and also check to see if there was an error sending or receiving data.

```
	int sendMessage(SOCKET socket, char* message, const int length);
	int receiveMessage(SOCKET sock, char* buffer, const int bufferSize);
```

In the update loop for the server the server will loop through all of the units in the game and using the wrapper functions above send all of the clients XML formatted messages telling them details about the state of the unit.
For example, it may say unit with ID=4 is now at position (34, 22, 8) and is facing in direction (0,1,0) and is alive. The clients will upon receipt of this message look up the unit with ID=4 and update the position and facing direction.
The server will do the same thing with the buildings in the game.

When a client wants to tell the server to do something such as move a unit to a specific location it will also use the wrapper functions mentioned above. The client will send an XML formatted message saying unit with ID=7 wants to travel to tile (3, 4).
The server will upon receipt run the A\* pathfinding algorithm to find a path to the specified tile. The client will see the update in the servers unit update messages described above when it is told about the units position.

The server will send information even if the client already has the latest information on something. This is because a client may lose packets. (This is using TCP protocol so it should not happen.) Also if a client joins the game late it will need to know about everything in the game.
It would not be good if the player can only see a few units on the battlefield and not all of them. They may see their units randomly disapear. But in reality they were just not notified of an update.

### Controls

Arrow keys on keyboard move camera around.

The rest is done by clicking on buttons/objects on screen.

Left click selects units and buildings.

Right click deselects whatever you have selected.

### Install

You will need to install the [Ogre3D SDK](https://www.ogre3d.org/) on your computer.

Inside the resources directory in this repository there is a directory called "media". Copy that folder over your Ogre3d SDK's media directory. This will add models and textures this game uses.

Then using CMake set the source files in this repository as your source directory and created a build directory somewhere and build to that directory. Once you configure and generate files based on your system you should be able to compile and run the game. **NOTE: This game uses WinSockets so it is only available at the moment on Windows**.

You may either need to copy the compiled executable in the Ogre3D SDK/bin directory or set that directory as your working directory in your favorite IDE for the game to run properly. (You may need to do this if you are getting missing Ogre_____.dll errors when you try to run the game.
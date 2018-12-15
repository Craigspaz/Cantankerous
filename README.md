# Cantankerous

Created by Craig Ferris

### Videos

- Final Video Still In Progress
- Second Status Update Video
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/g9OkHOtbZXs/maxresdefault.jpg)](https://www.youtube.com/watch?v=-LqgKJLrDDk)
- First Status Update Video
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/wH3U3SDuZtM/maxresdefault.jpg)](https://www.youtube.com/watch?v=wH3U3SDuZtM)

### Gameplay

This is a multiplayer based Real Time Strategy (RTS) game. TBD ...


### Controls

Arrow keys on keyboard move camera around.

The rest is done by clicking on buttons/objects on screen.

### Install

You will need to install the [Ogre3D SDK](https://www.ogre3d.org/) on your computer.

Inside the resources directory in this repository there is a directory called "media". Copy that folder over your Ogre3d SDK's media directory. This will add models and textures this game uses.

Then using CMake set the source files in this repository as your source directory and created a build directory somewhere and build to that directory. Once you configure and generate files based on your system you should be able to compile and run the game. **NOTE: This game uses WinSockets so it is only available at the moment on Windows**.
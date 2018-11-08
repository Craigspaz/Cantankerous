cmake_minimum_required (VERSION 2.8)
project(FinalProject)

## [discover_ogre]
# specify which version and components you need
find_package(OGRE 1.11 REQUIRED COMPONENTS Bites RTShaderSystem)

# copy resource.cfg next to our binaries where OGRE looks for it
file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_BINARY_DIR})
## [discover_ogre]

add_executable(FinalProject main.cpp Game.cpp Unit.cpp Building.cpp Level.cpp Tile.cpp Server.cpp Client.cpp)
target_link_libraries(FinalProject ${OGRE_LIBRARIES})



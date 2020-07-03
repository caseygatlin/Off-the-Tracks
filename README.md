# Description

Off the Tracks is a cooperative on-rails race to the finish where you control your cart using a manually pumped railroad handcar. This game was developed by myself and 10 other awesome people. It is currently in development for a Steam release, check us out on Twitter [@_Offthetracks](https://twitter.com/_OfftheTracks) for updates.

# Contributions

For this project I programmed the movement and leaning systems for the cart in-game, building off of a third-party blueprint library. I, with one of the other engineers on the team, converted the 20+ file Train and Rail library ([found here](https://www.unrealengine.com/marketplace/en-US/product/train-and-rail-system)) from Unreal Blueprints to C++ and played a major role in assisting our team with moving from Blueprints to C++ code. Along with that, I created custom actors like the first and second-player cameras and modified the game to work with two monitors for in-person play.

### Example Files

A few examples of the work I've done can be found at these locations in the repository:
- Collision box that allows for end-of-level behaviors.
    - [`EndLevelBox.h`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/Gamecode/Actors/EndLevelBox.h)
    - [`EndLevelBox.cpp`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/Gamecode/Actors/EndLevelBox.cpp)
- Cameras
    - [`DetachedCamera.h`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/Gamecode/Actors/DetachedCamera.h)
    - [`DetachedCamera.cpp`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/Gamecode/Actors/DetachedCamera.cpp)
- Track piece that marks the end of the level.
    - [`EndTrack.h`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/Gamecode/Tracks/EndTrack.h)
    - [`EndTrack.cpp`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/Gamecode/Tracks/EndTrack.cpp)
- Conversion of third-party blueprint library.
    - [`TrackTrigger.h`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/TrainAndRail/TrackTrigger.h)
    - [`TrackTrigger.cpp`](https://github.com/caseygatlin/Off-the-Tracks/blob/master/main/HaphazardHandcart/Source/HaphazardHandcart/TrainAndRail/TrackTrigger.cpp)


[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=17206062&assignment_repo_type=AssignmentRepo)

This project is the networking code to be used in my game project called Knight and Slime. It is a 2 player puzzle platformer based on the game Fire Boy and Water Girl.
This game is being made in Unity and also has a GitHub repo <link here>, this c++ code will be integrated into the c# focused unity by basically making it a library.
This networking "library" is being kept in c++ and not c# because I may have to jump ship if this project proves to difficult and I want to be able to slavage the not game specific protocol.

Over the first couple of days this project repo has gone through lots of design changes.
Current Design: p2p server client
  - the host is always the knight
  - the client is always the slime

Here are the files no longer being used in the current version of the design, but are being kept around for future scalability and improvements:
- server.hpp
- server.cpp
- player.hpp
- player.cpp

Here are the files that are not game-specific, but will need more functions to make them more robust for other projects
- basicNetworking.hpp
- basicNetworking.cpp
- host.hpp
- host.cpp
- client.hpp
- client.cpp
- player.hpp
- player.cpp
- server.hpp
- server.cpp

Here are the game specifc files
- knightHost.hpp
- knightHost.cpp
- slimeClient.hpp
- slimeClient.cpp

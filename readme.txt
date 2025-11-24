c++ | Raylib | Boost asio

*build instruction for client:
- use MSys32 MINGW64
- navigate to [projectFolder]/client or server
- run command line
    - mkdir build
    - cd build
    - rm -r *
    - cmake .. -G "MinGW Makefiles"
    - cmake --build .


for server:
- use MSys32 MINGW64
- run command :
g++ server.cpp -o server.exe -lws2_32 -lmswsock -mconsole
mkdir tmp
cd tmp
g++ -c ../3rd/enet/callbacks.c -I../3rd
g++ -c ../3rd/enet/compress.c  -I../3rd
g++ -c ../3rd/enet/host.c  -I../3rd
g++ -c ../3rd/enet/list.c  -I../3rd
g++ -c ../3rd/enet/packet.c  -I../3rd
g++ -c ../3rd/enet/peer.c  -I../3rd
g++ -c ../3rd/enet/protocol.c  -I../3rd
g++ -c ../3rd/enet/unix.c  -I../3rd
g++ -c ../src/core/core.cpp -I../3rd -I../src -I../src/core  -std=gnu++11 -w
g++ ../src/main_game.cpp -I../3rd -I../src -I../src/core  -std=gnu++14 -w -fpermissive -Wno-deprecated *.o -o../bin/Game
g++ ../src/main_hello_world.cpp -I../3rd -I../src -I../src/core  -std=gnu++14 -w -fpermissive -Wno-deprecated *.o -o../bin/Hello
g++ ../src/main_benchmark.cpp -I../3rd -I../src -I../src/core  -std=gnu++14 -w -fpermissive -Wno-deprecated *.o -o../bin/Benchmark


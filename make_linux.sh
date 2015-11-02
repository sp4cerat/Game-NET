mkdir tmp
cd tmp
g++ -c ../3rd/enet/callbacks.c -I../3rd
g++ -c ../3rd/enet/compress.c  -I../3rd
g++ -c ../3rd/enet/host.c  -I../3rd
g++ -c ../3rd/enet/list.c  -I../3rd
g++ -c ../3rd/enet/packet.c  -I../3rd
g++ -c ../3rd/enet/peer.c  -I../3rd
g++ -c ../3rd/enet/protocol.c  -I../3rd
g++ -c ../3rd/enet/unix.c -DHAS_SOCKLEN_T  -I../3rd
g++ -c ../src/core/core.cpp -I../3rd -I../src -I../src/core  -std=c++1y -w
g++ -pthread -lpthread ../src/main_game.cpp -I../3rd -I../src -I../src/core  -std=c++1y -w -fpermissive -Wno-deprecated *.o -o../bin/Game
g++ -pthread -lpthread ../src/main_hello_world.cpp -I../3rd -I../src -I../src/core  -std=c++1y -w -fpermissive -Wno-deprecated *.o -o../bin/Hello
g++ -pthread -lpthread ../src/main_benchmark.cpp -I../3rd -I../src -I../src/core  -std=c++1y -w -fpermissive -Wno-deprecated *.o -o../bin/Benchmark


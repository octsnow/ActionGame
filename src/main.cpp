#include "Game.hpp"
#include "Sound.hpp"


//#ifdef DEBUG
//#pragma comment(lib, "opencv_world455d.lib")

//#else
#pragma comment(lib, "opencv_world455.lib")

//#endif


#ifdef OCT_DEBUG
#pragma comment(lib, "oct_game_d.lib")

#else
#pragma comment(lib, "oct_game.lib")

#endif


#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "oct_binary.lib")
#pragma comment(lib, "oct_wav.lib")

using namespace std;

int main(int argc, char** argv) {
    Game game;

    game.Start(argc, argv);

    return 0;
}

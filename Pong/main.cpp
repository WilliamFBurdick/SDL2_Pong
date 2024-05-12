#include <stdexcept>
#include "Game.h"
#include <time.h>

int main(int argc, char* argv[])
{
	try
	{
		srand(time(NULL));
		Game game;
		game.Run();
	}
	catch (std::exception e)
	{

	}
	return 0;
}
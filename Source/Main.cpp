#include "PCH.h"
#include "Engine.h"

int main(int argc, char* argv[])
{
	Engine& engine = Engine::GetInstanceWrite();
	engine.Initialize();
	engine.Run();
	engine.Shutdown();

	return EXIT_SUCCESS;
}


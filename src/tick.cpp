#include "tick.h"
#include "server.h"
#include <thread>

long lastTime;

void tick() {
    long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    		std::chrono::system_clock::now().time_since_epoch()).count();
    double delta = (ms - lastTime) / 1000.0;
    
	std::vector<PlanetSurface*> surfacesToTick;
	for (ServerInterface::Conn conn : iface.connections) {
		for (PlanetSurface *surf : conn->surfacesLoaded) {
			if (std::find(surfacesToTick.begin(), surfacesToTick.end(), surf) == surfacesToTick.end()) {
				surfacesToTick.push_back(surf);
			}
		}
	}

    for (PlanetSurface *surf : surfacesToTick) {
		surf->tick(delta);
	}
	//std::cout << "Ticked " << surfacesToTick.size() << " surfaces\n";
    logger.num_surfaces(surfacesToTick.size());

	lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	    		std::chrono::system_clock::now().time_since_epoch()).count();
    if (ticks % 100 == 0) {
	    save(); //TODO not a good idea!
	}
	ticks++;
}

void runServerLogic() {
     long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    		std::chrono::system_clock::now().time_since_epoch()).count();
    lastTime = ms;
	while (true) {
		unsigned long long startns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();
	    
		tick();

		unsigned long long endns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();

		std::this_thread::sleep_for(std::chrono::microseconds(100000 - (endns - startns)));
		unsigned long long lastns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();
		//std::cout << "Tick took " << (endns - startns) / 1000.0 << "ms (" << 1000000.0 / (lastns - startns) << "tps)\n";
		logger.tps((endns - startns) / 1000.0, 1000000.0 / (lastns - startns));
	}
}

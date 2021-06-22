#include "server.h"
#include "logging.h"
#include "generation.h"
#include "network.h"
#include <fstream>
//TODO possible race condition with tasks

Logger logger;
ServerInterface iface(5555);
std::string saveName = "testsavev2.0";
uint64_t ticks = 0;

/*
void saveTasks() {
    Json::Value tasksJson;
    for (Task& t : tasks) {
        Json::Value value;
        value["type"] = (int)t.type;
        value["target"] = t.target;
        Json::Value surface;
        getJsonFromSurfaceLocator(t.surface, surface);
        value["surface"] = surface;
        value["timeLeft"] = t.timeLeft;
        tasksJson["tasks"].append(value);
    }
    std::ofstream afile;
	afile.open(saveName + "/" + "tasks.json");
	Json::StreamWriterBuilder writeBuilder;
	writeBuilder["indentation"] = "";
	afile << Json::writeString(writeBuilder, tasksJson) << "\n";
	afile.close();
}*/

void save() {
    map.saveAll(saveName);
    map.unloadAll();
    //saveTasks();
    //TODO save ticks
}

/*
void loadTasks() {
    std::ifstream afile;
	afile.open(saveName + "/" + "tasks.json");

	std::string content((std::istreambuf_iterator<char>(afile)), (std::istreambuf_iterator<char>()));

    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value root;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        content.c_str(),
        content.c_str() + content.size(),
        &root,
        &errors
    );
    delete reader;

    if (!parsingSuccessful) {
        std::cerr << "Error: could not parse JSON in tasks file\n";
    }

	afile.close();
	for (Json::Value task : root["tasks"]) {
         SurfaceLocator loc = getSurfaceLocatorFromJson(task["surface"]);
         tasks.push_back({(TaskType)task["type"].asInt(),
                          task["target"].asUInt(),
                          loc,
                          task["timeLeft"].asDouble()});
     }
}*/

int main() {
    const uint32_t LEVEL_SEED = 12345;

    noiseGen.SetNoiseType(FastNoise::Simplex);
    srand(LEVEL_SEED);
    loadConfig();
    //loadTasks();
    registerTaskTypeInfo();

    iface.startServer();
}

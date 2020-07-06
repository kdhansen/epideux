#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <fstream>
#include <random>

#include "spdlog/spdlog.h"
#include "epideux/epideux.h"

int main(int argc, char const *argv[]) {
  spdlog::set_level(spdlog::level::debug);

  using namespace std;
  using namespace epideux;
  using namespace std::chrono;
  using namespace std::chrono_literals;

  Model sim_model;
  sim_model.setStartDate(2020, 4, 1);

  double num_agents = 5;

  Location& home1 = sim_model.createLocation(0.7, "Home 1");
  Location& home2 = sim_model.createLocation(0.7, "Home 2");

  for (int i = 0; i < num_agents; ++i) {
    Person& person_i = sim_model.createPerson(home1, 24h, 24h);
  }

  auto t = sim_model.currentTime();
  for (int i = 0; i < num_agents; ++i) {
        ItineraryEntry visit2(home2, t + 1h, t + 6h);
        sim_model.getPerson(i).addItineraryEntry(visit2);
      }

  spdlog::debug("Before Simulation: Home 1: {}, Home 2: {}.", home1.getPersons().size(), home2.getPersons().size());
  sim_model.simulate(2h);
  spdlog::debug("After 2h simulation: Home 1: {}, Home 2: {}.", home1.getPersons().size(), home2.getPersons().size());
  sim_model.simulate(22h);
  spdlog::debug("After 22h simulation: Home 1: {}, Home 2: {}.", home1.getPersons().size(), home2.getPersons().size());

  return 0;
}
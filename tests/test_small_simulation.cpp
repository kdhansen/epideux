#include <iostream>
#include "epideux/epideux.h"

int main(int argc, char const *argv[]) {
  using namespace epideux;
  using namespace std::chrono_literals;

  Model sim_model;
  sim_model.setStartDate(2020, 4, 1);

  auto school = sim_model.createLocation(0.7);

  time_pt start_time;
  time_pt end_time = start_time + 2h;
  ItineraryEntry school_it(school, start_time, end_time);

  for (int i = 0; i < 10; ++i) {
    auto my_home = sim_model.createLocation(0.7);
    auto my_person = sim_model.createPerson(my_home);
    my_person->addItineraryEntry(school_it);
  }

  sim_model.simulate(24h);

  return 0;
}
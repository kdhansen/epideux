// Epideux - Agent Based Location-Graph Epidemic Simulation
//
// Copyright (C) 2020  Karl D. Hansen, Aalborg University <kdh@es.aau.dk>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
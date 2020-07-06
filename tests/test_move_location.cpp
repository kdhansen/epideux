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

  Location& location1 = sim_model.createLocation(0.7, "Location1");
  Location& location2 = sim_model.createLocation(0.7, "Location2");

  time_pt start_time = sim_model.currentTime() + 1h;
  time_pt end_time = start_time + 2h;
  ItineraryEntry loc2_it(location2, start_time, end_time);

  for (int i = 0; i < 10; ++i) {
    Person& my_person = sim_model.createPerson(location1, 4*24h, 7*24h);
    my_person.addItineraryEntry(loc2_it);
  }

  std::cout << "Persons in location 1/2: " << location1.getPersons().size()
            << '/' << location2.getPersons().size();

  sim_model.simulate(2h);

  std::cout << " (2h) ";
  std::cout << location1.getPersons().size() << '/'
            << location2.getPersons().size();

  sim_model.simulate(2h);

  std::cout << " (2h) ";
  std::cout << location1.getPersons().size() << '/'
            << location2.getPersons().size() << std::endl;

  return 0;
}
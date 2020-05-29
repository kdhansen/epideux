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

#include "epideux/epideux.h"

using namespace std::chrono_literals;

namespace epideux {

Person::Person(Model& simulation_model, std::shared_ptr<Location> home)
    : model_(simulation_model),
      home_(home),
      incubation_time_(4 * 24h),
      disease_time_(10 * 24h),
      infection_state_(InfectionCategory::Susceptible) {
  home_->enter(*this);
}

void Person::addItineraryEntry(ItineraryEntry new_entry) {
  itinerary_.push_back(new_entry);
}

void Person::infect() {
  if (infection_state_ == InfectionCategory::Susceptible) {
    infection_state_ = InfectionCategory::Exposed;
    infected_date_ = model_.currentTime();
  }
}

void Person::updateInfection() {
  if ((infection_state_ == InfectionCategory::Exposed) &&
      (model_.currentTime() > (infected_date_ + incubation_time_))) {
    infection_state_ = InfectionCategory::Infectious;
  } else if ((infection_state_ == InfectionCategory::Infectious) &&
             (model_.currentTime() > (infected_date_ + disease_time_))) {
    infection_state_ = InfectionCategory::Recovered;
  }
}

InfectionCategory Person::infectionState() { return infection_state_; }

}  // namespace epideux
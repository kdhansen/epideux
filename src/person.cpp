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
#include "spdlog/spdlog.h"

using namespace std::chrono_literals;

namespace epideux {

Person::Person(Model& simulation_model, uint32_t id, Location& home,
               time_duration incubation_time, time_duration disease_time)
    : model_(simulation_model),
      id_(id),
      home_(home),
      incubation_time_(incubation_time),
      disease_time_(disease_time),
      infection_state_(InfectionCategory::Susceptible),
      active_itinerary_entry_(nullptr),
      current_location(&home_) {
  leave_callback_ = home_.enter(*this);
}

void Person::addItineraryEntry(ItineraryEntry new_entry) {
  itinerary_.push_back(new_entry);
  auto* pushed_entry = &(itinerary_.back());
  spdlog::debug("Person {}, adding entry at {}.", id_, (void*) pushed_entry);
  model_.addToSchedule(
      new_entry.startTime(),
      std::bind(&Person::beginItineraryEntry, this, pushed_entry));
  model_.addToSchedule(
      new_entry.endTime(),
      std::bind(&Person::endItineraryEntry, this, pushed_entry));
}

///
/// Moves the person from one location to another.
///
void Person::moveToLocation(Location& location) {
  spdlog::debug("Person {}, Moving to location {}.", id_, location.name());
  leave_callback_();
  current_location = &location;
  leave_callback_ = current_location->enter(*this);
}

///
/// Callback to be used with the Model's schedule
///
/// When this callback is called, the person initiates a move according to his
/// itinerary. If another itinerary entry was active, this one takes precedence.
///
void Person::beginItineraryEntry(ItineraryEntry* entry) {
  spdlog::debug("Person {}, Beginning itinerary entry {}", id_, (void*) entry);
  active_itinerary_entry_ = entry;
  moveToLocation(entry->location());
}

///
/// Callback to be used with the Model's schedule
///
/// When this callback is called, the person evaluates if this itinerary entry
/// is still valid. If so, he moves home as no other entry has taken precedence.
///
void Person::endItineraryEntry(ItineraryEntry* entry) {
  spdlog::debug("Person {}, Ending itinerary entry {} (active entry {})", id_, (void*) entry, (void*) active_itinerary_entry_);
  // Compare the addresses to check if they are the same entry.
  if (entry == active_itinerary_entry_) {
    moveToLocation(home_);
  }
  // TODO: Think about whether to delete the old entries. Now they are just left
  // there in the itinerary. Deleting them could be in efficient as we are using
  // a vector, but a list may be more suited.
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
             (model_.currentTime() >
              (infected_date_ + incubation_time_ + disease_time_))) {
    infection_state_ = InfectionCategory::Recovered;
  }
}

InfectionCategory Person::infectionState() { return infection_state_; }

uint32_t Person::id() const { return id_; }

}  // namespace epideux
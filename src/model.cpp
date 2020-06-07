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

#include <algorithm>
#include <stdexcept>

#include "epideux/epideux.h"

using namespace std::chrono_literals;

namespace epideux {

///
/// @class Model
/// The Model class is the central object in the simulation.
///
/// The model has a pointer to all persons and locations. Use the createPerson
/// and createLocation methods to generate them and automatically registering
/// them with the model.
///
/// The model has a scheduler, which is used to continuously update the persons'
/// infection state. The scheduler uses a variable time-step AKA a 'lazy'
/// approach. It keeps track of the next person to move and updates all the
/// people around that person when he moves. This is opposed to updating
/// everybody at a fixed rate, e.g., once per hour. An entry in the schedule is
/// a std::pair with the time to update as the first member and a callback
/// function as the second.
///

Model::Model() : report_interval_(24h) {
  logger_ = spdlog::basic_logger_mt("Model", "logs/epideux-log.txt");
}

///
/// Run the model.
///
/// This it the function that starts the entire machinery. Use it when you have
/// set up a scenario and everything is ready to run.
///
/// First the simulation generates the reporting events and loads them into the
/// schedule. Then it goes through the schedule from the top, and continues
/// until the commanded duration is simulated.
///
/// @param simulation_duration The period to simulate forward.
///
void Model::simulate(time_duration simulation_duration) {
  // Generate reporting steps and put into schedule
  time_pt stop_sim_at = current_sim_time_ + simulation_duration;
  for (time_pt t = current_sim_time_; t < stop_sim_at; t += report_interval_) {
    schedule_.emplace_back(t, std::bind(&Model::collectSeir, this));
  }
  // Insert simulation stop event.
  schedule_.emplace_back(stop_sim_at, std::bind(&Model::stopSimulation, this));

  // Sort the schedule so everything is executed in order.
  std::sort(schedule_.begin(), schedule_.end());
  simulation_running_ = true;

  // Step through schedule
  logger_->debug("Starting simulation [simtime: {}]", getCurrentTimeString());

  while ((current_sim_time_ < stop_sim_at) && (schedule_.size() > 0)) {
    auto schedule_entry = schedule_.front();
    current_sim_time_ = schedule_entry.scheduled_time;
    schedule_entry.callback();
    schedule_.pop_front();
    logger_->debug("Reported at [simtime: {}]", getCurrentTimeString());
  }

  current_sim_time_ += simulation_duration;

  logger_->debug("Stoping simulation [simtime: {}]", getCurrentTimeString());
  simulation_running_ = false;
}

void Model::setStartDate(int year, int month, int day) {
  std::tm tm = {/* .tm_sec  = */ 0,
                /* .tm_min  = */ 0,
                /* .tm_hour = */ 0,
                /* .tm_mday = */ day,
                /* .tm_mon  = */ month - 1,
                /* .tm_year = */ year - 1900};
  tm.tm_isdst = 1;
  current_sim_time_ = std::chrono::system_clock::from_time_t(mktime(&tm));
}

time_pt Model::currentTime() { return current_sim_time_; }

std::string Model::getCurrentTimeString() {
  time_t t = std::chrono::system_clock::to_time_t(current_sim_time_);
  char t_str[100];
  std::strftime(t_str, sizeof(t_str), "%c", std::localtime(&t));
  std::string time_string(t_str);
  return time_string;
}

Location& Model::createLocation(double beta, std::string name) {
  locations_.emplace_back(*this, beta, name);
  return locations_.back();
}

Person& Model::createPerson(Location& home, time_duration incubation_time,
                            time_duration disease_time) {
  persons_.emplace_back(*this, getNextId(), home, incubation_time,
                        disease_time);
  return persons_.back();
}

Person& Model::getPerson(uint32_t i) {
  auto it = std::find_if(persons_.begin(), persons_.end(),
                         [i](const Person& p) { return p.id() == i; });
  if (it == persons_.end()) {
    throw std::out_of_range("Person not found");
  }
  return *it;
}

SeirReport Model::getReport() { return report_; }

///
/// Go through all agents and record their infection state.
///
/// This method go through all locations and 1) updates the infections at the
/// locations, 2) collects the infection status of the people at the locations.
/// In the end the infection state of all people in the simulation have been
/// updated and recorded.
///
void Model::collectSeir() {
  uint32_t s = 0;
  uint32_t e = 0;
  uint32_t i = 0;
  uint32_t r = 0;

  for (auto& l : locations_) {
    l.updateInfections();
    for (const auto& p : l.getPersons()) {
      switch (p->infectionState()) {
        case InfectionCategory::Susceptible:
          s++;
          break;
        case InfectionCategory::Exposed:
          e++;
          break;
        case InfectionCategory::Infectious:
          i++;
          break;
        case InfectionCategory::Recovered:
          r++;
          break;
      }
    }
  }

  report_.susceptible.push_back(s);
  report_.exposed.push_back(e);
  report_.infectious.push_back(i);
  report_.recovered.push_back(r);
}

///
/// Callback that is being called when the simulation stops.
///
void Model::stopSimulation() { return; }

///
/// Get the random generator from the model.
///
/// Use this generator for all randomness, then a single seed can control and
/// entire simulation run.
///
std::mt19937& Model::randomGenerator() { return random_generator_; };

///
/// Get the next id for a new agent
///
uint32_t Model::getNextId() { return last_id_++; }

///
/// Add a callback to the schedule
///
/// This is used but the agents to schedule the movements in their itineraries.
///
void Model::addToSchedule(time_pt scheduled_time,
                          std::function<void()> callback) {
  if (simulation_running_) {
    // TODO:
  } else {
    schedule_.push_back(ScheduleEntry(scheduled_time, callback));
  }
}

}  // namespace epideux
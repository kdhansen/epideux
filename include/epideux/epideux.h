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

#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <random>

#include "spdlog/sinks/basic_file_sink.h"

namespace epideux {

// Forward Declerations
class Person;
class Model;

typedef std::chrono::time_point<std::chrono::system_clock> time_pt;
typedef std::chrono::system_clock::duration time_duration;

enum class InfectionCategory {
  Susceptible,
  Exposed,
  Infectious,
  Recovered
};

struct SeirReport{
  std::vector<uint32_t> susceptible;
  std::vector<uint32_t> exposed;
  std::vector<uint32_t> infectious;
  std::vector<uint32_t> recovered;
};

class Location {
 public:
  Location(Model& simulation_model, double beta, std::string name);
  const std::vector<Person*>& getPersons() const;
  void updateInfections();
  void enter(Person& new_person);
  void leave(Person& leaving_person);

 private:
  Model& model_;
  std::string name_;
  std::vector<Person*> persons_;
  time_pt last_update_;
  double beta_per_sec_;
};

class ItineraryEntry {
 public:
  ItineraryEntry(std::shared_ptr<Location> location, time_pt start,
                 time_pt end);
  time_pt startTime();
  time_pt endTime();
  std::shared_ptr<Location> location();

 private:
  time_pt start_, end_;
  std::shared_ptr<Location> location_;
};

class Person {
 public:
  Person(Model& simulation_model, std::shared_ptr<Location> home);
  void addItineraryEntry(ItineraryEntry new_entry);
  void infect();
  void updateInfection();
  InfectionCategory infectionState();

 private:
  Model& model_;
  std::shared_ptr<Location> home_;
  std::vector<ItineraryEntry> itinerary_;
  InfectionCategory infection_state_;
  time_pt infected_date_;
  time_duration incubation_time_;
  time_duration disease_time_;
};

class Model {
 public:
  Model();
  std::shared_ptr<Location> createLocation(double beta, std::string name = "");
  std::shared_ptr<Person> createPerson(std::shared_ptr<Location> home);
  void simulate(time_duration simulation_duration);
  void setStartDate(int year, int month, int day);
  time_pt currentTime();
  std::shared_ptr<Person> getPerson(size_t i);
  SeirReport getReport();
  std::mt19937& randomGenerator();

 private:
  std::vector<std::shared_ptr<Location>> locations_;
  std::vector<std::shared_ptr<Person>> persons_;
  bool simulation_ready_;
  std::shared_ptr<spdlog::logger> logger_;
  time_pt current_sim_time_;
  SeirReport report_;
  time_duration report_interval_;
  void collectSeir();
  std::string getCurrentTimeString();
  std::deque<std::pair<time_pt, std::function<void()>>> schedule_;
  std::mt19937 random_generator_;
};

}  // namespace epideux
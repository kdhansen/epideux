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
#include <list>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "spdlog/sinks/basic_file_sink.h"

namespace epideux {

// Forward Declerations
class Person;
class Model;

typedef std::chrono::time_point<std::chrono::system_clock> time_pt;
typedef std::chrono::system_clock::duration time_duration;

enum class InfectionCategory { Susceptible, Exposed, Infectious, Recovered };

struct SeirReport {
  uint32_t susceptible = 0;
  uint32_t exposed = 0;
  uint32_t infectious = 0;
  uint32_t recovered = 0;
};

typedef std::vector<SeirReport> SeirTimeline;

class Location {
 public:
  Location(Model& simulation_model, double beta, std::string name);
  Location(const Location&) = delete;
  Location& operator=(const Location&) = delete;
  Location(Location&&) = delete;
  Location& operator=(Location&&) = delete;
  const std::list<Person*>& getPersons() const;
  void updateInfections();
  std::function<void()> enter(Person& new_person);
  void leave(std::list<Person*>::iterator& leaving_person_it);
  SeirReport collectSeir();
  void setBeta(double new_beta);
  std::string name();

 private:
  Model& model_;
  std::string name_;
  std::list<Person*> persons_;
  time_pt last_update_;
  double beta_per_sec_;
};

class ItineraryEntry {
 public:
  ItineraryEntry(Location& location, time_pt start, time_pt end);
  time_pt startTime() const;
  time_pt endTime() const;
  Location& location() const;

 private:
  Location& location_;
  time_pt start_, end_;
};

class Person {
 public:
  Person(Model& simulation_model, uint32_t id, Location& home,
         time_duration incubation_time, time_duration disease_time);
  Person(const Person&) = delete;
  Person& operator=(const Person&) = delete;
  Person(Person&&) = delete;
  Person& operator=(Person&&) = delete;
  void addItineraryEntry(ItineraryEntry new_entry);
  void infect();
  void updateInfection();
  InfectionCategory infectionState();
  uint32_t id() const;

 private:
  Model& model_;
  uint32_t id_;
  Location& home_;
  std::list<ItineraryEntry> itinerary_;
  InfectionCategory infection_state_;
  time_pt infected_date_;
  time_duration incubation_time_;
  time_duration disease_time_;
  const ItineraryEntry* active_itinerary_entry_;
  Location* current_location;
  std::function<void()> leave_callback_;
  void moveToLocation(Location& location);
  void beginItineraryEntry(ItineraryEntry* entry);
  void endItineraryEntry(ItineraryEntry* entry);
};

class Model {
 public:
  Model();
  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
  Model(Model&&) = delete;
  Model& operator=(Model&&) = delete;
  Location& createLocation(double beta, std::string name = "");
  Person& createPerson(Location& home, time_duration incubation_time,
                       time_duration disease_time);
  void simulate(time_duration simulation_duration);
  void setStartDate(int year, int month, int day);
  time_pt currentTime();
  Person& getPerson(uint32_t i);
  SeirReport getReport();
  SeirTimeline getDailyReports();
  std::mt19937& randomGenerator();
  void addToSchedule(time_pt scheduled_time, std::function<void()> callback);

 private:
  std::list<Location> locations_;
  std::list<Person> persons_;
  bool simulation_running_ = false;
  std::shared_ptr<spdlog::logger> logger_;
  time_pt current_sim_time_;
  SeirReport latest_report_;
  SeirTimeline daily_reports_;
  time_duration report_interval_;
  void collectSeir();
  void stopSimulation();
  std::string getCurrentTimeString();
  std::mt19937 random_generator_;
  uint32_t last_id_ = 0;
  uint32_t getNextId();

  struct ScheduleEntry {
    ScheduleEntry(time_pt t, std::function<void()> cb)
        : scheduled_time(t), callback(cb) {};
    ScheduleEntry(ScheduleEntry&& other)
        : scheduled_time(std::move(other.scheduled_time))
        , callback(std::move(other.callback)) {};
    time_pt scheduled_time;
    std::function<void()> callback;
    bool operator<(const ScheduleEntry& rhs) {
      return scheduled_time < rhs.scheduled_time;
    }
    ScheduleEntry& operator= (ScheduleEntry&& other) {
         scheduled_time = std::move(other.scheduled_time);
         callback = std::move(other.callback);
         return *this;
    }
  };
  std::deque<ScheduleEntry> schedule_;
};

}  // namespace epideux
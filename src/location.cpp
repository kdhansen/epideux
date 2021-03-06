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
#include <cmath>
#include <iterator>

#include "epideux/epideux.h"

namespace epideux {

///
/// Construct a Location.
///
/// @param simulation_model The simulation model that drives the simulation.
/// @param name A name for the location.
/// @param beta The beta parameter from standard SEIR models in infections/day.
///
Location::Location(Model& simulation_model, double beta, std::string name)
    : model_(simulation_model), name_(name), beta_per_sec_(beta / 86400) {}

///
/// Get a list of persons in the location.
///
/// @returns A reference to the internal list of persons in the location.
///
const std::list<Person*>& Location::getPersons() const { return persons_; }

///
/// Evaluates any possible infections since last update.
///
/// Using a binomial distribution, the method samples a number of people to
/// become infected. When we have found the number of people to infect, we
/// choose them by a uniform selection among all the persons at the location.
/// The random sampled person may be infectious himself but we accept that.
/// (Future considerations may prove us right or wrong).
///
void Location::updateInfections() {
  // Update timestamp. Short-circuit if the time didn't progress since last
  // update.
  time_duration time_delta((model_.currentTime() - last_update_).count());
  last_update_ = model_.currentTime();
  if (time_delta.count() == 0) {
    return;
  }

  // Check if we even have people here. Short-circuit if there are none.
  int num_persons_here = persons_.size();
  if (num_persons_here == 0) {
    return;
  }

  // Let everybody update their infection state.
  for (const auto& p : persons_) {
    p->updateInfection();
  }

  // Count number of infectious people.
  int num_infectious_persons =
      std::count_if(persons_.begin(), persons_.end(), [](const auto& p) {
        return p->infectionState() == InfectionCategory::Infectious;
      });

  // Compute the probability of getting infected.
  auto time_step =
      std::chrono::duration_cast<std::chrono::seconds>(time_delta).count();
  double prob_of_infection =
      1 - std::exp(-beta_per_sec_ * time_step * num_infectious_persons /
                   num_persons_here);

  // Sample a number to infect and select (and infect) them.
  std::binomial_distribution<int> binom_dist(num_persons_here,
                                             prob_of_infection);
  int num_infected = binom_dist(model_.randomGenerator());
  if (num_infected > 0) {
    std::uniform_int_distribution<int> uni_dist(0, num_persons_here - 1);
    auto& gen = model_.randomGenerator();
    std::vector<size_t> infected_indexes(num_infected);
    std::generate(infected_indexes.begin(), infected_indexes.end(),
                  [&uni_dist, &gen](){ return uni_dist(gen); });
    std::sort(infected_indexes.begin(), infected_indexes.end());
    // turn indexes into increments as list iterators are not random access and
    // need to be advanced forward.
    for (int i = infected_indexes.size()-1; i > 0 ; --i) {
      infected_indexes[i] -= infected_indexes[i-1];
    }
    auto persons_it = persons_.begin();
    for (auto i : infected_indexes) {
      std::advance(persons_it, i);
      (*persons_it)->infect();
    }
  }
}

std::function<void()> Location::enter(Person& new_person) {
  // TODO: Should maybe evaluate whether person is allowed in.
  updateInfections();
  persons_.push_back(&new_person);
  std::list<Person*>::iterator new_person_it = persons_.end();
  new_person_it--;
  return std::bind(&Location::leave, this, new_person_it);
}

void Location::leave(std::list<Person*>::iterator& leaving_person_it) {
  updateInfections();
  persons_.erase(leaving_person_it);
}

///
/// Go through all agents and record their infection state.
///
SeirReport Location::collectSeir() {
  updateInfections();

  SeirReport a_report;
  for (auto& p : persons_) {
    switch (p->infectionState()) {
        case InfectionCategory::Susceptible:
          a_report.susceptible++;
          break;
        case InfectionCategory::Exposed:
          a_report.exposed++;
          break;
        case InfectionCategory::Infectious:
          a_report.infectious++;
          break;
        case InfectionCategory::Recovered:
          a_report.recovered++;
          break;
    }
  }

  return a_report;
}

void Location::setBeta(double new_beta) {
  beta_per_sec_ = new_beta / 86400;
}

std::string Location::name() { return name_; }

}  // namespace epideux

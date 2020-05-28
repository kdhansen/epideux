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
#include <cstdlib>
#include <iostream>
#include "epideux/epideux.h"
#include "matplotlibcpp/matplotlibcpp.h"

#include "spdlog/spdlog.h"

int main(int argc, char const *argv[]) {
  spdlog::set_level(spdlog::level::info);

  // Process arguments
  if (argc != 5) {
    std::cout << "Please provide [number of young people] [old people] [beta] [simulation days]";
    return -1;
  }
  int num_young_people = std::atoi(argv[1]);
  int num_old_people = std::atoi(argv[2]);
  double beta = std::atof(argv[3]);
  int sim_days = std::atoi(argv[4]);

  using namespace epideux;
  using namespace std::chrono_literals;
  namespace plt = matplotlibcpp;

  // Setup Model
  Model sim_model;
  sim_model.setStartDate(2020, 4, 1);

  // Create a pool of old people
  Location& old_home = sim_model.createLocation(beta);
  for (int i = 0; i < num_old_people; ++i) {
    Person& a_person = sim_model.createPerson(old_home, 4*24h, 5*24h);
  }
  spdlog::info("{} old people spawned.", num_old_people);

  // Create a pool of young people and infect one
  Location& young_home = sim_model.createLocation(beta);
  for (int i = 0; i < num_young_people; ++i) {
    Person& a_person = sim_model.createPerson(young_home, 4*24h, 5*24h);
    // Pay regular visits after half the sim time
    auto ct = sim_model.currentTime();
    for (time_pt t = ct + (sim_days/2)*24h; t < ct + sim_days*24h; t += 24h) {
      ItineraryEntry old_it(old_home, t, t+2h);
      a_person.addItineraryEntry(old_it);
    }
  }
  spdlog::info("{} young people spawned.", num_young_people);
  sim_model.getPerson(num_old_people).infect();

  // Run model and time it.
  auto start = std::chrono::steady_clock::now();
  sim_model.simulate(24h*sim_days);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> sim_time(end-start);
  spdlog::info("Simulation took {} s", sim_time.count());

  std::vector<int> susceptible_v;
  std::vector<int> exposed_v;
  std::vector<int> infectious_v;
  std::vector<int> recovered_v;
  auto reports = sim_model.getDailyReports();
  for (auto& r : reports) {
    susceptible_v.push_back(double(r.susceptible));
    exposed_v.push_back(double(r.exposed));
    infectious_v.push_back(double(r.infectious));
    recovered_v.push_back(double(r.recovered));
  }

  plt::xkcd();
  plt::figure();
  plt::named_plot("Susceptible", susceptible_v);
  plt::named_plot("Exposed", exposed_v);
  plt::named_plot("Infectious", infectious_v);
  plt::named_plot("Recovered", recovered_v);
  plt::legend();
  plt::show();

  return 0;
}

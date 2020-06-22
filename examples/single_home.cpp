#include <cstdlib>
#include "epideux/epideux.h"
#include "matplotlibcpp/matplotlibcpp.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

int main(int argc, char const *argv[]) {
  // Process arguments
  auto console = spdlog::stdout_color_mt("console");
  if (argc != 4) {
    console->error("Please provide [number of people] [beta] [simulation days]");
  }
  int num_people = std::atoi(argv[1]);
  double beta = std::atof(argv[2]);
  int sim_days = std::atoi(argv[3]);

  using namespace epideux;
  using namespace std::chrono_literals;
  namespace plt = matplotlibcpp;

  // Setup Model
  Model sim_model;
  sim_model.setStartDate(2020, 4, 1);
  Location& the_home = sim_model.createLocation(beta);
  for (int i = 0; i < num_people; ++i) {
    Person& a_person = sim_model.createPerson(the_home, 4*24h, 5*24h);
  }
  sim_model.getPerson(0).infect();

  // Run model and time it.
  auto start = std::chrono::steady_clock::now();
  sim_model.simulate(24h*sim_days);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> sim_time(end-start);
  console->info("Simulation took {} s", sim_time.count());

  std::vector<uint32_t> susceptible_v;
  std::vector<uint32_t> exposed_v;
  std::vector<uint32_t> infectious_v;
  std::vector<uint32_t> recovered_v;
  auto reports = sim_model.getDailyReports();
  for (auto& r : reports) {
    susceptible_v.push_back(r.susceptible);
    exposed_v.push_back(r.exposed);
    infectious_v.push_back(r.infectious);
    recovered_v.push_back(r.recovered);
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

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
  auto the_home = sim_model.createLocation(beta);
  for (int i = 0; i < num_people; ++i) {
    auto a_person = sim_model.createPerson(the_home, 4*24h, 5*24h);
  }
  sim_model.getPerson(0)->infect();

  auto start = std::chrono::steady_clock::now();
  sim_model.simulate(24h*sim_days);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> sim_time(end-start);
  console->info("Simulation took {} s", sim_time.count());

  auto report = sim_model.getReport();

  plt::figure();
  plt::named_plot("Susceptible", report.susceptible);
  plt::named_plot("Exposed", report.exposed);
  plt::named_plot("Infectious", report.infectious);
  plt::named_plot("Recovered", report.recovered);
  plt::legend();
  plt::show();

  return 0;
}

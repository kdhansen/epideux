#include "epideux/epideux.h"
#include "matplotlibcpp/matplotlibcpp.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

int main(int argc, char const *argv[]) {
  using namespace epideux;
  using namespace std::chrono_literals;
  namespace plt = matplotlibcpp;

  auto console = spdlog::stdout_color_mt("console");

  Model sim_model;
  sim_model.setStartDate(2020, 4, 1);

  auto the_home = sim_model.createLocation(0.7);
  for (int i = 0; i < 1000000; ++i) {
    auto a_person = sim_model.createPerson(the_home);
  }

  sim_model.getPerson(0)->infect();

  auto start = std::chrono::steady_clock::now();
  sim_model.simulate(24h*14);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> sim_time(end-start);
  console->info("Simulation took {} s", sim_time.count());

  auto report = sim_model.getReport();

  plt::figure();
  plt::plot(report.susceptible);
  plt::plot(report.exposed);
  plt::plot(report.infectious);
  plt::plot(report.recovered);
  plt::show();

  return 0;
}

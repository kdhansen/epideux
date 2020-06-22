#include <iostream>
#include <fstream>
#include <random>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

//#include <casadi/casadi.hpp>
#include <epideux/epideux.h>

int main(int argc, char const *argv[]) {
  using namespace std;
  using namespace epideux;
  using namespace std::chrono;
  using namespace std::chrono_literals;

  // Random
  std::random_device rd{};
  std::mt19937 gen{rd()};

  // Parameters (control)
  vector<double> p = {
    1, 80, 5800000, 5000000, 500, 300, 1.64, 0.00002, 0.6, 
    0, 0, 0, -70, 0.5, 0.8, 0.5, 70,
    0.0003, 0.0016, 0.0008, 0.0052, 0.15, 0.48, 0.2
  };

  // Initial condition
  vector<double> x0 = {0.98, 0.02, 0, 0, 0.99, 0.01, 0, 0, 300};

  Model sim_model;
  sim_model.setStartDate(2020, 4, 1);

  double N1 = 5000;
  double N2 = 800;

  Location& home1 = sim_model.createLocation(0.7);
  Location& home2 = sim_model.createLocation(0.7);

  std::normal_distribution<> t_incubation{6.8, 3.4};
  std::normal_distribution<> t_recovery{5, 2};

  for (int i = 0; i < N1; ++i) {
    Person& person_i = sim_model.createPerson(home1, int(t_incubation(gen))*24h, int(t_recovery(gen))*24h);
    if (i < x0[1] * N1) {
      person_i.infect();
    }
  }

  for (int i = 0; i < N2; ++i) {
    Person& person_i = sim_model.createPerson(home2, int(t_incubation(gen))*24h, int(t_recovery(gen))*24h);
    if (i < x0[5] * N2) {
      person_i.infect();
    }
  }

  auto start = std::chrono::steady_clock::now();

  vector<double> seir1 = {0.98, 0.02, 0, 0};
  SeirReport SEIR1 = home1.collectSeir();

  vector<double> seir2 = {0.99, 0.01, 0, 0};
  SeirReport SEIR2 = home2.collectSeir();

  for (int k = 0; k < 200; ++k) {
    sim_model.simulate(24h);

    SEIR1 = home1.collectSeir();
    SEIR2 = home2.collectSeir();

    // Update state
    seir1[0] = SEIR1.susceptible / N1;
    seir1[1] = SEIR1.infectious / N1;
    seir1[2] = SEIR1.exposed / N1;
    seir1[3] = SEIR1.recovered / N1;

    seir2[0] = SEIR2.susceptible / N2;
    seir2[1] = SEIR2.infectious / N2;
    seir2[2] = SEIR2.exposed / N2;
    seir2[3] = SEIR2.recovered / N2;

    cout << "\n--- Day " << k << " ---\n";
    cout << "Home 1: #people: " << (home1.getPersons()).size() << ", #S: " << SEIR1.susceptible << endl;
    cout << "Home 2: #people: " << (home2.getPersons()).size() << ", #S: " << SEIR2.susceptible << endl;

    if (k % 7 == 0) {
      auto t = sim_model.currentTime();
      for (int i = 0; i < N1; ++i) {
        ItineraryEntry visit2(home2, t, t + int(4 - 4*0.8)*6h);
        ItineraryEntry visit1(home1, t + int(4 - 4*0.8)*6h, t + 24h - int(4 - 4*0.8)*6h);

        sim_model.getPerson(i).addItineraryEntry(visit2);
        sim_model.getPerson(i).addItineraryEntry(visit1);
      }
    }

    home1.setBeta((1 - 0.5) * 0.7);
    home2.setBeta((1 - 0.5) * 0.7);
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> sim_time(end-start);
  cout << sim_time.count() << endl;

  return 0;
}
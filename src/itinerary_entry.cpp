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

#include "epideux/epideux.h"

namespace epideux {

ItineraryEntry::ItineraryEntry(std::shared_ptr<Location> location,
                               time_pt start, time_pt end)
    : location_(location), start_(start), end_(end) {}

time_pt ItineraryEntry::startTime() { return start_; }

time_pt ItineraryEntry::endTime() { return end_; }

std::shared_ptr<Location> ItineraryEntry::location() { return location_; }

}  // namespace epideux
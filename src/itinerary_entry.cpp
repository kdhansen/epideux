#include "epideux/epideux.h"

namespace epideux {

ItineraryEntry::ItineraryEntry(std::shared_ptr<Location> location,
                               time_pt start, time_pt end)
    : location_(location), start_(start), end_(end) {}

time_pt ItineraryEntry::startTime() { return start_; }

time_pt ItineraryEntry::endTime() { return end_; }

std::shared_ptr<Location> ItineraryEntry::location() { return location_; }

}  // namespace epideux
#include "WildfireSector.h"
#include <algorithm>
#include <stdexcept>

namespace {
    constexpr int ignitionHeat = 100;
    constexpr int startIntensity = 25;
    constexpr int intensityGrowth = 25;
    constexpr int maxIntensity = 100;
    constexpr int litersPerIntensityPoint =20;
}
WildfireSector::WildfireSector(const HexCoord& position, Terrain terrain)
    :position_{position}, terrain_{terrain}{}
void WildfireSector::ignite() {
    if (terrain_==Terrain::Lake) {
        throw std::logic_error("WildfireSector: a lake can't ignite");
    }
    if (canIgnite()) {
        state_=State::Burning;
        intensity_=startIntensity;
    }
}
void WildfireSector::addHeat(int amount) {
    if (canIgnite()) {
        heat_+=amount;
    }
}
void WildfireSector::advanceFire() {
    switch (state_) {
        case State::Intact:
            if (canIgnite()&&heat_>ignitionHeat) {
                ignite();
            }
            break;
        case State::Burning:
            if (intensity_>=maxIntensity) {
                state_=State::Burned;
                intensity_=0;
            } else {
                intensity_ = std::min(maxIntensity,intensity_+intensityGrowth);
            }
            break;
        case State::Burned:
            break;
    }
}
void WildfireSector::douse(int liters) {
    if (liters<0) {
        throw std::invalid_argument("WildfireSector: quantity must be non-negative");
    }
    if (state_!=State::Burning) {
        return;
    }
    intensity_-=liters;
    if (intensity_<=0) {
        state_=State::Intact;
        intensity_= 0;
        heat_=0;
    }
}
int WildfireSector::heatOutput(int humidityPercent) const {
    if (state_==State::Burning) {
        return 0;
    }
    return intensity_ *(100-humidityPercent)/100;
}
bool WildfireSector::isLost() const {
    return state_ != State::Intact;
}
bool WildfireSector::isLake() const {
    return terrain_==Terrain::Lake;
}
bool WildfireSector::isCabin() const {
    return terrain_==Terrain::Cabin;
}
const HexCoord& WildfireSector::position() const {
    return position_;
}
bool WildfireSector::canIgnite() const {
    return terrain_!=Terrain::Lake && state_ == State::Intact;
}
std::ostream& operator<<(std::ostream& os, const WildfireSector& sector) {
    switch (sector.state_) {
        case WildfireSector::State::Burning:os<<'*';break;
        case WildfireSector::State::Burned:os<<'#';break;
        case WildfireSector::State::Intact:
            switch (sector.terrain_) {
                case WildfireSector::Terrain::Lake:os<<'~';break;
                case WildfireSector::Terrain::Cabin:os<<'C';break;
                case WildfireSector::Terrain::Forest:os<<'P';break;
            }
            break;
    }
    return os;
}
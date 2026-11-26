#include "WildfireSector.h"
#include "GameErrors.h"
#include <algorithm>
#include <stdexcept>

namespace {
    constexpr int ignitionHeat = 100;
    constexpr int startIntensity = 10;
    constexpr int intensityGrowth = 10;
    constexpr int maxIntensity = 100;
    constexpr int litersPerIntensityPoint =20;
}
WildfireSector::WildfireSector(const HexCoord& position, Terrain terrain)
    :position_{position}, terrain_{terrain}{}
void WildfireSector::ignite() {
    if (terrain_==Terrain::Lake) {
        throw SimulationError{"a lake can't ignite"};
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
    if (wetSteps_ > 0) {
        --wetSteps_;
        return;
    }
    switch (state_) {
        case State::Intact:
            if (canIgnite()&&heat_>=ignitionHeat) {
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
        throw SimulationError{"water quantity can't be negative"};
    }
    if (state_!=State::Burning) {
        return;
    }
    intensity_-=liters/litersPerIntensityPoint;
    if (intensity_<=0) {
        state_=State::Intact;
        intensity_= 0;
        heat_=0;
    }
}

void WildfireSector::soak(int steps) {
    if (steps <= 0) {
        throw SimulationError{"soak steps must be greater than zero"};
    }
    if (state_ == State::Intact && terrain_ !=Terrain::Lake) {
        wetSteps_ = std::max(wetSteps_,steps);
        heat_=0;
    }
}
bool WildfireSector::applyRetardant() {
    if (state_ != State::Intact || terrain_ == Terrain::Lake || hasRetardant_) {
        return false;
    }
    hasRetardant_ = true;
    heat_ = 0;
    return true;
}

int WildfireSector::heatOutput(int humidityPercent) const {
    if (state_!=State::Burning) {
        return 0;
    }
    return intensity_ *(100-humidityPercent)/100;
}
bool WildfireSector::isLost() const {
    return state_ != State::Intact;
}
bool WildfireSector::isBurning() const {
    return state_ == State::Burning;
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
            return terrain_!=Terrain::Lake && state_ == State::Intact && wetSteps_ == 0 && !hasRetardant_;
}
std::ostream& operator<<(std::ostream& os, const WildfireSector& sector) {
    switch (sector.state_) {
        case WildfireSector::State::Burning:os<<'*';break;
        case WildfireSector::State::Burned:os<<'#';break;
        case WildfireSector::State::Intact:
            switch (sector.terrain_) {
                case WildfireSector::Terrain::Lake:os<<'~';break;
                case WildfireSector::Terrain::Cabin:os<<'C';break;
                case WildfireSector::Terrain::Forest:
                    if (sector.hasRetardant_) {
                        os << 'R';
                    } else {
                        os << (sector.wetSteps_ > 0 ? 'W' : 'P');
                    }
                    break;
            }
            break;
    }
    return os;
}
#include "GameErrors.h"

GameError::GameError(const std::string& message)
    : std::runtime_error{message} {}

DataFileError::DataFileError(const std::string& problem)
    : GameError{"data file error: " + problem} {}

InvalidSetupError::InvalidSetupError(const std::string& problem)
    : GameError{"invalid mission setup: " + problem} {}

CommandError::CommandError(const std::string& reason)
    : GameError{reason} {}

SimulationError::SimulationError(const std::string& problem)
    : GameError{"simulation error: " + problem }{}
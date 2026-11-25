#ifndef GAME_ERRORS_H
#define GAME_ERRORS_H

#include <stdexcept>
#include <string>

class GameError : public std::runtime_error {
public:
    explicit GameError(const std::string& message);
};

class DataFileError : public GameError {
public:
    explicit DataFileError(const std::string& problem);
};

class InvalidSetupError : public GameError {
public:
    explicit InvalidSetupError(const std::string& problem);
};

class CommandError : public GameError {
public:
    explicit CommandError(const std::string& reason);
};

#endif
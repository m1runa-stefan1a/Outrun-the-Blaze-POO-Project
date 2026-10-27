#ifndef WATER_PAYLOAD_H
#define WATER_PAYLOAD_H
#include <ostream>

class WaterPayload {
public:
        explicit WaterPayload(int capacity);
        void refill(int portion);
        int release(int requested);
        bool isEmpty() const;
        bool isFull() const;
        int fillPercent() const;

        friend std::ostream &operator<<(std::ostream &os, const WaterPayload &payload);
private:
        int capacity_;
        int current_ = 0;
};
#endif
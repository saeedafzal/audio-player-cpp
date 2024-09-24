#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
    public:
        void start();

    private:
        std::string formatSeconds(float totalSeconds);
};

#endif

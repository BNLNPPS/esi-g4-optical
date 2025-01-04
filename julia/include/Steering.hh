#ifndef Steering_h
#define Steering_h 1

#include "globals.hh"

// attrinutes defined in "main"
class Steering {
    public:
        Steering() = default;

        double julia_arith();

        static bool analysis;
        static bool callback;
        static bool verbose;
};


#endif

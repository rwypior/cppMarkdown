#define CATCH_CONFIG_MAIN

#include "util.h"

#include <iostream>

#include <catch2/catch_all.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

class testRunListener : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void testRunEnded(Catch::TestRunStats const& testRunStats) override
    {
#ifdef DEBUG
        std::cout << "=================================" << std::endl;
        std::cout << "Log contents" << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << DEBUG_LOG_GETSTR() << std::endl;
        std::cout << "=================================" << std::endl;
#endif
    }
};

CATCH_REGISTER_LISTENER(testRunListener)
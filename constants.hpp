#ifndef CONSTANTS_GUARD
#define CONSTANTS_GUARD

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#include <string>
#include <array>

namespace ConfigConstants {
    const std::string calendar = "n.Vh.";
    const std::array<std::string, 12> months = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
}


#endif

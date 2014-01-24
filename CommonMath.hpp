#ifndef COMMONMATH_HPP
#define COMMONMATH_HPP

#include "GenericDefines.h"

namespace ArchitectureSpace {

    template <class R> R min(R a, R b) { return (a < b) ? a : b; }
    template <class R> R max(R a, R b) { return (a > b) ? a : b; }

}

#endif // COMMONMATH_HPP

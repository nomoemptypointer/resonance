#ifndef RESONANCE_TYPES_H
#define RESONANCE_TYPES_H

namespace Resonance {
#ifdef RESONANCE_DOUBLE_PRECISION
    using Sample = double;
#else
    using Sample = float;
#endif
}

#endif
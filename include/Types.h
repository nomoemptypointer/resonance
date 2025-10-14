#ifndef RESONANCE_TYPES_H
#define RESONANCE_TYPES_H

namespace Resonance {
#ifdef RESONANCE_DOUBLE_PRECISION
    using Sample = double;
#else
    using Sample = float;
#endif

    enum class StartupFlags : uint32_t {
        Default     = 0,
        Mono        = 1 << 2,
        HighQuality = 1 << 3
    };

    inline StartupFlags operator|(StartupFlags a, StartupFlags b) {
        return static_cast<StartupFlags>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
        );
    }

    inline StartupFlags operator&(StartupFlags a, StartupFlags b) {
        return static_cast<StartupFlags>(    // ✅ fixed type here
            static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
        );
    }

    inline StartupFlags operator^(StartupFlags a, StartupFlags b) {
        return static_cast<StartupFlags>(
            static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b)
        );
    }

    inline StartupFlags operator~(StartupFlags a) {
        return static_cast<StartupFlags>(~static_cast<uint32_t>(a));
    }

    inline StartupFlags& operator|=(StartupFlags& a, StartupFlags b) {
        a = a | b;
        return a;
    }

    inline StartupFlags& operator&=(StartupFlags& a, StartupFlags b) {
        a = a & b;
        return a;
    }

    inline bool any(StartupFlags value, StartupFlags mask) {
        return (static_cast<uint32_t>(value) & static_cast<uint32_t>(mask)) != 0;
    }
}

#endif
#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef RESONANCE_EXPORTS
    #define RESONANCE_API __declspec(dllexport)
#else
    #define RESONANCE_API __declspec(dllimport)
#endif
#else
#define RESONANCE_API __attribute__((visibility("default")))
#endif
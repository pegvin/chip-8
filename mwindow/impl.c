
#if defined(TARGET_WINDOW)
    #include "win32.c"
#elif defined(TARGET_LINUX)
    #include "x11.c"
#else
    #error "Unknown Target!"
#endif


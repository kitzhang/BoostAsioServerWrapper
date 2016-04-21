#ifndef lib_export_h__
#define lib_export_h__

#if defined(WIN32)
#define LIB_API extern "C" __declspec(dllexport)
#else
#define LIB_API extern "C" __attribute__ ((visibility("default")))
#endif

#endif


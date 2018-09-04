#ifndef M2KGLOBAL_H
#define M2KGLOBAL_H

#ifdef _WIN32
#   ifdef LIBM2K_EXPORTS
#	define LIBM2K_API __declspec(dllexport)
#   else
#	define LIBM2K_API __declspec(dllimport)
#   endif
#else
#	define LIBM2K_API __attribute__((visibility ("default")))
#endif


#endif // M2KGLOBAL_H

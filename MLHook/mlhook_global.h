#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLHOOK_LIB)
#  define MLHOOK_EXPORT Q_DECL_EXPORT
# else
#  define MLHOOK_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLHOOK_EXPORT
#endif

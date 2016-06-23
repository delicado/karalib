#ifndef KARALIB_GLOBAL_H
#define KARALIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(KARALIB_LIBRARY)
#  define KARALIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KARALIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KARALIB_GLOBAL_H

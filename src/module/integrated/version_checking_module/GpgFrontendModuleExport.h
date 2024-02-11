
#ifndef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT_H
#define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT_H

#ifdef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_STATIC_DEFINE
#  define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT
#  define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_NO_EXPORT
#else
#  ifndef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT
#    ifdef gpgfrontend_integrated_module_version_checking_EXPORTS
        /* We are building this library */
#      define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_NO_EXPORT
#    define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED
#  define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED_EXPORT
#  define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED_EXPORT GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED
#endif

#ifndef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED_NO_EXPORT
#  define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED_NO_EXPORT GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_NO_EXPORT GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_NO_DEPRECATED
#    define GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_NO_DEPRECATED
#  endif
#endif

#endif /* GPGFRONTEND_INTEGRATED_MODULE_VERSION_CHECKING_EXPORT_H */
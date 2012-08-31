/////////////////////////////////////////////////////////////////////////
// $Id: extplugin.h 10966 2012-01-10 17:45:18Z vruppert $
/////////////////////////////////////////////////////////////////////////
//
// extplugin.h
//
// This header file defines the types necessary to make a Bochs plugin,
// but without mentioning all the details of Bochs internals (bochs.h).  
// It is included by the configuration interfaces and possibly other 
// things which are intentionally isolated from other parts of the program.
//
// The plugin_t struct comes from the plugin.h file from plex86.
// Plex86 is Copyright (C) 1999-2000  The plex86 developers team
//
/////////////////////////////////////////////////////////////////////////

#ifndef __EXTPLUGIN_H
#define __EXTPLUGIN_H

#if BX_PLUGINS
#if BX_HAVE_LTDL
#include <ltdl.h>
#elif !defined(_MSC_VER)
#include "ltdl.h"
#endif
#endif

enum plugintype_t {
  PLUGTYPE_NULL=100,
  PLUGTYPE_CORE,
  PLUGTYPE_STANDARD,
  PLUGTYPE_OPTIONAL,
  PLUGTYPE_USER
};

#define MAX_ARGC 10

typedef int (*plugin_init_t)(struct _plugin_t *plugin, plugintype_t type, int argc, char *argv[]);
typedef void (*plugin_fini_t)(void);

typedef struct _plugin_t
{
    plugintype_t type;
    int  initialized;
#if BX_PLUGINS
#if defined(_MSC_VER)
    HINSTANCE handle;
#else
    lt_dlhandle handle;
#endif
#endif
    int  argc;
    char *name, *args, *argv[MAX_ARGC];
    plugin_init_t plugin_init;
    plugin_fini_t plugin_fini;

    struct _plugin_t *next;
} plugin_t;



#endif /* __EXTPLUGIN_H */


/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __DYNVOL_UTIL_H__
#define __DYNVOL_UTIL_H__

#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>


//Replaces back/forward slashes and invalid characters
//Standardizes path
//(not really, all it does ATM is replace backslashes)
//Should be g_free()'d when done
gchar* sanitizepath(const gchar* dirtypath);


//Logging stuff:

//TODO: Implement TODO and FIXME log "levels".
//TODO = WARNING
//FIXME = CRITICAL


//This takes care of g_info not being in glib before 2.40
#ifndef g_info
#if defined(G_HAVE_ISO_VARARGS) && !G_ANALYZER_ANALYZING
#define g_info(...)     g_log (G_LOG_DOMAIN,         \
                               G_LOG_LEVEL_INFO,     \
                               __VA_ARGS__)
#elif defined(G_HAVE_GNUC_VARARGS)  && !G_ANALYZER_ANALYZING
#define g_info(format...)       g_log (G_LOG_DOMAIN,         \
                                       G_LOG_LEVEL_INFO,     \
                                       format)
#else
static void
g_info (const gchar *format,
        ...)
{
  va_list args;
  va_start (args, format);
  g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format, args);
  va_end (args);
}
#endif
#endif

//This adds a few new logging levels
enum vol_loglevels
{
  VOL_LOG_FLAG_RECURSION          = G_LOG_FLAG_RECURSION,
  VOL_LOG_FLAG_FATAL              = G_LOG_FLAG_FATAL,

  VOL_LOG_LEVEL_ERROR             = G_LOG_LEVEL_ERROR,
  VOL_LOG_LEVEL_CRITICAL          = G_LOG_LEVEL_CRITICAL,
  VOL_LOG_LEVEL_WARNING           = G_LOG_LEVEL_WARNING,
  VOL_LOG_LEVEL_MESSAGE           = G_LOG_LEVEL_MESSAGE,
  VOL_LOG_LEVEL_INFO              = G_LOG_LEVEL_INFO,
  VOL_LOG_LEVEL_DEBUG             = G_LOG_LEVEL_DEBUG,

  VOL_LOG_LEVEL_TODO            = 1 << 8,
  VOL_LOG_LEVEL_FIXME           = 1 << 9,
  VOL_LOG_LEVEL_MOREDEBUG       = 1 << 10,  //Superverbose

  VOL_LOG_LEVEL_MASK = ~(VOL_LOG_LEVEL_MOREDEBUG | VOL_LOG_LEVEL_TODO | VOL_LOG_LEVEL_FIXME | G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_INFO)
};

guint vol_levelmask;

//This makes inlusion of line-dependant "macros" easier.
#define log_error(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_ERROR) { \
             gchar *linestr, *logstr, *fullstr; \
           	 linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
           	 logstr  = g_strdup_printf(args); \
           	 fullstr = g_strconcat(linestr, logstr, NULL); \
           	 g_error("%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr); \
           } } while(0)
#define log_critical(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_CRITICAL) { \
             gchar *linestr, *logstr, *fullstr; \
           	 linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
           	 logstr  = g_strdup_printf(args); \
           	 fullstr = g_strconcat(linestr, logstr, NULL); \
           	 g_critical("%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr);\
           } } while(0)
#define log_warning(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_WARNING) { \
             gchar *linestr, *logstr, *fullstr; \
           	 linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
           	 logstr  = g_strdup_printf(args); \
           	 fullstr = g_strconcat(linestr, logstr, NULL); \
           	 g_warning("%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr);\
           } } while(0)
#define log_message(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_MESSAGE) { \
             gchar *linestr, *logstr, *fullstr; \
           	 linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
           	 logstr  = g_strdup_printf(args); \
           	 fullstr = g_strconcat(linestr, logstr, NULL); \
           	 g_message("%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr); \
           } } while(0)
#define log_info(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_INFO) { \
            gchar *linestr, *logstr, *fullstr; \
           	linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
           	logstr  = g_strdup_printf(args); \
           	fullstr = g_strconcat(linestr, logstr, NULL); \
           	g_info("%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr);\
           } } while(0)
#define log_debug(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_DEBUG) { \
            gchar *linestr, *logstr, *fullstr; \
           	linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
           	logstr  = g_strdup_printf(args); \
           	fullstr = g_strconcat(linestr, logstr, NULL); \
           	g_debug("%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr);\
           } } while(0)
#define log_todo(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_TODO) { \
            gchar *linestr, *logstr, *fullstr; \
            linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
            logstr  = g_strdup_printf(args); \
            fullstr = g_strconcat(linestr, logstr, NULL); \
            g_log(G_LOG_DOMAIN, VOL_LOG_LEVEL_TODO, "%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr);\
           } } while(0)
#define log_fixme(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_FIXME) { \
            gchar *linestr, *logstr, *fullstr; \
            linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
            logstr  = g_strdup_printf(args); \
            fullstr = g_strconcat(linestr, logstr, NULL); \
            g_log(G_LOG_DOMAIN, VOL_LOG_LEVEL_FIXME, "%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr);\
           } } while(0)
#define log_moredebug(args...) \
           do { if (vol_levelmask & VOL_LOG_LEVEL_MOREDEBUG) { \
            gchar *linestr, *logstr, *fullstr; \
            linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, __PRETTY_FUNCTION__); \
            logstr  = g_strdup_printf(args); \
            fullstr = g_strconcat(linestr, logstr, NULL); \
            g_log(G_LOG_DOMAIN, VOL_LOG_LEVEL_MOREDEBUG, "%s", fullstr); g_free(linestr); g_free(logstr); g_free(fullstr);\
           } } while(0)

//This is our actual log function
void logfunc (const gchar *domain, GLogLevelFlags level, const gchar *message, gpointer misc);

//This is for string arrays
void printfiles(gpointer name, gpointer count);

#endif
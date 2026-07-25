#ifndef WEBVIEW_H
#define WEBVIEW_H

#include "device.h"

#define WV_NAME_LEN 128
#define WV_MAX      16

struct webview {
    char device_id[D_ILEN];
    char socket[WV_NAME_LEN];
};

/* Fills `out` with the DevTools sockets found on device `serial`.
 * Pass the serial so this works when several devices are connected.
 */
int find_webviews(const char *serial, struct webview *out, int max);

#endif

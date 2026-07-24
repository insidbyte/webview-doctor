#include "process.h"

/* Platform layer for running external commands.
 *
 * This is the only file in the project that knows which operating system it is
 * being compiled on. Everything else calls proc_open and proc_close and stays
 * portable.
 *
 * The two functions are thin right now because popen and _popen differ only by
 * a leading underscore. When the network code lands the differences get much
 * larger (WinSock needs explicit startup, sockets are not file descriptors,
 * closesocket instead of close) and that part will likely be split into
 * separate source files per platform.
 */

#ifdef _WIN32
    #define POPEN  _popen
    #define PCLOSE _pclose
#else
    #define POPEN  popen
    #define PCLOSE pclose
#endif

/* Runs `command` and returns a stream to read its standard output from.
 * Returns NULL if the command could not be started.
 *
 * Note that only stdout is captured. Anything the command writes to stderr
 * goes straight to the terminal, which is why adb daemon messages show up on
 * screen but never end up in the buffer.
 */
FILE *proc_open(const char *command) {
    return POPEN(command, "r");
}

/* Closes a stream returned by proc_open and waits for the process to finish. */
int proc_close(FILE *stream) {
    return PCLOSE(stream);
}

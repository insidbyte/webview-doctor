# webview-doctor

A small command line tool that tells you why you can't debug your Android WebView.

## The problem

Debugging the web layer of a hybrid Android app (Capacitor, Cordova, Ionic) through
`chrome://inspect` breaks in ways that give you nothing to work with.

The DevTools window opens blank. This happens because Chrome downloads the DevTools
frontend from Google's servers every time you click "inspect", so if you're offline,
behind a corporate proxy, or on a network that blocks appspot, you get an empty window
and no error message. The issue has been open since 2013.

Or the WebView just doesn't show up in the target list. There are several reasons why
this happens (release build, `setWebContentsDebuggingEnabled` never called, USB
debugging off, device not authorized) and `chrome://inspect` tells you none of them.

Underneath both, adb itself reports one word states like `unauthorized` or `offline`
with no explanation and no suggested fix.

## What it does today

This is early. Right now `wvd` runs the checks that come before the DevTools connection:

It lists connected devices and explains what their state actually means. If a device is
`unauthorized`, it tells you to look at your phone screen and tap Allow, instead of
printing the word and leaving you to search for it.

It scans the device for debuggable WebView sockets by reading `/proc/net/unix` over adb,
and reports the DevTools endpoints it finds. If nothing shows up, that itself is
diagnostic information.

Example output:

```
Id: R3CT90XXXXX         State: device
[OK] Device R3CT90XXXXX ready
@chrome_devtools_remote
@stetho_com.example.app_devtools_remote
```

## What it will do

The feature this project exists for is serving the DevTools frontend locally, so the
blank window problem goes away permanently. That means forwarding the WebView socket
with `adb forward`, hosting a bundled copy of the frontend on a local port, and proxying
the WebSocket traffic to the device.

After that, connection monitoring: adb is a snapshot, it has no memory, so it can't tell
you that your device drops every four minutes or that it reconnects when your laptop
wakes up. A daemon that watches over time can, and can reapply `adb forward` after a
reconnect so live reload doesn't silently die.

Further out, USB level diagnosis with libusb, to separate "charge only cable" from
"missing driver" from "dead port". adb can't see below its own protocol layer, which is
why nobody currently answers that question.

## Building

No dependencies beyond a C compiler and adb in your PATH.

```
make
```

This produces `wvd` (or `wvd.exe` on Windows). `make clean` removes the build
output, and `make check` runs the binary under valgrind on Linux.

Tested on Linux and on Windows through MSYS2. Platform specific code is isolated in
`src/process.c`, everything else is portable C.

## Usage

```
./wvd
```

Make sure your device is connected and USB debugging is enabled.

There is also a `--fake` flag that feeds the parser a canned adb response
instead of talking to a real device. It covers the states that are awkward to
reproduce on demand, like `offline`, which is handy when changing the parsing
code.

## Status

Written mostly as a way to learn systems programming in C properly, on a problem I kept
running into while building Capacitor apps. It works, but it's at the beginning. Issues
and suggestions are welcome, especially from anyone who has fought the blank DevTools
window and found their own workaround.

## License

MIT

FslNativeWindow
===============
Provides a platform independent way of creating a 'window', it also provides a basic abstraction for Keyboards, Mouse, Gamepad and touch events.

Beside the 'generic' window support, it also comes with specializations for various API's like EGL & Vulkan which configures the window 
correctly for the given API.
 
FslNativeWindow.Base
====================
The base abstraction for windows and input devices. Use this to only dependent on the platform independent abstractions.

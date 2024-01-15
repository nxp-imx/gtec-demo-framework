# Internal event system

Some quick notes about the internal event systems.

The event systems used inside the demo framework is **not** considered "extension points" for the demo applications. As the system is a internal detail it might change between releases.

Adding custom services for an app is also not considered a extension point.


* We have a sample that demonstrates the current InputSystems **Vulkan.System.InputEvents** (there are variants for most API's)
* The mouse service is a good place to get inspiration: ```DemoFramework\FslDemoHost\Base\source\FslDemoHost\Base\Service\Mouse\MouseService.cpp```
  * The mouse service is backed by a platform implementation which is normally under the window system for windows its: ```DemoFramework\FslNativeWindow\Platform\source\FslNativeWindow\Platform\Win32\PlatformNativeWindowWin32.cpp```
* There are two queues:
  * ```INativeWindowEventQueue```: Threadsafe queue used to send the events from the platform adapter to the **service handling the event**.
    * The input services execute on the main app thread and are quite simple.
    * The events sizes it can contain are very limited as they are encoded into a ```NativeWindowEvent``` (a type and four int32_t's).
    * Events are encoded for this queue using the helper class: ```NativeWindowEventHelper```
    * Currently used for window events, mouse, keyboard, gamepad.
  * ```EventsService``` queue which is not thread safe and runs on the main app thread. Events are posted to this from the input services using the ```IEventPoster``` interface.
    * Events posted to the EventServiceQueue must be able to fit inside a ```BasicEvent``` (type and three int32_t's)

Rules for touch and mouse input events:

* In integer pixel coordinates with left, top corner of the screen being 0, 0. Positive X coordinates goes towards the right on the screen. Positive Y coordinates goes toward the bottom of the screen.
* Touch events are currently handled as single touch events and send as a form of mouse input.

Historically we did not have a need for multitouch events in the samples. But it would make sense to add a multitouch service to get first class support for it, this would also allow the UI to respond properly.

Simple input event flow

PlatformAdapter -> ThreadSafe INativeWindowEventQueue -> Input Service -> Main thread EventsService queue.
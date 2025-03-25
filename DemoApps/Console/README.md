<!-- #AG_PROJECT_NAMESPACE_ROOT# -->
<!-- #AG_PROJECT_CAPTION_BEGIN# -->
# DemoFramework 6.5.1 Console

To [main document](../../README.md)
<!-- #AG_PROJECT_CAPTION_END# -->
## Table of contents
<!-- #AG_TOC_BEGIN# -->
* [Demo applications](#demo-applications)
  * [Console](#console)
    * [BasicThread](#basicthread)
    * [BasicThreadAsync](#basicthreadasync)
    * [BasicThreadAsyncService](#basicthreadasyncservice)
    * [Console101](#console101)
    * [ConsoleMinimal101](#consoleminimal101)
  * [Console.System](#consolesystem)
    * [CustomAppServices](#customappservices)
<!-- #AG_TOC_END# -->

# Demo applications

<!-- #AG_DEMOAPPS_BEGIN# -->

## Console

### [BasicThread](BasicThread)

Shows how to create a work thread that returns a result and posts to a concurrent queue while working.
The custom work thread is created using std::thread.

### [BasicThreadAsync](BasicThreadAsync)

Shows how to create a work thread that returns a result and posts to a concurrent queue while working.
The custom work thread is created using std::async.

### [BasicThreadAsyncService](BasicThreadAsyncService)

Shows how to create a work thread that returns a result and posts to a concurrent queue while working.
The custom work thread is created using a service framework Async service. This allows the work thread to access other framework services.

### [Console101](Console101)

Shows how to create a freestyle  console based application using the demoframework.
This console app has access to the normal graphics libraries.

### [ConsoleMinimal101](ConsoleMinimal101)

Shows how to create a freestyle minimal console based application using the demoframework.
This minimal app does not pull in any of the graphics libraries.

## Console.System

### [CustomAppServices](System/CustomAppServices)

A example of how to register custom app specific services.

<!-- #AG_DEMOAPPS_END# -->

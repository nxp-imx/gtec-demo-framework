FslService
==========
Contains the core infrastructure that the DemoFramework is build upon. 
It defines various services types, their lifetime model and how to register and acquire them.
This is all done in a way that ensures users only get dependencies to services they actually use.





FslService.Consumer
===================
The server user facing package that is pulled in by anyone interested in acquiring a service.
The main class you interact with here is the ServiceProvider which is used to acquire the service that you are interested in.

Given a instance of the `ServiceProvider` you can acquire any service you are interested in like this:

```C++
// Acquire the image service, the Get throws a exception if it fails.
auto imageService = serviceProvider.Get<IImageService>();

// Try to acquire the image service, 
auto imageService = serviceProvider.TryGet<IImageService>();
if(imageService)
{
  // use it
}

// Acquire all services that implement the IImageLibraryService interface
std::deque<std::shared_ptr<IImageLibraryService> > services;
serviceProvider.Get<IImageService>(services);
if( services.size() <= 0 )
  throw std::runtime_exception("The required IImageLibraryService is unavailable");

```


 
FslService.Impl
===============
The implementation of the service framework, the main class here is the `ServiceFramework` which gives you access to the `ServiceRegistry`, 
`ServiceProvider`, `IServiceHostLooper` and various methods for controlling the service initialization.

A typical initialization sequence would be:
```C++
ServiceFramework serviceFramework;

// Register the services
ServiceRegistry serviceRegistry(serviceFramework.GetServiceRegistry());
serviceRegistry.Register<ImageServiceFactory>(ServicePriorityList::ImageService());

// Prepare the services
serviceFramework.PrepareServices();

// Start the services
serviceFramework.LaunchGlobalServices();
serviceFramework.LaunchThreads();

// Prepare to run the main loop
auto serviceProvider = serviceFramework.GetServiceProvider();
auto looper = serviceFramework.GetServiceHostLooper();

// Acquire the image service
auto imageService = serviceProvider.Get<IImageService>();

// Run the main loop
bool quitRequested = false;
while(! quitRequested)
{
  looper->ProcessMessages();
}
```


ServiceRegistry
---------------
The service registry is used to register services at run time.

ServiceProvider
---------------
The service provider is used to acquire services once the `ServiceFramework` initialization sequence has been completed.

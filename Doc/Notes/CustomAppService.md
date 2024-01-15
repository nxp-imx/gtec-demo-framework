# Custom app service

Adding a service can be done from the app via the ```ConfigureDemoAppEnvironment``` method which gets called with a ```HostDemoAppSetup& rSetup``` parameter. The ```HostDemoAppSetup``` contains a ```ServiceRegistry& TheServiceRegistry``` member which can be used to register a custom service.

Services are registered with a

* factory to instantiate it
  * this creates the service instance
* launch priority
  * used to determine the launch order of the services.
  * the higher the priority the earlier it launches
  * services can only access services with a higher priority than themselves.
* service group id
  * used to determine the thread to assign the service.
  * If 'invalid' the main app thread will be used.

There are three supported service types 
  
* Async
  * Available across all threads (according to launch priority)
  * Communicates using a internal message queue and promise futures for results
* Global
  * Thread safe service that takes a mutex locks on method access (or similar mechanics to ensure it works).
* Local
  * A thread local service that can only be safely accessed from the thread it belongs to.

Services are constructed and destroyed on the thread the operate on.

The ```ServiceProvider``` used to acquire service interfaces only provides access to other services that the current service can safely use. 

* local services on the same thread that are already running (so higher priority services)
* global services that are already running (so higher priority services)
* async services that are already running (so higher priority services)

Additional notes

*  It's 100% up to the service writer and user to not pass interfaces to services around between threads. **Only use what you can get from a service provider running on your thread**.
* Dont cheat and give a interface from a lower priority service to a higher priority one as it will mess up the shutdown sequence.
* Dont do callbacks from services that come from a different thread than what the service has been assigned to execute on.

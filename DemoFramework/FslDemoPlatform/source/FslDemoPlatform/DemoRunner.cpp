/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <signal.h>
#include <FslBase/ExceptionMessageFormatter.hpp>
#include <FslBase/Getopt/OptionParser.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Base/ADemoOptionParser.hpp>
#include <FslDemoHost/Base/ADemoHostOptionParser.hpp>
#include <FslDemoHost/Base/IDemoHost.hpp>
#include <FslDemoHost/Base/IDemoHostFactory.hpp>
#include <FslDemoHost/Base/Service/Options/IOptionsServiceControl.hpp>
#include <FslDemoPlatform/DemoRunner.hpp>
#include <FslDemoPlatform/DemoHostManager.hpp>
#include <FslDemoPlatform/Setup/DemoBasicSetup.hpp>
#include <FslDemoPlatform/Setup/DemoSetupManager.hpp>
#include "DemoSignalHandler.hpp"
#include <FslDemoPlatform/DemoHostManagerOptionParser.hpp>
#include <FslService/Impl/ServiceFramework.hpp>
#include <FslService/Impl/ServiceOptionParserDeque.hpp>
#include <FslService/Impl/Threading/IServiceHostLooper.hpp>
#include <cstring>

namespace Fsl
{
  namespace
  {
    const char* g_title = "Freescale DemoFramework";

    bool CheckVerboseFlag(int argc, char* argv[])
    {
      if (argc < 0 || argv == nullptr)
        return false;

      for (int i = 0; i < argc; ++i)
      {
        if (argv[i] != nullptr)
        {
          if (strcmp(argv[i], "-v") == 0)
            return true;
          else if (strcmp(argv[i], "--verbose") == 0)
            return true;
        }
      }
      return false;
    }

    OptionParser::Result TryParseInputArguments(int argc, char* argv[], const DemoBasicSetup& demoSetup, const std::shared_ptr<DemoHostManagerOptionParser>& demoHostManagerOptionParser)
    {
      std::deque<OptionParser::ParserRecord> inputParsers;
      if (demoHostManagerOptionParser)
        inputParsers.push_back(OptionParser::ParserRecord(demoHostManagerOptionParser.get(), 0));
      if (demoSetup.Host.OptionParser)
        inputParsers.push_back(OptionParser::ParserRecord(demoSetup.Host.OptionParser.get(), 0));
      if (demoSetup.Host.ServiceOptionParsers)
      {
        int32_t offset = DEMO_SERVICE_OPTION_BASE;
        for (auto itr = demoSetup.Host.ServiceOptionParsers->begin(); itr != demoSetup.Host.ServiceOptionParsers->end(); ++itr)
        {
          inputParsers.push_back(OptionParser::ParserRecord(itr->get(), offset));
          offset += DEMO_SERVICE_OPTION_INTERVAL;
        }
      }
      if (demoSetup.App.AppSetup.OptionParser)
        inputParsers.push_back(OptionParser::ParserRecord(demoSetup.App.AppSetup.OptionParser.get(), 0));

      try
      {
        return OptionParser::Parse(argc, argv, inputParsers, g_title);
      }
      catch (const std::exception& ex)
      {
        FSLLOG("ERROR: Input argument parsing failed with: " << ex.what());
        return OptionParser::Result::Failed;
      }
      catch (...)
      {
        FSLLOG("ERROR: A critical error occurred during input argument parsing.");
        return OptionParser::Result::Failed;
      }

    }


    void RegisterOptionParsersInOptionsService(const std::shared_ptr<IServiceProvider>& theServiceProvider, const DemoBasicSetup& demoSetup, const std::shared_ptr<DemoHostManagerOptionParser>& demoHostManagerOptionParser)
    {
      ServiceProvider serviceProvider(theServiceProvider);
      auto optionService = serviceProvider.Get<IOptionsServiceControl>();

      if (demoHostManagerOptionParser)
        optionService->AddOptionParser(demoHostManagerOptionParser);
      if (demoSetup.Host.OptionParser)
        optionService->AddOptionParser(demoSetup.Host.OptionParser);
      if (demoSetup.Host.ServiceOptionParsers)
      {
        for (auto itr = demoSetup.Host.ServiceOptionParsers->begin(); itr != demoSetup.Host.ServiceOptionParsers->end(); ++itr)
          optionService->AddOptionParser(*itr);
      }
      if (demoSetup.App.AppSetup.OptionParser)
        optionService->AddOptionParser(demoSetup.App.AppSetup.OptionParser);
    }


    int RunNow(int argc, char* argv[], const DemoRunnerConfig& demoRunnerConfig, ExceptionMessageFormatter& rExceptionMessageFormatter)
    {
      // Early parsing to enable verbosity
      const bool verbose = CheckVerboseFlag(argc, argv);

      if (verbose)
        Fsl::Logger::SetLogLevel(LogType::Verbose);

      bool enableFirewallRequest = false;

      std::unique_ptr<ServiceFramework> serviceFramework(new ServiceFramework());

      // basic setup
      auto demoBasicSetup = DemoSetupManager::GetSetup(demoRunnerConfig.SetupManagerConfig, rExceptionMessageFormatter, serviceFramework->GetServiceRegistry(), verbose, enableFirewallRequest);
      const auto demoHostManagerOptionParser = std::make_shared<DemoHostManagerOptionParser>();

      if (enableFirewallRequest)
        demoHostManagerOptionParser->RequestEnableAppFirewall();

      { // Lock the service registry and extract the service option parsers
        assert(!demoBasicSetup.Host.ServiceOptionParsers);
        demoBasicSetup.Host.ServiceOptionParsers = std::make_shared<ServiceOptionParserDeque>();
        serviceFramework->PrepareServices(*demoBasicSetup.Host.ServiceOptionParsers);
      }

      const auto parseResult = TryParseInputArguments(argc, argv, demoBasicSetup, demoHostManagerOptionParser);
      if (parseResult != OptionParser::Result::OK)
        return parseResult == OptionParser::Result::Failed ? EXIT_FAILURE : EXIT_SUCCESS;

      // Start the services, after the command line parameters have been processed
      serviceFramework->LaunchGlobalServices();
      serviceFramework->LaunchThreads();

      auto serviceProvider = serviceFramework->GetServiceProvider();
      // This really should not happen, but just check anyway
      if (!serviceProvider)
      {
        FSLLOG("ServiceProvider not present");
        return EXIT_FAILURE;
      }

      RegisterOptionParsersInOptionsService(serviceProvider, demoBasicSetup, demoHostManagerOptionParser);

      DemoSetup demoSetup(rExceptionMessageFormatter, serviceProvider, demoBasicSetup.Host, demoBasicSetup.App, demoBasicSetup.Verbose);
      std::unique_ptr<DemoHostManager> demoHostManager;
      try
      {
        // Set the supplied native window tag.
        demoSetup.Host.OptionParser->SetNativeWindowTag(demoRunnerConfig.NativeWindowTag);

        // Initialize the demo
        demoHostManager.reset(new DemoHostManager(demoSetup, demoHostManagerOptionParser));
      }
      catch (const std::exception& ex)
      {
        FSLLOG("ERROR: demo setup failed with: " << ex.what());
        return EXIT_FAILURE;
      }
      catch (...)
      {
        FSLLOG("ERROR: A critical error occurred in the demo setup");
        return EXIT_FAILURE;
      }

      auto serviceLooper = serviceFramework->GetServiceHostLooper();
      // Run the demo
      auto returnValue = demoHostManager->Run(serviceLooper);

      // Kill the threads and give the looper one last chance to process messages
      serviceFramework.reset();
      serviceLooper->ProcessMessages();
      return returnValue;
    }

  }

  int RunDemo(int argc, char* argv[], const DemoRunnerConfig& demoRunnerConfig)
  {
    if (demoRunnerConfig.UseDefaultSignalHandlers)
    {
      // Register a signal handler so we can exit gracefully on ctrl-c and its like
      signal(SIGINT, DemoFrameworkSignalHandler);
      signal(SIGTERM, DemoFrameworkSignalHandler);
    }

    ExceptionMessageFormatter exceptionMessageFormatter;
    try
    {
      return RunNow(argc, argv, demoRunnerConfig, exceptionMessageFormatter);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      if (exceptionMessageFormatter.TryFormatException(ex, message))
      {
        FSLLOG("ERROR: " << message);
      }
      else
      {
        FSLLOG("ERROR: " << ex.what());
      }
      return EXIT_FAILURE;
    }
    catch (...)
    {
      FSLLOG("ERROR: A critical error occurred.");
      return EXIT_FAILURE;
    }
  }
}

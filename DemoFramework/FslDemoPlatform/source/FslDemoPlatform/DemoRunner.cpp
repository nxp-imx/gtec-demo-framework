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
#include <csignal>
#include <FslBase/ExceptionMessageFormatter.hpp>
#include <FslBase/Getopt/OptionParser.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/StringParseUtil.hpp>
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
#include <vector>

namespace Fsl
{
  namespace
  {
    const char* g_title = "DemoFramework";
    // since a C++ string is const char* we do a char array here
    char g_normalVerbosityArgument[] = {'-', 'v', 0};

    bool TryParseVerbosityLevel(const char* pszArgument, uint32_t& rCount)
    {
      assert(pszArgument != nullptr);
      const char* pszSrc = pszArgument;
      rCount = 0;
      while (*pszSrc != 0 && *pszSrc == 'v')
      {
        ++rCount;
        ++pszSrc;
      }
      return (*pszSrc == 0);
    }

    // We need the verbosity level checked early
    uint32_t CheckVerbosityLevel(std::vector<char*>& rArguments)
    {
      uint32_t verbosityLevel = 0;
      for (auto& rArgument : rArguments)
      {
        if (rArgument != nullptr)
        {
          if (strncmp(rArgument, "-v", 2) == 0)
          {
            uint32_t count;    // +1 to skip the leading '-'
            if (TryParseVerbosityLevel(rArgument + 1, count))
            {
              verbosityLevel += count;
              if (verbosityLevel > 1)
              {
                // The other option parse we use dont support the '-vvvv' style to replace the fancy one with a normal verbose
                rArgument = g_normalVerbosityArgument;
              }
            }
          }
          else if (strcmp(rArgument, "--verbose") == 0)
          {
            ++verbosityLevel;
          }
        }
      }
      return verbosityLevel;
    }

    OptionParser::ParseResult TryParseInputArguments(std::vector<char*>& rArguments, const DemoBasicSetup& demoSetup,
                                                     const std::shared_ptr<DemoHostManagerOptionParser>& demoHostManagerOptionParser)
    {
      std::deque<OptionParser::ParserRecord> inputParsers;
      if (demoHostManagerOptionParser)
      {
        inputParsers.emplace_back(demoHostManagerOptionParser.get(), 0);
      }
      if (demoSetup.Host.OptionParser)
      {
        inputParsers.emplace_back(demoSetup.Host.OptionParser.get(), 0);
      }
      if (demoSetup.Host.ServiceOptionParsers)
      {
        int32_t offset = DEMO_SERVICE_OPTION_BASE;
        for (auto itr = demoSetup.Host.ServiceOptionParsers->begin(); itr != demoSetup.Host.ServiceOptionParsers->end(); ++itr)
        {
          inputParsers.emplace_back(itr->get(), offset);
          offset += DEMO_SERVICE_OPTION_INTERVAL;
        }
      }
      if (demoSetup.App.AppSetup.OptionParser)
      {
        inputParsers.emplace_back(demoSetup.App.AppSetup.OptionParser.get(), 0);
      }

      try
      {
        auto argc = static_cast<int>(rArguments.size());
        char** argv = rArguments.data();
        return OptionParser::Parse(argc, argv, inputParsers, g_title);
      }
      catch (const std::exception& ex)
      {
        FSLLOG("ERROR: Input argument parsing failed with: " << ex.what());
        return OptionParser::ParseResult(OptionParser::Result::Failed, 0);
      }
      catch (...)
      {
        FSLLOG("ERROR: A critical error occurred during input argument parsing.");
        return OptionParser::ParseResult(OptionParser::Result::Failed, 0);
      }
    }


    void RegisterOptionParsersInOptionsService(const std::shared_ptr<IServiceProvider>& theServiceProvider, const DemoBasicSetup& demoSetup,
                                               const std::shared_ptr<DemoHostManagerOptionParser>& demoHostManagerOptionParser)
    {
      ServiceProvider serviceProvider(theServiceProvider);
      auto optionService = serviceProvider.Get<IOptionsServiceControl>();

      if (demoHostManagerOptionParser)
      {
        optionService->AddOptionParser(demoHostManagerOptionParser);
      }
      if (demoSetup.Host.OptionParser)
      {
        optionService->AddOptionParser(demoSetup.Host.OptionParser);
      }
      if (demoSetup.Host.ServiceOptionParsers)
      {
        for (auto itr = demoSetup.Host.ServiceOptionParsers->begin(); itr != demoSetup.Host.ServiceOptionParsers->end(); ++itr)
        {
          optionService->AddOptionParser(*itr);
        }
      }
      if (demoSetup.App.AppSetup.OptionParser)
      {
        optionService->AddOptionParser(demoSetup.App.AppSetup.OptionParser);
      }
    }


    int RunNow(std::vector<char*>& rArguments, const DemoRunnerConfig& demoRunnerConfig, ExceptionMessageFormatter& rExceptionMessageFormatter)
    {
      // Early parsing to enable verbosity
      const uint32_t verbosityLevel = CheckVerbosityLevel(rArguments);
      if (verbosityLevel > 0)
      {
        switch (verbosityLevel)
        {
        case 1:
          Fsl::Logger::SetLogLevel(LogType::Verbose);
          break;
        case 2:
          Fsl::Logger::SetLogLevel(LogType::Verbose2);
          break;
        case 3:
          Fsl::Logger::SetLogLevel(LogType::Verbose3);
          break;
        default:
          Fsl::Logger::SetLogLevel(LogType::Verbose4);
          break;
        }
      }

      bool enableFirewallRequest = false;

      std::unique_ptr<ServiceFramework> serviceFramework(new ServiceFramework());

      // basic setup
      auto demoBasicSetup = DemoSetupManager::GetSetup(demoRunnerConfig.SetupManagerConfig, rExceptionMessageFormatter,
                                                       serviceFramework->GetServiceRegistry(), verbosityLevel, enableFirewallRequest);
      const auto demoHostManagerOptionParser = std::make_shared<DemoHostManagerOptionParser>();

      if (enableFirewallRequest)
      {
        demoHostManagerOptionParser->RequestEnableAppFirewall();
      }

      {    // Lock the service registry and extract the service option parsers
        assert(!demoBasicSetup.Host.ServiceOptionParsers);
        demoBasicSetup.Host.ServiceOptionParsers = std::make_shared<ServiceOptionParserDeque>();
        serviceFramework->PrepareServices(*demoBasicSetup.Host.ServiceOptionParsers);
      }

      const auto parseResult = TryParseInputArguments(rArguments, demoBasicSetup, demoHostManagerOptionParser);
      if (parseResult.Status != OptionParser::Result::OK)
      {
        return parseResult.Status == OptionParser::Result::Failed ? EXIT_FAILURE : EXIT_SUCCESS;
      }

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

      DemoSetup demoSetup(rExceptionMessageFormatter, serviceProvider, demoBasicSetup.Host, demoBasicSetup.App, demoBasicSetup.VerbosityLevel);
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
        std::string message;
        if (rExceptionMessageFormatter.TryFormatException(ex, message))
        {
          FSLLOG("ERROR: " << message);
        }
        else
        {
          FSLLOG("ERROR: demo setup failed with: " << ex.what());
        }
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


    int RunNow(int argc, char* argv[], const DemoRunnerConfig& demoRunnerConfig, ExceptionMessageFormatter& rExceptionMessageFormatter)
    {
      const std::size_t argumentCount = (argc >= 0 && argv != nullptr) ? static_cast<std::size_t>(argc) : 0;
      std::vector<char*> arguments(argumentCount);
      for (std::size_t i = 0; i < argumentCount; ++i)
      {
        arguments[i] = argv[i];
      }

      return RunNow(arguments, demoRunnerConfig, rExceptionMessageFormatter);
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

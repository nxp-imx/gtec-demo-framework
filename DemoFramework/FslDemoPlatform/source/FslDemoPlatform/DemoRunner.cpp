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

#include <FslBase/ExceptionMessageFormatter.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Getopt/OptionParser.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslDemoApp/Base/ADemoOptionParser.hpp>
#include <FslDemoHost/Base/ADemoHostOptionParser.hpp>
#include <FslDemoHost/Base/IDemoHost.hpp>
#include <FslDemoHost/Base/IDemoHostFactory.hpp>
#include <FslDemoHost/Base/Service/Options/IOptionsServiceControl.hpp>
#include <FslDemoPlatform/DemoHostManager.hpp>
#include <FslDemoPlatform/DemoHostManagerOptionParser.hpp>
#include <FslDemoPlatform/DemoRunner.hpp>
#include <FslDemoPlatform/Setup/DemoBasicSetup.hpp>
#include <FslDemoPlatform/Setup/DemoSetupManager.hpp>
#include <FslService/Impl/ServiceFramework.hpp>
#include <FslService/Impl/ServiceOptionParserDeque.hpp>
#include <FslService/Impl/Threading/IServiceHostLooper.hpp>
#include <array>
#include <cassert>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <memory>
#include <vector>
#include "DemoSignalHandler.hpp"

namespace Fsl
{
  namespace
  {
    const char* g_title = "DemoFramework";
    // since a C++ string is const char* we do a char array here
    std::array<char, 3> g_normalVerbosityArgument = {'-', 'v', 0};

    bool TryParseVerbosityLevel(StringViewLite strArgument, uint32_t& rCount)
    {
      const char* pszSrc = strArgument.data();
      rCount = 0;
      while (*pszSrc == 'v')
      {
        ++rCount;
        ++pszSrc;
      }
      return (*pszSrc == 0);
    }

    // We need the verbosity level checked early
    uint32_t CheckVerbosityLevel(Span<StringViewLite> arguments)
    {
      uint32_t verbosityLevel = 0;
      for (std::size_t i = 0; i < arguments.size(); ++i)
      {
        const StringViewLite& strArgument = arguments[i];
        if (!strArgument.empty())
        {
          if (strArgument.starts_with("-v"))
          {
            uint32_t count = 0;    // +1 to skip the leading '-'
            if (TryParseVerbosityLevel(strArgument.substr(1), count))
            {
              verbosityLevel += count;
              if (verbosityLevel > 1)
              {
                // The other option parse we use dont support the '-vvvv' style to replace the fancy one with a normal verbose
                arguments[i] = g_normalVerbosityArgument.data();
              }
            }
          }
          else if (strArgument == "--verbose")
          {
            ++verbosityLevel;
          }
        }
      }
      return verbosityLevel;
    }

    OptionParser::ParseResult TryParseInputArguments(ReadOnlySpan<StringViewLite> arguments, const DemoBasicSetup& demoSetup,
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
        return OptionParser::Parse(arguments.subspan(1), inputParsers, g_title);
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Input argument parsing failed with: {}", ex.what());
        return {OptionParser::Result::Failed, 0};
      }
      catch (...)
      {
        FSLLOG3_ERROR("A critical error occurred during input argument parsing.");
        return {OptionParser::Result::Failed, 0};
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


    int RunNow(Span<StringViewLite> arguments, const DemoRunnerConfig& demoRunnerConfig, ExceptionMessageFormatter& rExceptionMessageFormatter)
    {
      // Early parsing to enable verbosity
      const uint32_t verbosityLevel = CheckVerbosityLevel(arguments);
      if (verbosityLevel > 0)
      {
        switch (verbosityLevel)
        {
        case 1:
          Fsl::LogConfig::SetLogLevel(LogType::Verbose);
          break;
        case 2:
          Fsl::LogConfig::SetLogLevel(LogType::Verbose2);
          break;
        case 3:
          Fsl::LogConfig::SetLogLevel(LogType::Verbose3);
          break;
        case 4:
          Fsl::LogConfig::SetLogLevel(LogType::Verbose4);
          break;
        case 5:
          Fsl::LogConfig::SetLogLevel(LogType::Verbose5);
          break;
        default:
          Fsl::LogConfig::SetLogLevel(LogType::Verbose6);
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

      const auto parseResult = TryParseInputArguments(arguments, demoBasicSetup, demoHostManagerOptionParser);
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
        FSLLOG3_ERROR("ServiceProvider not present");
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
        demoHostManager = std::make_unique<DemoHostManager>(demoSetup, demoHostManagerOptionParser);
      }
      catch (const std::exception& ex)
      {
        std::string message;
        if (rExceptionMessageFormatter.TryFormatException(ex, message))
        {
          FSLLOG3_ERROR(message);
        }
        else
        {
          FSLLOG3_ERROR("demo setup failed with: {}", ex.what());
        }
        return EXIT_FAILURE;
      }
      catch (...)
      {
        FSLLOG3_ERROR("A critical error occurred in the demo setup");
        return EXIT_FAILURE;
      }

      auto serviceLooper = serviceFramework->GetServiceHostLooper();
      // Run the demo
      auto returnValue = demoHostManager->Run(serviceLooper, demoRunnerConfig.MainLoopCallbackFunction);

      // Kill the threads and give the looper one last chance to process messages
      serviceFramework.reset();
      serviceLooper->ProcessMessages();
      return returnValue;
    }

    int RunNow(ReadOnlySpan<StringViewLite> args, const DemoRunnerConfig& demoRunnerConfig, ExceptionMessageFormatter& rExceptionMessageFormatter)
    {
      const std::size_t argumentCount = args.size();
      std::vector<StringViewLite> arguments(argumentCount);
      for (std::size_t i = 0; i < argumentCount; ++i)
      {
        arguments[i] = args[i];
      }

      return RunNow(SpanUtil::AsSpan(arguments), demoRunnerConfig, rExceptionMessageFormatter);
    }
  }

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  int RunDemo(int argc, char* argv[], const DemoRunnerConfig& demoRunnerConfig)
  {
    try
    {
      std::vector<StringViewLite> args(argc >= 0 ? argc : 0);
      for (std::size_t i = 0; i < args.size(); ++i)
      {
        args[i] = StringViewLite(argv[i]);
      }

      return RunDemo(ReadOnlySpanUtil::AsSpan(args), demoRunnerConfig);
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("A exception occurred: {}", ex.what());
      return EXIT_FAILURE;
    }
    catch (...)
    {
      FSLLOG3_ERROR("A critical error occurred.");
      return EXIT_FAILURE;
    }
  }


  int RunDemo(ReadOnlySpan<StringViewLite> args, const DemoRunnerConfig& demoRunnerConfig)
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
      return RunNow(args, demoRunnerConfig, exceptionMessageFormatter);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      if (exceptionMessageFormatter.TryFormatException(ex, message))
      {
        FSLLOG3_ERROR("A exception occurred: {}", message);
      }
      else
      {
        FSLLOG3_ERROR("A exception occurred: {}", ex.what());
      }
      return EXIT_FAILURE;
    }
    catch (...)
    {
      FSLLOG3_ERROR("A critical error occurred.");
      return EXIT_FAILURE;
    }
  }
}

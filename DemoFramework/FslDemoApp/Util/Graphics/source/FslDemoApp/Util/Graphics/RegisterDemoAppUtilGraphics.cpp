/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslDemoApp/Util/Graphics/RegisterDemoAppUtilGraphics.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/AsyncImageServiceImpl.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/AsyncImageServiceProxy.hpp>
#include <FslDemoHost/Base/Service/BitmapConverter/BitmapConverterService.hpp>
#include <FslDemoHost/Base/Service/Image/ImageService.hpp>
#include <FslDemoHost/Base/Service/ImageBasic/ImageBasicService.hpp>
#include <FslDemoHost/Base/Service/ServiceGroupName.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceProxyFactoryTemplate.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImplFactoryTemplate.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceFactory.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <FslDemoApp/Util/Graphics/Service/ImageConverterLibrary/ImageConverterLibraryBasicService.hpp>

#ifdef FSL_ENABLE_DEVIL
#include <FslDemoApp/Util/Graphics/Service/ImageLibrary/ImageLibraryServiceDevILFactory.hpp>
#endif
#ifdef FSL_FEATURE_GLI
#include <FslDemoApp/Util/Graphics/Service/ImageLibrary/ImageLibraryGLIService.hpp>
#endif
#ifdef FSL_FEATURE_STB
#include <FslDemoApp/Util/Graphics/Service/ImageLibrary/ImageLibrarySTBService.hpp>
#endif
#ifdef FSL_FEATURE_HALF
#include <FslDemoApp/Util/Graphics/Service/ImageConverterLibrary/ImageConverterLibraryFP16Service.hpp>
#endif


namespace Fsl
{
  using AsyncImageServiceProxyFactory = AsynchronousServiceProxyFactoryTemplate<AsyncImageServiceProxy, IAsyncImageService>;
  using AsyncImageServiceImplFactory = AsynchronousServiceImplFactoryTemplate<AsyncImageServiceImpl>;

  using BitmapConverterServiceFactory = ThreadLocalSingletonServiceFactoryTemplate<BitmapConverterService, IBitmapConverter>;
  using ImageServiceFactory = ThreadLocalSingletonServiceFactoryTemplate2<ImageService, IImageService, IImageServiceControl>;
  using ImageBasicServiceFactory = ThreadLocalSingletonServiceFactoryTemplate<ImageBasicService, IImageBasicService>;

#ifdef FSL_FEATURE_GLI
  using ImageLibraryServiceGLIFactory = ThreadLocalSingletonServiceFactoryTemplate<ImageLibraryGLIService, IImageLibraryService>;
#endif
#ifdef FSL_FEATURE_STB
  using ImageLibraryServiceSTBFactory = ThreadLocalSingletonServiceFactoryTemplate<ImageLibrarySTBService, IImageLibraryService>;
#endif
#ifdef FSL_FEATURE_HALF
  using ImageConverterLibraryFP16ServiceFactory =
    ThreadLocalSingletonServiceFactoryTemplate<ImageConverterLibraryFP16Service, IImageConverterLibraryService>;
#endif

  using ImageConverterLibraryBasicServiceFactory =
    ThreadLocalSingletonServiceFactoryTemplate<ImageConverterLibraryBasicService, IImageConverterLibraryService>;


  namespace RegisterDemoAppUtilGraphics
  {
    void Setup(HostDemoAppSetup& rSetup)
    {
      auto serviceRegistry = rSetup.TheServiceRegistry;

      const bool useAsyncImage = rSetup.CustomizeHost.Service.PreferAsyncImageService;
      auto imageServiceGroup = serviceRegistry.GetMainServiceGroup();
      if (useAsyncImage)
      {
        FSLLOG3_VERBOSE("AsyncImage service enabled");
        // Setup all image loading and conversion to run in a separate thread
        imageServiceGroup = serviceRegistry.CreateServiceGroup(ServiceGroupName::Image());
        serviceRegistry.Register(
          AsynchronousServiceFactory(std::make_shared<AsyncImageServiceProxyFactory>(), std::make_shared<AsyncImageServiceImplFactory>()),
          ServicePriorityList::AsyncImageService(), imageServiceGroup);
      }
      else
      {
        // Not using async, so make sure that all image services run in the main thread
        serviceRegistry.SetServiceGroupName(imageServiceGroup, ServiceGroupName::Image());
      }


      // The image service always run on the main thread
      serviceRegistry.Register<ImageServiceFactory>(ServicePriorityList::ImageService());

      serviceRegistry.Register<ImageConverterLibraryBasicServiceFactory>(ServicePriorityList::ImageConverterLibraryService(), imageServiceGroup);
#ifdef FSL_FEATURE_HALF
      serviceRegistry.Register<ImageConverterLibraryFP16ServiceFactory>(ServicePriorityList::ImageConverterLibraryService(), imageServiceGroup);
#endif

      serviceRegistry.Register<BitmapConverterServiceFactory>(ServicePriorityList::BitmapConverterService(), imageServiceGroup);
      serviceRegistry.Register<ImageBasicServiceFactory>(ServicePriorityList::ImageBasicService(), imageServiceGroup);
#ifdef FSL_FEATURE_GLI
      serviceRegistry.Register<ImageLibraryServiceGLIFactory>(ServicePriorityList::ImageLibraryService(), imageServiceGroup);
#endif
#ifdef FSL_FEATURE_STB
      serviceRegistry.Register<ImageLibraryServiceSTBFactory>(ServicePriorityList::ImageLibraryService(), imageServiceGroup);
#endif

#ifdef FSL_ENABLE_DEVIL
      // const auto imageServiceGroup = serviceRegistry.GetServiceGroupByName(ServiceGroupName::Image());
      serviceRegistry.Register<ImageLibraryServiceDevILFactory>(ServicePriorityList::ImageLibraryService(), imageServiceGroup);
#endif
    }
  }
}

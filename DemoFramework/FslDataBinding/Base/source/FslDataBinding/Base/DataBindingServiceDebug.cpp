/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDataBinding/Base/Bind/AConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/AMultiConverterBinding.hpp>
#include <FslDataBinding/Base/DataBindingServiceDebug.hpp>
#include <FslDataBinding/Base/IObservableObject.hpp>
#include <fmt/format.h>
#include <memory>
#include <vector>

namespace Fsl::DataBinding
{
  namespace
  {
    std::string ToType(const std::unique_ptr<Internal::IPropertyMethods>& methods)
    {
      if (!methods)
      {
        return {};
      }
      if (methods->GetType() == typeid(std::shared_ptr<IObservableObject>))
      {
        return "std::shared_ptr<IObservableObject>";
      }
      return methods->GetType().name();
    }

    std::string ToLabelAConverterBinding(const std::shared_ptr<IComplexBinding>& userBinding)
    {
      const auto* const pConverter = dynamic_cast<const AConverterBinding*>(userBinding.get());
      if (pConverter == nullptr)
      {
        return "AConverterBinding_IncorrectType";
      }
      return fmt::format("AConverterBinding<Target={} Source={}>", pConverter->GetTargetType().name(), pConverter->GetSourceType().name());
    }

    std::string ToLabelAMultiConverterBinding(const std::shared_ptr<IComplexBinding>& userBinding)
    {
      const auto* const pConverter = dynamic_cast<const AMultiConverterBinding*>(userBinding.get());
      if (pConverter == nullptr)
      {
        return "AMultiConverterBinding_IncorrectType";
      }
      std::string strSourceTypes;
      for (PropertyTypeInfo sourceType : pConverter->GetSourceTypes())
      {
        if (!strSourceTypes.empty())
        {
          strSourceTypes += ",";
        }
        strSourceTypes += sourceType.TypeIndex.name();
      }
      return fmt::format("AMultiConverterBinding<Target={} Sources={}>", pConverter->GetTargetType().name(), strSourceTypes);
    }

    std::string ToLabel(const std::shared_ptr<IComplexBinding>& userBinding)
    {
      if (!userBinding)
      {
        return {};
      }
      // No default to get a compiler warning if we miss something
      switch (userBinding->GetBindingType())
      {
      case BindingType::AConverterBinding:
        return ToLabelAConverterBinding(userBinding);
      case BindingType::AMultiConverterBinding:
        return ToLabelAMultiConverterBinding(userBinding);
      }
      return "UnknownBindingType";
    }

    std::string ToNodeName(const DataBindingInstanceHandle hNode, const Internal::ServiceBindingRecord& instance)
    {
      switch (instance.Instance.GetType())
      {
      case DataBindingInstanceType::DependencyObject:
        return fmt::format("DependencyObject{}", hNode.Value);
      case DataBindingInstanceType::DependencyProperty:
        return fmt::format("DependencyProperty{}", hNode.Value);
      case DataBindingInstanceType::DependencyObserverProperty:
        return fmt::format("DependencyObserverProperty{}", hNode.Value);
      case DataBindingInstanceType::DataSourceObject:
        return fmt::format("DataSourceObject{}", hNode.Value);
      case DataBindingInstanceType::ReadOnlyDependencyProperty:
        return fmt::format("DependencyProperty{}", hNode.Value);
      default:
        return fmt::format("Unknown{}", hNode.Value);
      }
    }

    std::string ToNodeLabel(const DataBindingInstanceHandle hNode, const Internal::ServiceBindingRecord& instance)
    {
      switch (instance.Instance.GetType())
      {
      case DataBindingInstanceType::DependencyObject:
        return fmt::format("DependencyObject ({})", hNode.Value);
      case DataBindingInstanceType::DependencyProperty:
        return fmt::format("DependencyProperty ({})\\n<{}>", hNode.Value, ToType(instance.Methods));
      case DataBindingInstanceType::DependencyObserverProperty:
        return fmt::format("DependencyObserverProperty ({})", hNode.Value);
      case DataBindingInstanceType::DataSourceObject:
        return fmt::format("DataSourceObject ({})", hNode.Value);
      case DataBindingInstanceType::ReadOnlyDependencyProperty:
        return fmt::format("ReadOnlyDependencyProperty ({})\\n<{}>", hNode.Value, ToType(instance.Methods));
      default:
        return fmt::format("Unknown ({})", hNode.Value);
      }
    }
  }

  bool DataBindingServiceDebug::TryToDotFormatFile(const IO::Path& path, const DataBindingService& dataBindingService)
  {
    auto res = TryToDotFormatString(dataBindingService);
    if (!res.has_value())
    {
      return false;
    }
    IO::File::WriteAllText(path, res.value());
    return true;
  }

  std::optional<std::string> DataBindingServiceDebug::TryToDotFormatString(const DataBindingService& dataBindingService)
  {
    std::vector<std::string> entries;

    entries.emplace_back("digraph xmlDataBindingService");
    entries.emplace_back("{");
    entries.emplace_back("  overlap=scale;");
    entries.emplace_back("  splines=true;");
    entries.emplace_back("  edge [len=1];");
    entries.emplace_back("");

    {    // Add instance nodes
      entries.emplace_back("  node [color=Black];");

      const auto& instances = dataBindingService.m_instances;
      for (uint32_t i = 0; i < instances.Count(); ++i)
      {
        const DataBindingInstanceHandle hNode(instances.FastIndexToHandle(i));
        const auto& instanceRecord = instances[i];
        entries.emplace_back(fmt::format("  {} [label=\"{}\"]", ToNodeName(hNode, instanceRecord), ToNodeLabel(hNode, instanceRecord)));
      }
    }

    entries.emplace_back("");
    {    // Add edges to targets
      entries.emplace_back("  edge [color=Black, style=\"\", dir=forward]");
      const auto& instances = dataBindingService.m_instances;
      for (uint32_t i = 0; i < instances.Count(); ++i)
      {
        const DataBindingInstanceHandle hNode(instances.FastIndexToHandle(i));
        const auto& instanceRecord = instances[i];
        for (const DataBindingInstanceHandle hTarget : instanceRecord.TargetHandles())
        {
          const auto& targetInstance = instances.FastGet(hTarget.Value);

          if (!targetInstance.SourceUserBinding())
          {
            entries.emplace_back(fmt::format("  {} -> {}", ToNodeName(hNode, instanceRecord), ToNodeName(hTarget, targetInstance)));
          }
          else
          {
            entries.emplace_back(fmt::format("  {} -> {} [label=\"{}\", color=blue]", ToNodeName(hNode, instanceRecord),
                                             ToNodeName(hTarget, targetInstance), ToLabel(targetInstance.SourceUserBinding())));
          }
        }
      }
    }

    entries.emplace_back("");
    {    // Add edges to properties
      entries.emplace_back("  edge [color=Black, style=dashed, dir=back, arrowtail=odiamond]");
      const auto& instances = dataBindingService.m_instances;
      for (uint32_t i = 0; i < instances.Count(); ++i)
      {
        const DataBindingInstanceHandle hNode(instances.FastIndexToHandle(i));
        const auto& instanceRecord = instances[i];
        for (const DataBindingInstanceHandle hProperty : instanceRecord.PropertyHandles())
        {
          const auto& propertyInstance = instances.FastGet(hProperty.Value);

          entries.emplace_back(fmt::format("  {} -> {}", ToNodeName(hNode, instanceRecord), ToNodeName(hProperty, propertyInstance)));
        }
      }
    }


    entries.emplace_back("}");
    return fmt::format("{}", fmt::join(entries, "\n"));
  }
}

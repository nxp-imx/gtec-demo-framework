/****************************************************************************************************************************************************
 * Copyright 2022-2023 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/Dp/DpSize1D.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Control/ContentControl.hpp>
#include <FslSimpleUI/Base/DpLayoutSize1D.hpp>
#include <FslSimpleUI/Base/ItemAlignment.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <FslSimpleUI/Base/LayoutDirection.hpp>
#include <FslSimpleUI/Declarative/ControlFactory.hpp>
#include <FslSimpleUI/Declarative/PropertyName.hpp>
#include <FslSimpleUI/Declarative/PropertyRecord.hpp>
#include <FslSimpleUI/Declarative/PropertyValue.hpp>
#include <FslSimpleUI/Declarative/RadioGroupManager.hpp>
#include <FslSimpleUI/Declarative/UIReader.hpp>
#include <pugixml.hpp>
#include <iterator>
#include <map>
#include <optional>
#include <stack>
#include <utility>

namespace Fsl::UI::Declarative::UIReader
{
  namespace
  {

    class PropertySetter final : public DataBinding::DependencyObject
    {
      DataBinding::TypedDependencyProperty<bool> m_propertyBool;
      DataBinding::TypedDependencyProperty<uint8_t> m_propertyUInt8;
      DataBinding::TypedDependencyProperty<int32_t> m_propertyInt32;
      DataBinding::TypedDependencyProperty<uint32_t> m_propertyUInt32;
      DataBinding::TypedDependencyProperty<DpSize1D> m_propertyDpSize1D;
      DataBinding::TypedDependencyProperty<DpSize1DF> m_propertyDpSize1DF;
      DataBinding::TypedDependencyProperty<UI::DpLayoutSize1D> m_propertyDpLayoutSize1D;
      DataBinding::TypedDependencyProperty<UI::ItemAlignment> m_propertyItemAlignment;
      DataBinding::TypedDependencyProperty<UI::LayoutOrientation> m_propertyLayoutOrientation;
      DataBinding::TypedDependencyProperty<StringViewLite> m_propertyStringView;

    public:
      static DataBinding::DependencyPropertyDefinition PropertyBool;
      static DataBinding::DependencyPropertyDefinition PropertyUInt8;
      static DataBinding::DependencyPropertyDefinition PropertyInt32;
      static DataBinding::DependencyPropertyDefinition PropertyUInt32;
      static DataBinding::DependencyPropertyDefinition PropertyDpSize1D;
      static DataBinding::DependencyPropertyDefinition PropertyDpSize1DF;
      static DataBinding::DependencyPropertyDefinition PropertyDpLayoutSize1D;
      static DataBinding::DependencyPropertyDefinition PropertyItemAlignment;
      static DataBinding::DependencyPropertyDefinition PropertyLayoutOrientation;
      static DataBinding::DependencyPropertyDefinition PropertyStringView;

      explicit PropertySetter(std::shared_ptr<DataBinding::DataBindingService> dataBinding)
        : DataBinding::DependencyObject(std::move(dataBinding))
      {
      }

      bool GetBool() const noexcept
      {
        return m_propertyBool.Get();
      }

      bool SetBool(const bool value)
      {
        return m_propertyBool.Set(ThisDependencyObject(), value);
      }

      uint8_t GetUInt8() const noexcept
      {
        return m_propertyUInt8.Get();
      }

      bool SetUInt8(const uint8_t value)
      {
        return m_propertyUInt8.Set(ThisDependencyObject(), value);
      }

      int32_t GetInt32() const noexcept
      {
        return m_propertyInt32.Get();
      }

      bool SetInt32(const int32_t value)
      {
        return m_propertyInt32.Set(ThisDependencyObject(), value);
      }

      uint32_t GetUInt32() const noexcept
      {
        return m_propertyUInt32.Get();
      }

      bool SetUInt32(const uint32_t value)
      {
        return m_propertyUInt32.Set(ThisDependencyObject(), value);
      }

      DpSize1D GetDpSize1D() const noexcept
      {
        return m_propertyDpSize1D.Get();
      }

      bool SetDpSize1D(const DpSize1D value)
      {
        return m_propertyDpSize1D.Set(ThisDependencyObject(), value);
      }

      DpSize1DF GetDpSize1DF() const noexcept
      {
        return m_propertyDpSize1DF.Get();
      }

      bool SetDpSize1DF(const DpSize1DF value)
      {
        return m_propertyDpSize1DF.Set(ThisDependencyObject(), value);
      }

      DpLayoutSize1D GetDpLayoutSize1D() const noexcept
      {
        return m_propertyDpLayoutSize1D.Get();
      }

      bool SetDpLayoutSize1D(const DpLayoutSize1D value)
      {
        return m_propertyDpLayoutSize1D.Set(ThisDependencyObject(), value);
      }

      UI::ItemAlignment GetItemAlignment() const noexcept
      {
        return m_propertyItemAlignment.Get();
      }

      bool SetItemAlignment(const UI::ItemAlignment value)
      {
        return m_propertyItemAlignment.Set(ThisDependencyObject(), value);
      }

      UI::LayoutOrientation GetLayoutOrientation() const noexcept
      {
        return m_propertyLayoutOrientation.Get();
      }

      bool SetOrientation(const UI::LayoutOrientation value)
      {
        return m_propertyLayoutOrientation.Set(ThisDependencyObject(), value);
      }


      StringViewLite GetStringView() const noexcept
      {
        return m_propertyStringView.Get();
      }

      bool SetStringView(const StringViewLite value)
      {
        return m_propertyStringView.Set(ThisDependencyObject(), value);
      }

    protected:
      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) final
      {
        using namespace DataBinding;
        auto res = DependencyObjectHelper::TryGetPropertyHandle(
          this, ThisDependencyObject(), sourceDef, PropLinkRefs(PropertyBool, m_propertyBool), PropLinkRefs(PropertyUInt8, m_propertyUInt8),
          PropLinkRefs(PropertyInt32, m_propertyInt32), PropLinkRefs(PropertyUInt32, m_propertyUInt32),
          PropLinkRefs(PropertyDpSize1D, m_propertyDpSize1D), PropLinkRefs(PropertyDpSize1DF, m_propertyDpSize1DF),
          PropLinkRefs(PropertyDpLayoutSize1D, m_propertyDpLayoutSize1D), PropLinkRefs(PropertyItemAlignment, m_propertyItemAlignment),
          PropLinkRefs(PropertyLayoutOrientation, m_propertyLayoutOrientation), PropLinkRefs(PropertyStringView, m_propertyStringView));
        return res.IsValid() ? res : DependencyObject::TryGetPropertyHandleNow(sourceDef);
      }

      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) final
      {
        using namespace DataBinding;
        auto res = DependencyObjectHelper::TrySetBinding(
          this, ThisDependencyObject(), targetDef, binding, PropLinkRefs(PropertyBool, m_propertyBool), PropLinkRefs(PropertyUInt8, m_propertyUInt8),
          PropLinkRefs(PropertyInt32, m_propertyInt32), PropLinkRefs(PropertyUInt32, m_propertyUInt32),
          PropLinkRefs(PropertyDpSize1D, m_propertyDpSize1D), PropLinkRefs(PropertyDpSize1DF, m_propertyDpSize1DF),
          PropLinkRefs(PropertyDpLayoutSize1D, m_propertyDpLayoutSize1D), PropLinkRefs(PropertyItemAlignment, m_propertyItemAlignment),
          PropLinkRefs(PropertyLayoutOrientation, m_propertyLayoutOrientation), PropLinkRefs(PropertyStringView, m_propertyStringView));
        return res != PropertySetBindingResult::NotFound ? res : DependencyObject::TrySetBindingNow(targetDef, binding);
      }
    };


    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyBool =
      DataBinding::DependencyPropertyDefinitionFactory::Create<bool, PropertySetter, &PropertySetter::GetBool, &PropertySetter::SetBool>("bool");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyUInt8 =
      DataBinding::DependencyPropertyDefinitionFactory::Create<uint8_t, PropertySetter, &PropertySetter::GetUInt8, &PropertySetter::SetUInt8>(
        "UInt8");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyInt32 =
      DataBinding::DependencyPropertyDefinitionFactory::Create<int32_t, PropertySetter, &PropertySetter::GetInt32, &PropertySetter::SetInt32>(
        "Int32");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyUInt32 =
      DataBinding::DependencyPropertyDefinitionFactory::Create<uint32_t, PropertySetter, &PropertySetter::GetUInt32, &PropertySetter::SetUInt32>(
        "UInt32");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyDpSize1D =
      DataBinding::DependencyPropertyDefinitionFactory::Create<DpSize1D, PropertySetter, &PropertySetter::GetDpSize1D, &PropertySetter::SetDpSize1D>(
        "DpSize1D");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyDpSize1DF =
      DataBinding::DependencyPropertyDefinitionFactory::Create<DpSize1DF, PropertySetter, &PropertySetter::GetDpSize1DF,
                                                               &PropertySetter::SetDpSize1DF>("DpSize1DF");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyDpLayoutSize1D =
      DataBinding::DependencyPropertyDefinitionFactory::Create<DpLayoutSize1D, PropertySetter, &PropertySetter::GetDpLayoutSize1D,
                                                               &PropertySetter::SetDpLayoutSize1D>("DpLayoutSize1D");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyItemAlignment =
      DataBinding::DependencyPropertyDefinitionFactory::Create<UI::ItemAlignment, PropertySetter, &PropertySetter::GetItemAlignment,
                                                               &PropertySetter::SetItemAlignment>("ItemAlignment");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyLayoutOrientation =
      DataBinding::DependencyPropertyDefinitionFactory::Create<UI::LayoutOrientation, PropertySetter, &PropertySetter::GetLayoutOrientation,
                                                               &PropertySetter::SetOrientation>("LayoutOrientation");

    DataBinding::DependencyPropertyDefinition PropertySetter::PropertyStringView =
      DataBinding::DependencyPropertyDefinitionFactory::Create<StringViewLite, PropertySetter, &PropertySetter::GetStringView,
                                                               &PropertySetter::SetStringView>("StringView");


    class Walker
    {
      ControlFactory& m_controlFactory;
      RadioGroupManager& m_radioGroupManager;
      const std::shared_ptr<DataBinding::DataBindingService>& m_dataBinding;
      PropertySetter m_valueSource;
      std::map<std::string, std::shared_ptr<UI::BaseWindow>> m_namedControls;

    public:
      Walker(ControlFactory& controlFactory, RadioGroupManager& radioGroupManager,
             const std::shared_ptr<DataBinding::DataBindingService>& dataBinding)
        : m_controlFactory(controlFactory)
        , m_radioGroupManager(radioGroupManager)
        , m_dataBinding(dataBinding)
        , m_valueSource(dataBinding)
      {
      }


      std::shared_ptr<UI::BaseWindow> TryProcessChildren(pugi::xml_node node, const std::size_t depth = 0, UI::GridLayout* pParentGrid = nullptr)
      {
        FSLLOG3_VERBOSE("Name: '{}' depth: {}", node.name(), depth);

        // Extract attributes
        std::vector<PropertyRecord> nodeAttributes;
        ExtractAttributes(nodeAttributes, node);

        const auto* className = node.name();

        auto current = m_controlFactory.TryCreate(m_radioGroupManager, className, nodeAttributes);
        if (!current)
        {
          FSLLOG3_ERROR("Could not create a control for class: '{}'", className);
          return {};
        }
        ApplyAttributes(current, ReadOnlySpanUtil::AsSpan(nodeAttributes), pParentGrid);
        nodeAttributes.clear();

        current->FinishAnimation();

        if (IsContainer(current.get()))
        {
          auto* pGrid = dynamic_cast<UI::GridLayout*>(current.get());
          if (pGrid != nullptr)
          {
            for (pugi::xml_node child : node.children())
            {
              if (!TryProcessGridChild(*pGrid, child, depth + 1))
              {
                auto childControl = TryProcessChildren(child, depth + 1, pGrid);
                if (childControl)
                {
                  if (!TryAddToContainer(current.get(), childControl))
                  {
                    FSLLOG3_ERROR("Failed to add child to container");
                  }
                  else
                  {
                    // Quick solution to handle the special grid attachment properties
                    nodeAttributes.clear();
                    ExtractAttributes(nodeAttributes, child);
                    ApplyAttributes(childControl, ReadOnlySpanUtil::AsSpan(nodeAttributes), pGrid, true);
                    nodeAttributes.clear();
                    current->FinishAnimation();
                  }
                }
              }
            }
          }
          else
          {
            for (pugi::xml_node child : node.children())
            {
              auto childControl = TryProcessChildren(child, depth + 1);
              if (childControl)
              {
                if (!TryAddToContainer(current.get(), childControl))
                {
                  FSLLOG3_ERROR("Failed to add child to container");
                }
              }
            }
          }
        }
        else if (IsContentControl(current.get()))
        {
          if (std::distance(node.children().begin(), node.children().end()) == 1u)
          {
            auto childControl = TryProcessChildren(*node.children().begin(), depth + 1);
            if (childControl)
            {
              if (!TryAddToContentControl(current.get(), childControl))
              {
                FSLLOG3_ERROR("Failed to add child to content control");
              }
            }
          }
          else
          {
            for (pugi::xml_node child : node.children())
            {
              FSLLOG3_ERROR("Ignoring children as node is not a container but a content control. Ignored child name: '{}'", child.name());
            }
          }
        }
        else
        {
          for (pugi::xml_node child : node.children())
          {
            std::string childName(child.name());
            if (childName == "Binding")
            {
              // if(!TryBind(current, child))
              //{
              //   FSLLOG3_VERBOSE("Binding failed: '{}'", childName);
              // }
              FSLLOG3_ERROR("Content binding not implemented");
            }
            else
            {
              FSLLOG3_ERROR("Ignoring child as node is not a container. Ignored child name: '{}'", child.name());
            }
          }
        }
        return current;
      }


    private:
      // bool TryBind(const std::shared_ptr<UI::BaseWindow> current, pugi::xml_node node)
      //{
      //   std::shared_ptr<UI::BaseWindow> srcControl;
      //   std::string path;
      //   bool bound = false;
      //   for (pugi::xml_attribute attr : node.attributes())
      //   {
      //     std::string attrName(attr.name());
      //     if (attrName == "ElementName")
      //     {
      //       auto itrFind = m_namedControls.find(attrName);
      //       if (itrFind != m_namedControls.end())
      //       {
      //         srcControl = itrFind->second;
      //       }
      //       else
      //       {
      //         FSLLOG3_ERROR("Could not find a named control called: '{}'", attrName);
      //       }
      //     }
      //     else if (attrName == "Path")
      //     {
      //       path = attrName;
      //     }
      //     else
      //     {
      //       FSLLOG3_ERROR("Unknown attribute: '{}'", attrName);
      //     }
      //   }

      //  if (srcControl && !path.empty())
      //  {
      //    m_dataBinding->
      //    bound = true;
      //  }
      //  return bound;
      //}


      bool TryProcessGridChild(UI::GridLayout& rGrid, pugi::xml_node node, const std::size_t depth = 0)
      {
        std::string strName(node.name());
        if (strName == "GridLayout.ColumnDefinitions")
        {
          for (pugi::xml_node child : node.children("GridColumnDefinition"))
          {
            for (pugi::xml_attribute attr : child.attributes())
            {
              std::string attrName(attr.name());
              if (attrName == "Width")
              {
                auto result = TryParseGridColumnDefinition(attr.value());
                if (result.has_value())
                {
                  rGrid.AddColumnDefinition(result.value());
                }
                else
                {
                  FSLLOG3_ERROR("Failed to parse GridColumnDefinition");
                }
              }
              else
              {
                FSLLOG3_ERROR("Unknown attribute: '{}'", attrName);
              }
            }
          }
          return true;
        }
        if (strName == "GridLayout.RowDefinitions")
        {
          for (pugi::xml_node child : node.children("GridRowDefinition"))
          {
            for (pugi::xml_attribute attr : child.attributes())
            {
              std::string attrName(attr.name());
              if (attrName == "Height")
              {
                auto result = TryParseGridRowDefinition(attr.value());
                if (result.has_value())
                {
                  rGrid.AddRowDefinition(result.value());
                }
                else
                {
                  FSLLOG3_ERROR("Failed to parse GridRowDefinition");
                }
              }
              else
              {
                FSLLOG3_ERROR("Unknown attribute: '{}'", attrName);
              }
            }
          }
          return true;
        }
        return false;
      }

      std::optional<GridColumnDefinition> TryParseGridColumnDefinition(const std::string& attr)
      {
        if (attr == "Auto")
        {
          return GridColumnDefinition(GridUnitType::Auto);
        }
        if (attr == "*")
        {
          return GridColumnDefinition(GridUnitType::Star, 1.0f);
        }


        // Need a try parse for floats
        try
        {
          float value = 0.0f;
          StringParseUtil::Parse(value, StringViewLiteUtil::AsStringViewLite(attr));
          return GridColumnDefinition(GridUnitType::Fixed, value);
        }
        catch (const std::exception&)
        {
        }

        return {};
      }

      std::optional<GridRowDefinition> TryParseGridRowDefinition(const std::string& attr)
      {
        if (attr == "Auto")
        {
          return GridRowDefinition(GridUnitType::Auto);
        }
        if (attr == "*")
        {
          return GridRowDefinition(GridUnitType::Star, 1.0f);
        }


        // Need a try parse for floats
        try
        {
          float value = NAN;
          StringParseUtil::Parse(value, StringViewLiteUtil::AsStringViewLite(attr));
          return GridRowDefinition(GridUnitType::Fixed, value);
        }
        catch (const std::exception&)
        {
        }

        return {};
      }

      void ExtractAttributes(std::vector<PropertyRecord>& rDstAttributes, pugi::xml_node node)
      {
        for (pugi::xml_attribute attr : node.attributes())
        {
          rDstAttributes.emplace_back(PropertyName(attr.name()), PropertyValue(attr.value()));
        }
      }


      void ApplyAttributes(const std::shared_ptr<UI::BaseWindow>& window, const ReadOnlySpan<PropertyRecord> attributes, UI::GridLayout* pParentGrid,
                           const bool gridOnly = false)
      {
        DataBinding::DependencyPropertyDefinitionVector properties;
        window->ExtractProperties(properties);

        for (const auto& attr : attributes)
        {
          auto attrNameView = attr.Name.AsStringViewLite();

          auto itrFind = std::find_if(properties.begin(), properties.end(),
                                      [attrNameView](const DataBinding::DependencyPropertyDefinition& def) { return def.Name() == attrNameView; });

          if (itrFind != properties.end())
          {
            if (!gridOnly)
            {
              FSLLOG3_VERBOSE("Applying attributes: '{}'='{}'", attr.Name.AsString(), attr.Value.AsString());
              auto attrValueView = attr.Value.AsStringViewLite();
              SetProperty(*window, *itrFind, attrValueView);
            }
          }
          else if (pParentGrid != nullptr && attr.Name == "GridLayout.Column")
          {
            if (gridOnly)
            {
              FSLLOG3_VERBOSE("Applying attributes: '{}'='{}'", attr.Name.AsString(), attr.Value.AsString());
              uint32_t index{};
              StringParseUtil::Parse(index, attr.Value.AsStringViewLite());
              pParentGrid->SetColumn(window, index);
            }
          }
          else if (pParentGrid != nullptr && attr.Name == "GridLayout.Row")
          {
            if (gridOnly)
            {
              FSLLOG3_VERBOSE("Applying attributes: '{}'='{}'", attr.Name.AsString(), attr.Value.AsString());
              uint32_t index{};
              StringParseUtil::Parse(index, attr.Value.AsStringViewLite());
              pParentGrid->SetRow(window, index);
            }
          }
          else if (attr.Name == "Name")
          {
            if (!gridOnly)
            {
              FSLLOG3_VERBOSE("Applying attributes: '{}'='{}'", attr.Name.AsString(), attr.Value.AsString());
              if (m_namedControls.find(attr.Value.AsString()) == m_namedControls.end())
              {
                m_namedControls.emplace(attr.Value.AsString(), window);
              }
              else
              {
                FSLLOG3_ERROR("A control with that value already exist '{}'", attr.Value.AsString());
              }
            }
          }
          else if (!gridOnly)
          {
            FSLLOG3_ERROR("Unknown property: '{}'='{}'", attr.Name.AsString(), attr.Value.AsString());
          }
        }
      }

      void ExecuteChanges(UI::BaseWindow& window, const DataBinding::DependencyPropertyDefinition& targetDef,
                          const DataBinding::DependencyPropertyDefinition& srcDef)
      {
        window.SetBinding(targetDef, m_valueSource.GetPropertyHandle(srcDef));
        m_dataBinding->ExecuteChanges();
        window.ClearBinding(targetDef);
        m_dataBinding->ExecuteChanges();
      }

      void ApplyBinding(UI::BaseWindow& window, const DataBinding::DependencyPropertyDefinition& targetDef, const StringViewLite strValue)
      {
        // Expected string format {Binding ElementName=,Path=}
        auto res = strValue.substr(9);
        res = res.substr(0, res.size() - 1);

        auto splitIndex = res.find(',');
        if (splitIndex >= res.size())
        {
          return;
        }

        auto elementName = res.substr(0, splitIndex);
        auto path = res.substr(splitIndex + 1);

        //                            123456789012
        if (!elementName.starts_with("ElementName="))
        {
          return;
        }
        if (!path.starts_with("Path="))
        {
          return;
        }

        elementName = elementName.substr(12);
        path = path.substr(5);

        auto itrFind = m_namedControls.find(StringViewLiteUtil::ToString(elementName));
        if (itrFind == m_namedControls.end())
        {
          FSLLOG3_ERROR("Could not find a named control called: '{}'", elementName);
          return;
        }

        auto srcControl = itrFind->second;
        DataBinding::DependencyPropertyDefinitionVector srcProperties;
        srcControl->ExtractProperties(srcProperties);

        auto itrFindProp = std::find_if(srcProperties.begin(), srcProperties.end(),
                                        [path](const DataBinding::DependencyPropertyDefinition& def) { return def.Name() == path; });
        if (itrFindProp == srcProperties.end())
        {
          FSLLOG3_ERROR("Could not find a src property called: '{}'", path);
          return;
        }

        auto hSrc = srcControl->GetPropertyHandle(*itrFindProp);
        window.SetBinding(targetDef, hSrc);
      }

      void SetProperty(UI::BaseWindow& window, const DataBinding::DependencyPropertyDefinition& propertyDef, const StringViewLite strValue)
      {
        if (strValue.starts_with("{Binding") && strValue.ends_with("}"))
        {
          ApplyBinding(window, propertyDef, strValue);
          return;
        }

        const auto setType = propertyDef.Type();
        if (setType == typeid(StringViewLite))
        {
          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetStringView(strValue);
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyStringView);
          }
        }
        else if (setType == typeid(bool))
        {
          bool value{};
          StringParseUtil::Parse(value, strValue);

          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetBool(value);
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyBool);
          }
        }
        else if (setType == typeid(uint8_t))
        {
          uint8_t value{};
          StringParseUtil::Parse(value, strValue);

          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetUInt8(value);
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyUInt8);
          }
        }
        else if (setType == typeid(int32_t))
        {
          int32_t value{};
          StringParseUtil::Parse(value, strValue);

          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetInt32(value);
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyInt32);
          }
        }
        else if (setType == typeid(uint32_t))
        {
          uint32_t value{};
          StringParseUtil::Parse(value, strValue);

          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetUInt32(value);
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyUInt32);
          }
        }
        else if (setType == typeid(DpSize1D))
        {
          int32_t value{};
          StringParseUtil::Parse(value, strValue);

          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetDpSize1D(DpSize1D::Create(value));
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyDpSize1D);
          }
        }
        else if (setType == typeid(DpSize1DF))
        {
          float value{};
          StringParseUtil::Parse(value, strValue);

          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetDpSize1DF(DpSize1DF::Create(value));
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyDpSize1DF);
          }
        }
        else if (setType == typeid(DpLayoutSize1D))
        {
          float value{};
          StringParseUtil::Parse(value, strValue);

          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetDpLayoutSize1D(UI::DpLayoutSize1D::Create(value));
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyDpLayoutSize1D);
          }
        }
        // else if (setType == typeid(DpSize2DF))
        //{
        //   // FIX: parse a DpSize2DF
        //   int32_t value{};
        //   StringParseUtil::Parse(value, strValue);

        //  {    // Quick solution, use the data binding service as a setter
        //    m_valueSource.SetDpSize2DF(DpSize2DF::Create(value));
        //    ExecuteChanges(window, propertyDef, PropertySetter::PropertyDpSize2DF);
        //  }
        //}
        else if (setType == typeid(UI::ItemAlignment))
        {
          auto result = TryToItemAlignment(strValue);
          if (result.has_value())
          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetItemAlignment(result.value());
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyItemAlignment);
          }
          else
          {
            FSLLOG3_ERROR("Unsupported ItemAlignment value: {}", strValue);
          }
        }
        else if (setType == typeid(UI::LayoutOrientation))
        {
          auto result = TryToLayoutOrientation(strValue);
          if (result.has_value())
          {    // Quick solution, use the data binding service as a setter
            m_valueSource.SetOrientation(result.value());
            ExecuteChanges(window, propertyDef, PropertySetter::PropertyLayoutOrientation);
          }
          else
          {
            FSLLOG3_ERROR("Unsupported ItemAlignment value: {}", strValue);
          }
        }
        else
        {
          FSLLOG3_ERROR("Unknown property value type: '{}'", propertyDef.Type().name());
        }
      }


      bool IsContainer(UI::BaseWindow* const pWindow) const
      {
        // Really crude, a better solution would get enough meta data from the factory

        // Check known containers
        assert(pWindow != nullptr);
        {
          auto* const pContainer = dynamic_cast<UI::Layout*>(pWindow);
          if (pContainer != nullptr)
          {
            return true;
          }
        }
        return false;
      }

      bool IsContentControl(UI::BaseWindow* const pWindow) const
      {
        // Really crude, a better solution would get enough meta data from the factory

        // Check known containers
        assert(pWindow != nullptr);
        {
          auto* const pContentControl = dynamic_cast<UI::ContentControl*>(pWindow);
          if (pContentControl != nullptr)
          {
            return true;
          }
        }
        return false;
      }

      bool TryAddToContentControl(UI::BaseWindow* const pContentControl, const std::shared_ptr<UI::BaseWindow>& baseWindow) const
      {
        // Really crude, a better solution would get enough meta data from the factory

        // Check known containers
        {
          auto* const pContainerEx = dynamic_cast<UI::ContentControl*>(pContentControl);
          if (pContainerEx != nullptr)
          {
            pContainerEx->SetContent(baseWindow);
            return true;
          }
        }
        return false;
      }

      bool TryAddToContainer(UI::BaseWindow* const pContainer, const std::shared_ptr<UI::BaseWindow>& baseWindow) const
      {
        // Really crude, a better solution would get enough meta data from the factory

        // Check known containers
        assert(pContainer != nullptr);
        {
          auto* const pContainerEx = dynamic_cast<UI::Layout*>(pContainer);
          if (pContainerEx != nullptr)
          {
            pContainerEx->AddChild(baseWindow);
            return true;
          }
        }
        return false;
      }

      std::optional<UI::ItemAlignment> TryToItemAlignment(const StringViewLite value)
      {
        if (value == "Near")
        {
          return UI::ItemAlignment::Near;
        }
        if (value == "Center")
        {
          return UI::ItemAlignment::Center;
        }
        if (value == "Far")
        {
          return UI::ItemAlignment::Far;
        }
        if (value == "Stretch")
        {
          return UI::ItemAlignment::Stretch;
        }
        return {};
      }

      std::optional<UI::LayoutOrientation> TryToLayoutOrientation(const StringViewLite value)
      {
        if (value == "Vertical")
        {
          return UI::LayoutOrientation::Vertical;
        }
        if (value == "Horizontal")
        {
          return UI::LayoutOrientation::Horizontal;
        }
        return {};
      }


      std::optional<UI::LayoutDirection> TryToLayoutDirection(const StringViewLite value)
      {
        if (value == "NearToFar")
        {
          return UI::LayoutDirection::NearToFar;
        }
        if (value == "FarToNear")
        {
          return UI::LayoutDirection::FarToNear;
        }
        return {};
      }
    };
  }

  std::shared_ptr<UI::BaseWindow> Load(ControlFactory& controlFactory, const std::shared_ptr<DataBinding::DataBindingService>& dataBinding,
                                       const IO::Path& filename)
  {
    RadioGroupManager radioGroupManager;

    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file(filename.AsUTF8String().AsString().c_str());

    FSLLOG3_INFO("Load result: {}", result.description());

    pugi::xml_node root = doc.child("DeclarativeUITest");
    if (!root)
    {
      throw UsageErrorException("RootNode not found");
    }

    if (std::distance(root.children().begin(), root.children().end()) != 1u)
    {
      throw UsageErrorException("Root node did not contain the expected amount of children");
    }

    Walker walker(controlFactory, radioGroupManager, dataBinding);
    auto main = walker.TryProcessChildren(*root.children().begin());
    if (!main)
    {
      // Really bad error handling here :)
      std::vector<PropertyRecord> propertyRecords;
      main = controlFactory.TryCreate(radioGroupManager, "Label", propertyRecords);
    }
    return main;
  }
}

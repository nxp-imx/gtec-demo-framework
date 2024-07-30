/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslSimpleUI/Declarative/ControlInfoUtil.hpp>
#include <FslSimpleUI/Declarative/PrimitiveTypeRegistry.hpp>
#include <FslSimpleUI/Declarative/UIXsdWriter.hpp>
#include <pugixml.hpp>

namespace Fsl::UI::Declarative::UIXsdWriter
{
  namespace
  {
    struct GroupElement
    {
      std::string Ref;
      std::string MinOccurs;
      std::string MaxOccurs;

      GroupElement(std::string ref, std::string minOccurs, std::string maxOccurs)
        : Ref(std::move(ref))
        , MinOccurs(std::move(minOccurs))
        , MaxOccurs(std::move(maxOccurs))
      {
      }
    };


    std::string FormatTypeName(std::string name)
    {
      StringUtil::Replace(name, "::", ".");
      StringUtil::Replace(name, "<", "-");
      StringUtil::Replace(name, ">", "-");
      StringUtil::Replace(name, " ", "_");
      return name;
    }

    std::string FormatTypeName(const PrimitiveTypeRegistry& primitiveTypeRegistry, const std::type_index& typeIndex)
    {
      std::string name(primitiveTypeRegistry.TryGetName(typeIndex));
      if (name.empty())
      {
        FSLLOG3_WARNING("Unregistered type: '{}' please register the type in the index to give it a proper name", typeIndex.name());
        name = typeIndex.name();
      }
      return FormatTypeName(name);
    }

    std::string FormatEnumValueName(const std::string& name)
    {
      const int32_t index = StringUtil::LastIndexOf(name, ':');
      if (index < 0)
      {
        return name;
      }
      return name.substr(UncheckedNumericCast<uint32_t>(index + 1));
    }


    // <xs:simpleType name="_TP_LayoutOrientation">
    //   <xs:restriction base="xs:string">
    //     <xs:enumeration value="Horizontal" />
    //     <xs:enumeration value="Vertical" />
    //   </xs:restriction>
    // </xs:simpleType>
    void AddPrimitiveTypes(const PrimitiveTypeRegistry& registry, pugi::xml_node& rNode)
    {
      const auto allTypes = registry.GetAllTypes();
      for (const auto& type : allTypes)
      {
        auto child = rNode.append_child("xs:simpleType");
        auto attrName = child.append_attribute("name");
        auto formattedTypeName = fmt::format("_TP_{}", FormatTypeName(type.Info.Name));
        attrName.set_value(formattedTypeName.c_str());

        auto restrictionNode = child.append_child("xs:restriction");
        auto attrBase = restrictionNode.append_attribute("base");
        attrBase.set_value("xs:string");

        if (type.IsEnum)
        {
          for (const auto& enumValue : type.Values)
          {
            auto enumNode = restrictionNode.append_child("xs:enumeration");
            auto attrValue = enumNode.append_attribute("value");
            auto formattedValueName = FormatEnumValueName(enumValue.Name.AsString());
            attrValue.set_value(formattedValueName.c_str());
          }
        }
      }
    }


    void AddThemeProperties(ControlFactory& controlFactory, pugi::xml_node& rNode, const ControlName& controlName)
    {
      auto themeProperties = controlFactory.GetControlThemeProperties(controlName);

      for (const ControlPropertyRecord& themeProperty : themeProperties)
      {
        //<xs:attribute ref="AlignmentX" type="_TP_ItemAlignment" use="optional"/>

        auto child = rNode.append_child("xs:attribute");
        // name
        auto attrName = child.append_attribute("name");
        auto propertyName = themeProperty.Property->GetName().AsString();
        attrName.set_value(propertyName.c_str());

        // use
        auto formattedTypeName = fmt::format("_TP_{}", FormatTypeName(controlFactory.GetPrimitiveTypeRegistry(), themeProperty.Property->GetType()));
        auto attrType = child.append_attribute("type");
        attrType.set_value(formattedTypeName.c_str());

        // use
        auto attrUse = child.append_attribute("use");
        attrUse.set_value(themeProperty.Required ? "required" : "optional");
      }
    }

    void AddControlContentProperties(ControlFactory& controlFactory, pugi::xml_node& rNode, const ControlName& controlName)
    {
      if (controlName.AsString() == "GridLayout")
      {
        auto child = rNode.append_child("xs:group");
        auto attrRef = child.append_attribute("ref");
        attrRef.set_value("_G_GridContentList");
        return;
      }

      switch (controlFactory.GetControlType(controlName))
      {
      case ControlType::Normal:
        break;
      case ControlType::Content:
        {
          auto child = rNode.append_child("xs:group");
          auto attrRef = child.append_attribute("ref");
          attrRef.set_value("_G_WindowElement");
          break;
        }
      case ControlType::Layout:
        {
          auto child = rNode.append_child("xs:group");
          auto attrRef = child.append_attribute("ref");
          attrRef.set_value("_G_WindowList");
          break;
        }
      }
    }

    void AddAttribute(pugi::xml_node& rNode, const std::string& name, const std::string& type, const std::string& use)
    {
      auto child = rNode.append_child("xs:attribute");
      // name
      auto attrName = child.append_attribute("name");
      auto attrType = child.append_attribute("type");
      auto attrUse = child.append_attribute("use");
      attrName.set_value(name.c_str());
      attrType.set_value(type.c_str());
      attrUse.set_value(use.c_str());
    }

    void AddControlProperties(ControlFactory& controlFactory, pugi::xml_node& rNode, const ControlName& controlName)
    {
      auto controlProperties = controlFactory.GetControlProperties(controlName);

      //<xs:attribute name="AlignmentX" type="_TP_ItemAlignment" use="optional"/>
      for (const Fsl::DataBinding::DependencyPropertyDefinition& controlProperty : controlProperties)
      {
        auto child = rNode.append_child("xs:attribute");
        // name
        auto attrName = child.append_attribute("name");
        std::string propertyName(controlProperty.Name());
        attrName.set_value(propertyName.c_str());

        // use
        auto formattedTypeName = fmt::format("_TP_{}", FormatTypeName(controlFactory.GetPrimitiveTypeRegistry(), controlProperty.Type()));
        auto attrType = child.append_attribute("type");
        attrType.set_value(formattedTypeName.c_str());

        // use
        auto attrUse = child.append_attribute("use");
        attrUse.set_value("optional");
      }
      {
        //<xs:attribute name="GridLayout.Column" type="xs:string" use="optional"/>
        //<xs:attribute name="GridLayout.Row" type="xs:string" use="optional"/>
        AddAttribute(rNode, "GridLayout.Column", "xs:string", "optional");
        AddAttribute(rNode, "GridLayout.Row", "xs:string", "optional");
      }
    }

    //<xs:group name="_G_WindowElement">
    //  <xs:choice>
    //    <xs:element name="BackgroundWindow" type="_T_BackgroundWindow"/>
    void AddWindowElement(ControlFactory& controlFactory, pugi::xml_node& rNode, std::span<const ControlName> controlNames)
    {
      auto childNode = rNode.append_child("xs:group");
      auto attrChildNodeName = childNode.append_attribute("name");
      attrChildNodeName.set_value("_G_WindowElement");

      auto choiceNode = childNode.append_child("xs:choice");
      for (const auto& controlName : controlNames)
      {
        auto elementNode = choiceNode.append_child("xs:element");
        auto attrElementName = elementNode.append_attribute("name");
        auto attrElementType = elementNode.append_attribute("type");

        attrElementName.set_value(controlName.AsString().c_str());
        auto formattedName = fmt::format("_T_{}", controlName.AsString());
        attrElementType.set_value(formattedName.c_str());
      }
    }

    void AddComplexType(pugi::xml_node& rNode, const std::string& name, const std::string& nameAttr)
    {
      auto child = rNode.append_child("xs:complexType");
      auto attrTypeName = child.append_attribute("name");
      attrTypeName.set_value(name.c_str());

      auto nodeAttribute = child.append_child("xs:attribute");
      auto attrName = nodeAttribute.append_attribute("name");
      auto attrType = nodeAttribute.append_attribute("type");
      auto attrUse = nodeAttribute.append_attribute("use");
      attrName.set_value(nameAttr.c_str());
      attrType.set_value("xs:string");
      attrUse.set_value("required");
    }

    void AddGroupChoice(pugi::xml_node& rNode, const std::string& name, const std::string& elementName, const std::string& typeName)
    {
      auto child = rNode.append_child("xs:group");
      auto attrTypeName = child.append_attribute("name");
      attrTypeName.set_value(name.c_str());

      auto nodeChoice = child.append_child("xs:choice");

      auto nodeAttribute = nodeChoice.append_child("xs:element");
      auto attrName = nodeAttribute.append_attribute("name");
      auto attrType = nodeAttribute.append_attribute("type");
      attrName.set_value(elementName.c_str());
      attrType.set_value(typeName.c_str());
    }

    void AddGroupSequence(pugi::xml_node& rNode, const std::string& name, const GroupElement& element)
    {
      auto child = rNode.append_child("xs:group");
      auto attrTypeName = child.append_attribute("name");
      attrTypeName.set_value(name.c_str());

      auto nodeChoice = child.append_child("xs:sequence");

      auto nodeAttribute = nodeChoice.append_child("xs:group");
      auto attrRef = nodeAttribute.append_attribute("ref");
      auto attrMinOccurs = nodeAttribute.append_attribute("minOccurs");
      auto attrMaxOccurs = nodeAttribute.append_attribute("maxOccurs");
      attrRef.set_value(element.Ref.c_str());
      attrMinOccurs.set_value(element.MinOccurs.c_str());
      attrMaxOccurs.set_value(element.MaxOccurs.c_str());
    }

    void AddGroupSequence(pugi::xml_node& rNode, const std::string& name, const std::vector<GroupElement>& elements)
    {
      auto child = rNode.append_child("xs:group");
      auto attrTypeName = child.append_attribute("name");
      attrTypeName.set_value(name.c_str());

      auto nodeChoice = child.append_child("xs:sequence");

      for (const auto& element : elements)
      {
        auto nodeAttribute = nodeChoice.append_child("xs:group");
        auto attrRef = nodeAttribute.append_attribute("ref");
        auto attrMinOccurs = nodeAttribute.append_attribute("minOccurs");
        auto attrMaxOccurs = nodeAttribute.append_attribute("maxOccurs");
        attrRef.set_value(element.Ref.c_str());
        attrMinOccurs.set_value(element.MinOccurs.c_str());
        attrMaxOccurs.set_value(element.MaxOccurs.c_str());
      }
    }

    void AddComplexTypeGroup(pugi::xml_node& rNode, const std::string& name, const std::string& ref)
    {
      auto child = rNode.append_child("xs:complexType");
      auto attrTypeName = child.append_attribute("name");
      attrTypeName.set_value(name.c_str());

      auto nodeAttribute = child.append_child("xs:group");
      auto attrRef = nodeAttribute.append_attribute("ref");
      attrRef.set_value(ref.c_str());
    }
  }

  //<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  //<xs:annotation>
  //  <xs:documentation>
  //    Describes the Fsl.gen format
  //  </xs:documentation>
  //</xs:annotation>
  void Save(const IO::Path& filename, ControlFactory& controlFactory)
  {
    pugi::xml_document doc;

    auto rootNode = doc.append_child("xs:schema");
    {
      auto attrXS = rootNode.append_attribute("xmlns:xs");
      attrXS.set_value("http://www.w3.org/2001/XMLSchema");

      auto childNode = rootNode.append_child("xs:annotation");
      auto docNode = childNode.append_child("xs:documentation");
      docNode.set_value("Describes the declarative FslSimpleUI format");
    }

    AddPrimitiveTypes(controlFactory.GetPrimitiveTypeRegistry(), rootNode);

    AddComplexType(rootNode, "_T_GridColumnDefinition", "Width");
    AddComplexType(rootNode, "_T_GridRowDefinition", "Height");
    AddGroupChoice(rootNode, "_G_GridColumnDefinition", "GridColumnDefinition", "_T_GridColumnDefinition");
    AddGroupChoice(rootNode, "_G_GridRowDefinition", "GridRowDefinition", "_T_GridRowDefinition");
    AddGroupSequence(rootNode, "_G_GridColumnDefinitionList", GroupElement("_G_GridColumnDefinition", "0", "unbounded"));
    AddGroupSequence(rootNode, "_G_GridRowDefinitionList", GroupElement("_G_GridRowDefinition", "0", "unbounded"));

    AddComplexTypeGroup(rootNode, "_T_GridLayoutColumnDefinitions", "_G_GridColumnDefinitionList");
    AddComplexTypeGroup(rootNode, "_T_GridLayoutRowDefinitions", "_G_GridRowDefinitionList");

    AddGroupChoice(rootNode, "_G_GridColumnDefinitions", "GridLayout.ColumnDefinitions", "_T_GridLayoutColumnDefinitions");
    AddGroupChoice(rootNode, "_G_GridRowDefinitions", "GridLayout.RowDefinitions", "_T_GridLayoutRowDefinitions");

    AddGroupSequence(rootNode, "_G_GridLayoutColumnDefinitions", GroupElement("_G_GridColumnDefinitions", "0", "1"));
    AddGroupSequence(rootNode, "_G_GridLayoutRowDefinitions", GroupElement("_G_GridRowDefinitions", "0", "1"));

    AddGroupSequence(rootNode, "_G_GridContentList",
                     {GroupElement("_G_GridLayoutColumnDefinitions", "0", "1"), GroupElement("_G_GridLayoutRowDefinitions", "0", "1"),
                      GroupElement("_G_WindowElement", "0", "unbounded")});


    std::vector<ControlName> controlNames = controlFactory.GetControlNames();

    for (const auto& name : controlNames)
    {
      auto formattedName = fmt::format("_T_{}", name.AsString());
      auto child = rootNode.append_child("xs:complexType");
      auto attrName = child.append_attribute("name");
      attrName.set_value(formattedName.c_str());

      AddControlContentProperties(controlFactory, child, name);
      AddThemeProperties(controlFactory, child, name);
      AddControlProperties(controlFactory, child, name);
    }

    AddWindowElement(controlFactory, rootNode, controlNames);

    //<xs:group name="_G_WindowList">
    //  <xs:sequence>
    //    <xs:group ref="_G_WindowElement" minOccurs="0" maxOccurs="unbounded"/>
    //  </xs:sequence>
    //</xs:group>
    {
      auto childNode = rootNode.append_child("xs:group");
      auto attrName = childNode.append_attribute("name");
      attrName.set_value("_G_WindowList");
      auto sequenceNode = childNode.append_child("xs:sequence");
      auto groupNode = sequenceNode.append_child("xs:group");
      auto attrRef = groupNode.append_attribute("ref");
      attrRef.set_value("_G_WindowElement");
      auto attrMinOccurs = groupNode.append_attribute("minOccurs");
      attrMinOccurs.set_value("0");
      auto attrMaxOccurs = groupNode.append_attribute("maxOccurs");
      attrMaxOccurs.set_value("unbounded");
    }

    // <xs:complexType name="_T_DeclarativeUITest">
    //   <xs:group ref="_G_RootElement"/>
    // </xs:complexType>
    {
      auto childNode = rootNode.append_child("xs:complexType");
      auto attrName = childNode.append_attribute("name");
      attrName.set_value("_T_FslSimpleUIRoot");
      auto groupNode = childNode.append_child("xs:group");
      auto attrRef = groupNode.append_attribute("ref");
      attrRef.set_value("_G_WindowElement");
    }

    //   <xs:element name="DeclarativeUITest" type="_T_DeclarativeUITest"/>
    {
      auto elementNode = rootNode.append_child("xs:element");
      auto attrName = elementNode.append_attribute("name");
      auto typeName = elementNode.append_attribute("type");
      // attrName.set_value("FslSimpleUIRoot");
      attrName.set_value("DeclarativeUITest");
      typeName.set_value("_T_FslSimpleUIRoot");
    }

    doc.save_file(filename.AsUTF8String().AsString().c_str());
  }
}

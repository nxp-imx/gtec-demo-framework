#ifndef FSLDATABINDING_BASE_EXCEPTIONS_HPP
#define FSLDATABINDING_BASE_EXCEPTIONS_HPP
/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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

namespace Fsl::DataBinding
{
  class UnknownPropertyInstanceException : public NotFoundException
  {
  public:
    explicit UnknownPropertyInstanceException(const char* const pszWhatArg)
      : NotFoundException(pszWhatArg)
    {
    }
    explicit UnknownPropertyInstanceException(const std::string& whatArg)
      : NotFoundException(whatArg)
    {
    }
  };


  class InvalidParentInstanceException : public std::invalid_argument
  {
  public:
    explicit InvalidParentInstanceException(const char* const pszWhatArg)
      : std::invalid_argument(pszWhatArg)
    {
    }

    explicit InvalidParentInstanceException(const std::string& whatArg)
      : std::invalid_argument(whatArg)
    {
    }
  };


  class DeadInstanceException : public std::invalid_argument
  {
  public:
    explicit DeadInstanceException(const char* const pszWhatArg)
      : std::invalid_argument(pszWhatArg)
    {
    }

    explicit DeadInstanceException(const std::string& whatArg)
      : std::invalid_argument(whatArg)
    {
    }
  };

  // class BindingTargetPropertyWrongClassException : public UsageErrorException
  //{
  // public:
  //   explicit BindingTargetPropertyWrongClassException(const char* const pszWhatArg)
  //     : UsageErrorException(pszWhatArg)
  //   {
  //   }

  //  explicit BindingTargetPropertyWrongClassException(const std::string& whatArg)
  //    : UsageErrorException(whatArg)
  //  {
  //  }
  //};

  // class BindingTargetClassImplementationErrorIncorrectClassException : public UsageErrorException
  //{
  // public:
  //   explicit BindingTargetClassImplementationErrorIncorrectClassException(const char* const pszWhatArg)
  //     : UsageErrorException(pszWhatArg)
  //   {
  //   }

  //  explicit BindingTargetClassImplementationErrorIncorrectClassException(const std::string& whatArg)
  //    : UsageErrorException(whatArg)
  //  {
  //  }
  //};


  class BindingUnsupportedException : public UsageErrorException
  {
  public:
    explicit BindingUnsupportedException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit BindingUnsupportedException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class BindingIncompatiblePropertiesException : public UsageErrorException
  {
  public:
    explicit BindingIncompatiblePropertiesException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit BindingIncompatiblePropertiesException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class BindingIncompatibleTypesException : public UsageErrorException
  {
  public:
    explicit BindingIncompatibleTypesException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit BindingIncompatibleTypesException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class CyclicBindingException : public UsageErrorException
  {
  public:
    explicit CyclicBindingException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit CyclicBindingException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class TwoWayBindingSourceException : public UsageErrorException
  {
  public:
    explicit TwoWayBindingSourceException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit TwoWayBindingSourceException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class TwoWayBindingReadOnlySourceException : public UsageErrorException
  {
  public:
    explicit TwoWayBindingReadOnlySourceException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit TwoWayBindingReadOnlySourceException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class DependencyPropertyDefinitionException : public UsageErrorException
  {
  public:
    explicit DependencyPropertyDefinitionException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit DependencyPropertyDefinitionException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class BindingErrorException : public UsageErrorException
  {
  public:
    explicit BindingErrorException(const char* const pszWhatArg)
      : UsageErrorException(pszWhatArg)
    {
    }

    explicit BindingErrorException(const std::string& whatArg)
      : UsageErrorException(whatArg)
    {
    }
  };


  class BindingNotSupportedException : public BindingErrorException
  {
  public:
    explicit BindingNotSupportedException(const char* const pszWhatArg)
      : BindingErrorException(pszWhatArg)
    {
    }

    explicit BindingNotSupportedException(const std::string& whatArg)
      : BindingErrorException(whatArg)
    {
    }
  };
}

#endif

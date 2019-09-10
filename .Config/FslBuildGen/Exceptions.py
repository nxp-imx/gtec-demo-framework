#!/usr/bin/env python3
#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
#      its contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************************************************************************************

from typing import Any
from typing import Dict
from typing import List
from typing import Optional

class ExitException(Exception):
    def __init__(self, exitCode: int) -> None:
        super().__init__()
        self.ExitCode = exitCode

class AggregateException(Exception):
    def __init__(self, exceptionList: List[Exception]) -> None:
        super().__init__("AggregateException")
        self.ExceptionList = exceptionList # type: List[Exception]

class GroupedException(Exception):
    def __init__(self, exceptionList: List[Exception]) -> None:
        super().__init__("GroupedException")
        self.ExceptionList = exceptionList # type: List[Exception]


class PackageHasMultipleDefinitionsException(Exception):
    def __init__(self, message: str) -> None:
        super().__init__(message)


class PackageMissingRequiredIncludeDirectoryException(Exception):
    def __init__(self, directory: str) -> None:
        msg = "Required include directory '{0}' not found".format(directory)
        super().__init__(msg)


class PackageMissingRequiredSourceDirectoryException(Exception):
    def __init__(self, directory: str) -> None:
        msg = "Required source directory '{0}' not found".format(directory)
        super().__init__(msg)


class PackageRequirementExtendsUnusedFeatureException(Exception):
    def __init__(self, requirementName: str, requirementExtends: str, packageName: str) -> None:
        msg = "Package requirement '{0}' in package '{1}' extends unknown feature '{2}'. Is it a feature spelling error or missing package dependency?".format(requirementName, packageName, requirementExtends)
        super().__init__(msg)


class InternalErrorException(Exception):
    """ Error
    """


class UsageErrorException(Exception):
    """ Error
    """


class UnknownTypeException(Exception):
    """ Error
    """


class FileNotFoundException(Exception):
    """ Error
    """
    def __init__(self, oldSchoolFormat: str, filename: str) -> None:
        super().__init__(oldSchoolFormat % filename)

class PackageIncludeFilePathInvalidException(Exception):
    """ Error
    """
    def __init__(self, packageName: str, filename: str, expectedIncludePathStart: str) -> None:
        msg = "Package '{0}' include file '{1}' did not start with '{2} as expected".format(packageName, filename, expectedIncludePathStart)
        super().__init__(msg)


class CircularDependencyException(Exception):
    """ E
    """


class CircularDependencyInDependentModuleException(Exception):
    """ E
    """


class NotImplementedException(Exception):
    """ E
    """


class UnsupportedException(Exception):
    """ E
    """


class DuplicatedNewProjectTemplatesRootPath(Exception):
    """ E
    """
    def __init__(self, name: str, configFileName1: str, configFileName2: str) -> None:
        msg = "Root path '{0}' listed multiple times in {1} and {2}".format(name, configFileName1, configFileName2)
        super().__init__(msg)


class DuplicatedConfigRootPath(Exception):
    """ E
    """
    def __init__(self, name: str, configFileName: str) -> None:
        msg = "Root path '{0}' listed multiple times in {1}".format(name, configFileName)
        super().__init__(msg)


class DuplicatedConfigBasePackage(Exception):
    """ E
    """
    def __init__(self, name: str, configFileName: str) -> None:
        msg = "Base package '{0}' listed multiple times in {1}".format(name, configFileName)
        super().__init__(msg)


class DuplicatedConfigPackageLocation(Exception):
    """ E
    """
    def __init__(self, name: str, configFileName: str) -> None:
        msg = "Package location '{0}' listed multiple times in {1}".format(name, configFileName)
        super().__init__(msg)


class DuplicatedConfigContentBuilder(Exception):
    """ E
    """
    def __init__(self, name: str, configFileName: str) -> None:
        msg = "ContentBuilder '{0}' listed multiple times in {1}".format(name, configFileName)
        super().__init__(msg)


class IncompleteVariableFoundException(Exception):
    """ E
    """
    def __init__(self, message: str, tag: Optional[object]) -> None:
        msg = "String '{0}' contains '${{' but is missing a terminating '}}'.".format(message)
        super().__init__(msg)
        self.Tag = tag


class IncompleteEnvironmentVariableFoundException(Exception):
    """ E
    """
    def __init__(self, message: str, tag: Optional[object]) -> None:
        msg = "String '{0}' contains '$(' but is missing a terminating ')'.".format(message)
        super().__init__(msg)
        self.Tag = tag


class EnvironmentVariableInMiddleOfStringException(Exception):
    """ E
    """
    def __init__(self, message: str, tag: Optional[object]) -> None:
        msg = "Environment variables can not be located in the middle of a string '{0}'".format(message)
        super().__init__(msg)
        self.Tag = tag


class CombinedEnvironmentVariableAndPathException(Exception):
    """ E
    """
    def __init__(self, message: str, tag: Optional[object]) -> None:
        msg = "String '{0}' contains a environment variable mixed with a path.".format(message)
        super().__init__(msg)
        self.Tag = tag


class CombinedVariableAndPathException(Exception):
    """ E
    """
    def __init__(self, message: str, tag: Optional[object]) -> None:
        msg = "String '{0}' contains a variable mixed with a path.".format(message)
        super().__init__(msg)
        self.Tag = tag


class VariableInMiddleOfStringException(Exception):
    """ E
    """
    def __init__(self, message: str, tag: Optional[object]) -> None:
        msg = "Variables can not be located in the middle of a string '{0}'".format(message)
        super().__init__(msg)
        self.Tag = tag


class VariableNotDefinedException(Exception):
    """ E
    """
    def __init__(self, variableName: str, variableDict: Dict[str, Optional[object]]) -> None:
        msg = "Variable '{0}' is not defined, possible variable names [{1}]".format(variableName, ", ".join(list(variableDict.keys())))
        super().__init__(msg)

class ToolDependencyNotFoundException(Exception):
    def __init__(self, depName: str, additionalMessage: str = "") -> None:
        message = "Package not found '{0}', its required by the tool chain.".format(depName)
        if len(additionalMessage) > 0:
            message += " {0}".format(additionalMessage)
        super().__init__(message)


class BasePackageNotFoundException(Exception):
    def __init__(self, depName: str, additionalMessage: str = "") -> None:
        message = "Base package not found '{0}' (added by project)".format(depName)
        if len(additionalMessage) > 0:
            message += " {0}".format(additionalMessage)
        super().__init__(message)

class DependencyNotFoundException(Exception):
    def __init__(self, packageName: str, depName: str, candidateList: Optional[List[str]] = None, additionalMessage: str = "") -> None:
        if candidateList is None or len(candidateList) <= 0:
            message = "'{0}' has a dependency to a unknown package '{1}'.".format(packageName, depName)
        else:
            message = "'{0}' has a dependency to a unknown package '{1}' did you mean {2}.".format(packageName, depName, candidateList)
        if len(additionalMessage) > 0:
            message += " {0}".format(additionalMessage)
        super().__init__(message)

# If this exception fires it means the package loader has a bug
class PackageLoaderFailedToLocatePackageException(DependencyNotFoundException):
    def __init__(self, packageName: str, depName: str) -> None:
        msg = "*INTERNAL_ERROR* the package exist, but the package loader failed to locate it."
        super().__init__(packageName, depName, additionalMessage=msg)


class InvalidDependencyException(Exception):
    def __init__(self, packageName: str, depName: str) -> None:
        super().__init__("'{0}' has a invalid dependency to package '{1}'".format(packageName, depName))

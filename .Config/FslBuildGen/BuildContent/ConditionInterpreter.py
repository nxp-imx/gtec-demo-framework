#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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
from typing import List
import ast
import copy
#import sys

# We use the python AST to do a safe condition evaluation
# Example
#  Expression
#    BoolOp
#      And
#      BoolOp
#        Or
#        Name
#          Load
#        Name
#          Load
#      UnaryOp
#        Not
#        Name
#          Load

class LocalNodeVisitor(ast.NodeVisitor):
    def __init__(self, contentProcessorName: str, source: str) -> None:
        self.ContentProcessorName = contentProcessorName
        self.Source = source
        self.Indent = 0

    def CheckNodeType(self, node: Any) -> None:
        if not (isinstance(node, ast.Expression) or
                isinstance(node, ast.BoolOp) or
                isinstance(node, ast.UnaryOp) or
                isinstance(node, ast.Not) or
                isinstance(node, ast.Or) or
                isinstance(node, ast.And) or
                isinstance(node, ast.Name) or
                isinstance(node, ast.Load)):
            if hasattr(node, 'col_offset'):
                if hasattr(node, 'id'):
                    raise Exception("FeatureRequirements contain unsupported node type '{0}' at '{1} (x:{2})' in content builder '{3}' feature requirement: '{4}'".format(type(node).__name__, node.id, node.col_offset, self.ContentProcessorName, self.Source))
                else:
                    raise Exception("FeatureRequirements contain unsupported node type '%s' at (x:%s) in content builder '%s' feature requirement: '%s'" % (type(node).__name__, node.col_offset, self.ContentProcessorName, self.Source))
            else:
                raise Exception("FeatureRequirements contain unsupported node type '%s' in content builder '%s' feature requirement: '%s'" % (type(node).__name__, self.ContentProcessorName, self.Source))


class ConditionNodeVisitor(LocalNodeVisitor):
    def __init__(self, contentProcessorName: str, source: str) -> None:
        super(ConditionNodeVisitor, self).__init__(contentProcessorName, source)

    def generic_visit(self, node: Any) -> None:
        self.CheckNodeType(node)
        #print(" "* (self.Indent*2) + type(node).__name__)
        self.Indent = self.Indent + 1
        ast.NodeVisitor.generic_visit(self, node)
        self.Indent = self.Indent - 1


class ConditionInterpreterNodeTransformer(ast.NodeTransformer):
    def __init__(self, featureIds: List[str]) -> None:
        self.__FeatureIds = featureIds

    def visit_Name(self, node: Any) -> Any:
        featureInList = node.id.lower() in self.__FeatureIds # type: bool
        # workaround the issue that ast.Num no longer accepts a bool in python3 and
        # ast.Constant is python 3.6+
        val = ast.Num(1 if featureInList else 0)
#        if sys.version_info < (3, 6):
 #       else:
 #           val = ast.Constant(featureInList)
        return ast.copy_location(val, node)


class ConditionInterpreter(object):
    def __init__(self, contentProcessorName: str, featureRequirements: str) -> None:
        self.__ContentProcessorName = contentProcessorName
        self.__FeatureRequirements = featureRequirements

        # Do some validation on the condition to ensure it only contains the elements we want and support
        astRootNode = self.__Parse(featureRequirements)
        nodeVisitor = ConditionNodeVisitor(contentProcessorName, featureRequirements)
        nodeVisitor.visit(astRootNode)
        self.__RootNode = astRootNode

    def __Parse(self, featureRequirements: str) -> ast.Module:
        try:
            return ast.parse(featureRequirements, mode='eval')
        except SyntaxError as ex:
            raise Exception("FeatureRequirements {0} in ContentBuilder '{1}' at 'x:{2}' in '{3}' ".format(ex.msg, self.__ContentProcessorName, ex.offset, str(ex.text).strip()))

    def CheckFeatureRequirements(self, featuresIds: List[str]) -> bool:
        astRootNode = copy.deepcopy(self.__RootNode)
        nodeTransformer = ConditionInterpreterNodeTransformer(featuresIds)
        nodeTransformer.visit(astRootNode)
        fixed = ast.fix_missing_locations(astRootNode)
        codeobj = compile(astRootNode, '<string>', mode='eval')
        # this should be safe as the root ast tree object has been verified to only contain things we expect
        return eval(codeobj) is True

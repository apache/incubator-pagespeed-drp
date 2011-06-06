#!/usr/bin/python2.4
#
# Copyright 2011 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Tests for node_table_builder."""

__author__ = 'bmcquade@google.com (Bryan McQuade)'

import unittest

import node_table_builder
import trie_node

class NodeTableBuilderTest(unittest.TestCase):
  """Test cases for the NodeTableBuilder."""

  def setUp(self):
    self._hostname_part_trie = trie_node.TrieNode()
    self._builder = node_table_builder.NodeTableBuilder()

  def testEmptyTable(self):
    """Tests an empty node table."""
    self._builder.BuildNodeTables(self._hostname_part_trie)
    self.assertEqual(0, len(self._builder.GetNodeTable()))

  def testBasic(self):
    """Tests a basic node table."""
    com = self._hostname_part_trie.GetOrCreateChild('com')
    uk = self._hostname_part_trie.GetOrCreateChild('uk')
    loo = com.GetOrCreateChild('loo')
    boo = com.GetOrCreateChild('boo')
    igloo = uk.GetOrCreateChild('igloo')

    self._builder.BuildNodeTables(self._hostname_part_trie)
    self.assertEqual([com, uk], self._builder.GetNodeTable())
    self.assertEqual([boo, loo, igloo], self._builder.GetLeafNodeTable())
    self.assertEqual(2, self._builder.GetChildNodeOffset(com))
    self.assertEqual(4, self._builder.GetChildNodeOffset(uk))

if __name__ == '__main__':
  unittest.main()

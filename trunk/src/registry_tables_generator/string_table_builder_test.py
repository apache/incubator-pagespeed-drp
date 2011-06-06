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

"""Tests for string_table_builder."""

__author__ = 'bmcquade@google.com (Bryan McQuade)'

import unittest

import string_table_builder
import trie_node

class StringTableBuilderTest(unittest.TestCase):
  """Test cases for the StringTableBuilder."""

  def setUp(self):
    self._hostname_part_trie = trie_node.TrieNode()
    self._suffix_trie = trie_node.TrieNode()
    self._builder = string_table_builder.StringTableBuilder()

  def testEmptyTable(self):
    """Tests an empty string table."""
    self._builder.BuildStringTable(self._hostname_part_trie, self._suffix_trie)
    self.assertEqual(0, len(self._builder.GetStringTable()))
    self.assertRaises(
        KeyError,
        string_table_builder.StringTableBuilder.GetHostnamePartOffset,
        self._builder, '')
    self.assertRaises(
        KeyError,
        string_table_builder.StringTableBuilder.GetHostnamePartOffset,
        self._builder, 'foo')

  def testBasic(self):
    """Tests a basic string table."""

    # First we need to populate the hostname part trie.
    com = self._hostname_part_trie.GetOrCreateChild('com')
    uk = self._hostname_part_trie.GetOrCreateChild('uk')
    com.AddChild('loo')
    com.AddChild('boo')
    uk.AddChild('igloo')

    # Next we populate the string suffix trie.
    m = self._suffix_trie.GetOrCreateChild('m')
    k = self._suffix_trie.GetOrCreateChild('k')
    o = self._suffix_trie.GetOrCreateChild('o')
    om = m.GetOrCreateChild('o')
    com = om.GetOrCreateChild('c')
    uk = k.GetOrCreateChild('u')
    oo = o.GetOrCreateChild('o')
    loo = oo.GetOrCreateChild('l')
    boo = oo.GetOrCreateChild('b')
    gloo = loo.GetOrCreateChild('g')
    igloo = gloo.GetOrCreateChild('i')
    com.SetTerminalNode()
    uk.SetTerminalNode()
    loo.SetTerminalNode()
    boo.SetTerminalNode()
    igloo.SetTerminalNode()

    self._builder.BuildStringTable(self._hostname_part_trie, self._suffix_trie)
    self.assertEqual(0, self._builder.GetHostnamePartOffset('com'))
    self.assertEqual(4, self._builder.GetHostnamePartOffset('uk'))
    self.assertEqual(7, self._builder.GetHostnamePartOffset('boo'))
    self.assertEqual(13, self._builder.GetHostnamePartOffset('loo'))
    self.assertEqual(11, self._builder.GetHostnamePartOffset('igloo'))
    self.assertRaises(
        KeyError,
        string_table_builder.StringTableBuilder.GetHostnamePartOffset,
        self._builder, 'gloo')

    expected = ['c', 'o', 'm', '\0',
                'u', 'k', '\0',
                'b', 'o', 'o', '\0',
                'i', 'g', 'l', 'o', 'o', '\0']
    self.assertEqual(expected, self._builder.GetStringTable())

if __name__ == '__main__':
  unittest.main()

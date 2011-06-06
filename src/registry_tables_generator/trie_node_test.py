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

"""Tests for trie_node."""

__author__ = 'bmcquade@google.com (Bryan McQuade)'

import unittest

import trie_node

class TrieNodeTest(unittest.TestCase):
  """Test cases for TrieNode."""

  def testRoot(self):
    """Tests the properties of a root node."""
    root = trie_node.TrieNode()
    self.assertRaises(AttributeError, trie_node.TrieNode.GetName, root)
    self.assertRaises(AttributeError, trie_node.TrieNode.GetParent, root)
    self.assertEqual([], root.GetParentChain())
    self.assertEqual('', root.GetIdentifier())
    self.assertRaises(ValueError, trie_node.TrieNode.GetChild, root, 'a')
    self.assertFalse(root.HasChildren())
    self.assertEqual([], root.GetChildren())
    self.assertFalse(root.IsTerminalNode())

  def testAddChildren(self):
    """Tests that AddChild and variants behaves as expected."""
    root = trie_node.TrieNode()
    root.AddChild('a')
    a = root.GetChild('a')
    self.assertNotEqual(None, a)
    self.assertNotEqual(None, root.GetOrCreateChild('a'))
    self.assertEqual('a', a.GetName())
    self.assertEqual('a', a.GetIdentifier())
    self.assertRaises(ValueError, trie_node.TrieNode.AddChild, root, 'a')
    self.assertFalse(a.HasChildren())

    self.assertNotEqual(None, root.GetOrCreateChild('b'))
    b = root.GetChild('b')
    self.assertNotEqual(None, b)
    self.assertEqual('b', b.GetName())
    self.assertEqual('b', b.GetIdentifier())
    self.assertFalse(b.HasChildren())

    self.assertTrue(root.HasChildren())
    self.assertEqual([a, b], root.GetChildren())

  def testTrieDepth(self):
    """Tests parent chain/identifier computations for 3-deep tries."""
    root = trie_node.TrieNode()
    a = root.GetOrCreateChild('a')
    ab = a.GetOrCreateChild('b')
    ac = a.GetOrCreateChild('c')
    ab2 = ab.GetOrCreateChild('2')
    ab1 = ab.GetOrCreateChild('1')

    self.assertTrue(a.HasChildren())
    self.assertTrue(ab.HasChildren())
    self.assertFalse(ac.HasChildren())
    self.assertFalse(ab1.HasChildren())

    self.assertEqual('1', ab1.GetName())
    self.assertEqual('1ba', ab1.GetIdentifier())
    self.assertEqual('1.b.a', ab1.GetIdentifier('.'))
    self.assertEqual(['1', 'b', 'a'], ab1.GetParentChain())

    self.assertEqual([ab1, ab2], ab.GetChildren())
    self.assertEqual([ab, ac], a.GetChildren())

if __name__ == '__main__':
  unittest.main()



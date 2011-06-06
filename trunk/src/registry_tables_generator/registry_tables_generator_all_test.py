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

"""Test suite for all registry_tables_generator tests."""

__author__ = 'bmcquade@google.com (Bryan McQuade)'

import unittest

import registry_tables_generator_test
import node_table_builder_test
import string_table_builder_test
import trie_node_test

ALL_TEST_CASES = (registry_tables_generator_test.RegistryTablesGeneratorTest,
                  node_table_builder_test.NodeTableBuilderTest,
                  string_table_builder_test.StringTableBuilderTest,
                  trie_node_test.TrieNodeTest)

def _BuildTestSuite(loader):
    """Get a test suite containing all RegistryTablesGenerator test cases."""
    suite = unittest.TestSuite()
    for test_class in ALL_TEST_CASES:
        tests = loader.loadTestsFromTestCase(test_class)
        suite.addTests(tests)
    return suite

def main():
  """Run all registry_tables_generator tests."""
  suite = _BuildTestSuite(unittest.TestLoader())
  unittest.TextTestRunner(verbosity=2).run(suite)

if __name__ == '__main__':
  main()

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

"""Tests for registry_tables_generator."""

__author__ = 'bmcquade@google.com (Bryan McQuade)'

import filecmp
import os
import unittest

import registry_tables_generator

# paths for test data
SCRIPT_DIR = os.path.dirname(__file__)
TEST_SRC_BASE = os.path.normpath(os.path.join(SCRIPT_DIR, 'testdata'))
TEST_TMP_BASE = '/tmp'
TEST_GOLDEN_BASE = os.path.join(TEST_SRC_BASE, 'golden')

class RegistryTablesGeneratorTest(unittest.TestCase):
  """Test cases for the RegistryTablesGenerator."""

  def setUp(self):
    self._out_dir = os.path.join(TEST_TMP_BASE,
                                 'registry_tables_generator_test_dir')

    self.assertFalse(os.path.exists(self._out_dir))
    os.mkdir(self._out_dir)
    self._outfile = os.path.join(self._out_dir, 'out.c')
    self._outtestfile = os.path.join(self._out_dir, 'test_out.c')

  def tearDown(self):
    os.system('rm -rf %s' % self._out_dir)

  def testNoSuchFile(self):
    """Tests that missing files are handled gracefully."""
    infile = os.path.join(TEST_SRC_BASE, 'does_not_exist.dat')
    self.assertNotEqual(
      0,
      registry_tables_generator.RegistryTablesGenerator(
        infile, self._outfile, self._outtestfile))

  def testSimple(self):
    """Tests that generated output matches golden files for a basic input."""
    self.assertEqual([], os.listdir(self._out_dir))
    infile = os.path.join(TEST_SRC_BASE, 'effective_tld_names_test.dat')
    golden = os.path.join(TEST_GOLDEN_BASE, 'out.c')
    golden_test = os.path.join(TEST_GOLDEN_BASE, 'test_out.c')
    self.assertEqual(
      0,
      registry_tables_generator.RegistryTablesGenerator(
        infile, self._outfile, self._outtestfile))
    self.assertTrue(filecmp.cmp(self._outfile, golden))
    self.assertTrue(filecmp.cmp(self._outtestfile, golden_test))

if __name__ == '__main__':
  unittest.main()

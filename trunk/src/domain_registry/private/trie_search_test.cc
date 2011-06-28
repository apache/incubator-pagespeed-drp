// Copyright 2011 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

extern "C" {

#include "domain_registry/private/trie_search.h"

// These additional methods in trie_search.c are not public, but we
// want to test them, so we declare their signatures here.

const struct TrieNode* FindNodeInRange(
    const char* value,
    const struct TrieNode* start,
    const struct TrieNode* end);

const char* FindLeafNodeInRange(
    const char* value,
    const REGISTRY_U16* start,
    const REGISTRY_U16* end);

}  // extern "C"

#include "testing/gtest/include/gtest/gtest.h"

// Include the simple test tables inline.
#include "domain_registry/testing/simple_node_table.c"

namespace {

class TrieSearchTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    // For these tests we use the test tables from simple_node_table.c.
    SetRegistryTables(kSimpleStringTable,
                      kSimpleNodeTable,
                      kSimpleNumRootChildren,
                      kSimpleLeafNodeTable,
                      kSimpleLeafNodeTableOffset);
  }

  static void TearDownTestCase() {
    SetRegistryTables(NULL, NULL, 0, NULL, 0);
  }
};

class TrieSearchFindNodeTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    // For these tests we need only the string table from simple_node_table.c.
    SetRegistryTables(kSimpleStringTable, NULL, 0, NULL, 0);
  }

  static void TearDownTestCase() {
    SetRegistryTables(NULL, NULL, 0, NULL, 0);
  }
};

TEST_F(TrieSearchTest, FindRegistryNode) {
  // Tests for searching root nodes.
  EXPECT_EQ(NULL, FindRegistryNode("", NULL));
  EXPECT_EQ(&kSimpleNodeTable[0], FindRegistryNode("com", NULL));
  EXPECT_EQ(&kSimpleNodeTable[1], FindRegistryNode("foo", NULL));

  // Tests for searching non-root nodes.
  EXPECT_EQ(&kSimpleNodeTable[2],
            FindRegistryNode("baz", &kSimpleNodeTable[1]));
  EXPECT_EQ(&kSimpleNodeTable[3],
            FindRegistryNode("bar", &kSimpleNodeTable[1]));
  EXPECT_EQ(&kSimpleNodeTable[3],
            FindRegistryNode("bar", &kSimpleNodeTable[1]));
  EXPECT_EQ(&kSimpleNodeTable[4],
            FindRegistryNode("zzz", &kSimpleNodeTable[1]));
  EXPECT_EQ(&kSimpleNodeTable[4],
            FindRegistryNode("wildcard", &kSimpleNodeTable[1]));
  EXPECT_EQ(&kSimpleNodeTable[4],
            FindRegistryNode("wc", &kSimpleNodeTable[1]));

  // Tests to verify that searches for wildcard and exceptions never match.
  EXPECT_EQ(NULL, FindRegistryNode("!baz", &kSimpleNodeTable[1]));
  EXPECT_EQ(NULL, FindRegistryNode("*", &kSimpleNodeTable[1]));

  // Test to verify that a search for the empty string on a wildcard
  // node doesn't match.
  EXPECT_EQ(NULL, FindRegistryNode("", &kSimpleNodeTable[1]));
}

TEST_F(TrieSearchTest, FindRegistryLeafNode) {
  // Simple leaf tests
  EXPECT_EQ(NULL, FindRegistryLeafNode("", &kSimpleNodeTable[0]));
  EXPECT_EQ(&kSimpleStringTable[4],
            FindRegistryLeafNode("foo", &kSimpleNodeTable[0]));

  EXPECT_EQ(&kSimpleStringTable[4],
            FindRegistryLeafNode("foo", &kSimpleNodeTable[3]));
  EXPECT_EQ(&kSimpleStringTable[8],
            FindRegistryLeafNode("zzz", &kSimpleNodeTable[3]));

  EXPECT_EQ(&kSimpleStringTable[10],
            FindRegistryLeafNode("baz", &kSimpleNodeTable[4]));
  EXPECT_EQ(&kSimpleStringTable[4],
            FindRegistryLeafNode("foo", &kSimpleNodeTable[4]));
  EXPECT_EQ(&kSimpleStringTable[8],
            FindRegistryLeafNode("zzz", &kSimpleNodeTable[4]));
  EXPECT_EQ(&kSimpleStringTable[8],
            FindRegistryLeafNode("wildcard", &kSimpleNodeTable[4]));
  EXPECT_EQ(&kSimpleStringTable[8],
            FindRegistryLeafNode("wc", &kSimpleNodeTable[4]));

  // Tests to verify that searches for wildcard and exceptions never
  // match.
  EXPECT_EQ(NULL, FindRegistryLeafNode("!baz", &kSimpleNodeTable[4]));
  EXPECT_EQ(NULL, FindRegistryLeafNode("*", &kSimpleNodeTable[4]));

  // Test to verify that a search for the empty string on a wildcard
  // node doesn't match.
  EXPECT_EQ(NULL, FindRegistryLeafNode("", &kSimpleNodeTable[4]));
}

TEST_F(TrieSearchTest, GetHostnamePart) {
  EXPECT_STREQ("com", GetHostnamePart(0));
  EXPECT_STREQ("foo", GetHostnamePart(4));
}

TEST_F(TrieSearchTest, HasLeafChildren) {
  EXPECT_TRUE(HasLeafChildren(&kSimpleNodeTable[0]));
  EXPECT_FALSE(HasLeafChildren(&kSimpleNodeTable[1]));
}

TEST_F(TrieSearchFindNodeTest, FindNodeInRangeSingleNode) {
  EXPECT_EQ(&kSimpleNodeTable[0],
            FindNodeInRange("com", &kSimpleNodeTable[0], &kSimpleNodeTable[0]));
  EXPECT_EQ(NULL,
            FindNodeInRange("co", &kSimpleNodeTable[0], &kSimpleNodeTable[0]));
  EXPECT_EQ(NULL,
            FindNodeInRange("comm",
                            &kSimpleNodeTable[0],
                            &kSimpleNodeTable[0]));
  EXPECT_EQ(NULL,
            FindNodeInRange("foo", &kSimpleNodeTable[0], &kSimpleNodeTable[0]));
  EXPECT_EQ(NULL,
            FindNodeInRange("", &kSimpleNodeTable[0], &kSimpleNodeTable[0]));
}

TEST_F(TrieSearchFindNodeTest, FindNodeInRangeTwoNodes) {
  EXPECT_EQ(&kSimpleNodeTable[0],
            FindNodeInRange("com", &kSimpleNodeTable[0], &kSimpleNodeTable[1]));
  EXPECT_EQ(&kSimpleNodeTable[1],
            FindNodeInRange("foo", &kSimpleNodeTable[0], &kSimpleNodeTable[1]));
  EXPECT_EQ(NULL,
            FindNodeInRange("", &kSimpleNodeTable[0], &kSimpleNodeTable[1]));
}

TEST_F(TrieSearchFindNodeTest, FindNodeInRangeThreeNodes) {
  EXPECT_EQ(&kSimpleNodeTable[2],
            FindNodeInRange("!baz", &kSimpleNodeTable[2], &kSimpleNodeTable[4]));
  EXPECT_EQ(&kSimpleNodeTable[3],
            FindNodeInRange("bar", &kSimpleNodeTable[2], &kSimpleNodeTable[4]));
  EXPECT_EQ(&kSimpleNodeTable[4],
            FindNodeInRange("*", &kSimpleNodeTable[2], &kSimpleNodeTable[4]));

  // exception and wildcard matches are not performed at this level,
  // so we expect them to fail here.
  EXPECT_EQ(NULL,
            FindNodeInRange("baz", &kSimpleNodeTable[2], &kSimpleNodeTable[4]));
  EXPECT_EQ(NULL,
            FindNodeInRange("wc", &kSimpleNodeTable[2], &kSimpleNodeTable[4]));
}

TEST_F(TrieSearchFindNodeTest, FindLeafNodeInRangeSingleNode) {
  EXPECT_EQ(&kSimpleStringTable[10],
            FindLeafNodeInRange("!baz",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[0]));
  EXPECT_EQ(NULL,
            FindLeafNodeInRange("foo",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[0]));
  EXPECT_EQ(NULL,
            FindLeafNodeInRange("",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[0]));
  EXPECT_EQ(NULL,
            FindLeafNodeInRange("!ba",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[0]));
  EXPECT_EQ(NULL,
            FindLeafNodeInRange("!bazz",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[0]));
}

TEST_F(TrieSearchFindNodeTest, FindLeafNodeInRangeTwoNodes) {
  EXPECT_EQ(&kSimpleStringTable[10],
            FindLeafNodeInRange("!baz",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[1]));
  EXPECT_EQ(&kSimpleStringTable[4],
            FindLeafNodeInRange("foo",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[1]));
  EXPECT_EQ(&kSimpleStringTable[8],
           FindLeafNodeInRange("*",
                                &kSimpleLeafNodeTable[1],
                                &kSimpleLeafNodeTable[2]));
}

TEST_F(TrieSearchFindNodeTest, FindLeafNodeInRangeThreeNodes) {
  EXPECT_EQ(&kSimpleStringTable[10],
            FindLeafNodeInRange("!baz",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[2]));
  EXPECT_EQ(&kSimpleStringTable[4],
            FindLeafNodeInRange("foo",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[2]));
  EXPECT_EQ(&kSimpleStringTable[8],
            FindLeafNodeInRange("*",
                                &kSimpleLeafNodeTable[0],
                                &kSimpleLeafNodeTable[2]));
}

}  // namespace

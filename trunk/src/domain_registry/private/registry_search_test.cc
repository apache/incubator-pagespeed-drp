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
#include "domain_registry/domain_registry.h"
#include "domain_registry/private/trie_search.h"
}  // extern "C"

#include "testing/gtest/include/gtest/gtest.h"

// Include the simple test tables inline.
#include "domain_registry/testing/simple_node_table.c"

namespace {

class RegistrySearchTest : public ::testing::Test {
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

TEST_F(RegistrySearchTest, Basic) {
  EXPECT_EQ(0, GetRegistryLength(""));
  EXPECT_EQ(0, GetRegistryLength("zzz"));
  EXPECT_EQ(0, GetRegistryLength("."));
  EXPECT_EQ(0, GetRegistryLength(".."));
}

TEST_F(RegistrySearchTest, FooDotCom) {
  EXPECT_EQ(0, GetRegistryLength("com"));
  EXPECT_EQ(0, GetRegistryLength("bar.com"));
  EXPECT_EQ(0, GetRegistryLength("www.bar.com"));
  EXPECT_EQ(0, GetRegistryLength("foo.com"));
  EXPECT_EQ(7, GetRegistryLength("a.foo.com"));
  EXPECT_EQ(7, GetRegistryLength("b.foo.com"));
  EXPECT_EQ(7, GetRegistryLength("zzz.foo.com"));
  EXPECT_EQ(7, GetRegistryLength("www.zzz.foo.com"));
}

TEST_F(RegistrySearchTest, DotFoo) {
  EXPECT_EQ(0, GetRegistryLength("foo"));
  EXPECT_EQ(0, GetRegistryLength(".foo"));
  EXPECT_EQ(0, GetRegistryLength("zzz.foo"));
}

TEST_F(RegistrySearchTest, BazDotFoo) {
  EXPECT_EQ(3, GetRegistryLength("baz.foo"));
  EXPECT_EQ(3, GetRegistryLength("a.baz.foo"));
  EXPECT_EQ(3, GetRegistryLength("b.baz.foo"));
}

TEST_F(RegistrySearchTest, BarDotFoo) {
  EXPECT_EQ(0, GetRegistryLength("bar.foo"));
  EXPECT_EQ(0, GetRegistryLength(".bar.foo"));

  // Our ruleset includes both "bar.foo" and "*.bar.foo". In all
  // cases, "*.bar.foo" should take precendence, since the list format
  // specification says: "If a hostname matches more than one rule in
  // the file, the longest matching rule (the one with the most
  // levels) will be used.".
  EXPECT_EQ(0, GetRegistryLength("www.bar.foo"));
  EXPECT_EQ(0, GetRegistryLength("foo.bar.foo"));

  // Tests for children of bar.foo (foo.bar.foo,
  // *.bar.foo). foo.bar.foo is redundant however we want to verify
  // that the implementation properly handles redundant entries such
  // as *.au and act.au in the actual list.
  EXPECT_EQ(11, GetRegistryLength("www.foo.bar.foo"));
  EXPECT_EQ(11, GetRegistryLength("www.zzz.bar.foo"));
  EXPECT_EQ(12, GetRegistryLength("www.asdf.bar.foo"));
  EXPECT_EQ(9, GetRegistryLength("z.a.bar.foo"));
  EXPECT_EQ(0, GetRegistryLength(".a.bar.foo"));
}

TEST_F(RegistrySearchTest, StarDotFoo) {
  // Tests for children of *.foo (!baz.*.foo, foo.*.foo, *.*.foo).

  // !baz.*.foo:
  EXPECT_EQ(5, GetRegistryLength("baz.a.foo"));
  EXPECT_EQ(5, GetRegistryLength("baz.b.foo"));
  EXPECT_EQ(7, GetRegistryLength("baz.zzz.foo"));
  EXPECT_EQ(7, GetRegistryLength(".baz.zzz.foo"));
  EXPECT_EQ(7, GetRegistryLength("www.baz.zzz.foo"));

  // foo.*.foo:
  EXPECT_EQ(0, GetRegistryLength("foo.a.foo"));
  EXPECT_EQ(0, GetRegistryLength(".foo.a.foo"));
  EXPECT_EQ(0, GetRegistryLength("..foo.a.foo"));
  EXPECT_EQ(9, GetRegistryLength("www.foo.a.foo"));
  EXPECT_EQ(9, GetRegistryLength("www.foo.b.foo"));
  EXPECT_EQ(11, GetRegistryLength("www.foo.zzz.foo"));
  EXPECT_EQ(11, GetRegistryLength("www.foo.zzz.foo"));

  // *.*.foo:
  EXPECT_EQ(0, GetRegistryLength("a.a.foo"));
  EXPECT_EQ(0, GetRegistryLength("b.b.foo"));
  EXPECT_EQ(0, GetRegistryLength("zzz.zzz.foo"));
  EXPECT_EQ(7, GetRegistryLength("a.a.a.foo"));
  EXPECT_EQ(7, GetRegistryLength("b.b.b.foo"));
  EXPECT_EQ(11, GetRegistryLength("www.zzz.zzz.foo"));
}

TEST_F(RegistrySearchTest, UnknownRegistries) {
  EXPECT_EQ(0, GetRegistryLength("foo.bar"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("foo.bar"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries(".foo.bar"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("..foo.bar"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("foo..bar"));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("bar"));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries(".bar"));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("..bar"));
}

TEST_F(RegistrySearchTest, MultipleDots) {
  // First, we know that *.foo.com should match.
  EXPECT_EQ(7, GetRegistryLength("a.foo.com"));

  // A single trailing dot indicates a fully qualified domain name,
  // which is a valid input.
  EXPECT_EQ(8, GetRegistryLength("a.foo.com."));
  EXPECT_EQ(8, GetRegistryLength(".a.foo.com."));
  EXPECT_EQ(8, GetRegistryLength("..a.foo.com."));

  // Multiple trailing dots are invalid.
  EXPECT_EQ(0, GetRegistryLength("a.foo.com.."));

  // If there are multiple dots between any hostname parts of the
  // registry, it should no longer match.
  EXPECT_EQ(0, GetRegistryLength("a.foo..com"));

  // If there are multiple dots before the registry, it's a match.
  EXPECT_EQ(7, GetRegistryLength("a..foo.com"));
  EXPECT_EQ(7, GetRegistryLength("a...foo.com"));
  EXPECT_EQ(7, GetRegistryLength(".a...foo.com"));
}

}  // namespace

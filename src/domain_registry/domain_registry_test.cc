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

#include "domain_registry/domain_registry.h"
#include "domain_registry/testing/test_entry.h"

#include "testing/gtest/include/gtest/gtest.h"

// Include the generated file that contains the actual registry tables.
#include "registry_tables_genfiles/test_registry_tables.c"

namespace {

static const size_t kTestTableLen = sizeof(kTestTable) / sizeof(kTestTable[0]);

class DomainRegistryTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    InitializeDomainRegistry();
  }
};

TEST_F(DomainRegistryTest, All) {
  for (size_t i = 0; i < kTestTableLen; ++i) {
    const struct TestEntry* test_entry = kTestTable + i;
    const char* hostname = test_entry->hostname;
    size_t expected_registry_len = test_entry->registry_len;
    size_t actual_registry_len = GetRegistryLength(hostname);
    EXPECT_EQ(expected_registry_len, actual_registry_len) << hostname;

    // Get the substring that represents the actual registry.
    const char* registry = hostname + strlen(hostname) - actual_registry_len;
    EXPECT_EQ('.', *(registry - 1)) << hostname;

    // When we pass in just the registry and ask for the registry
    // length, we expect to always get length 0, which indicates that
    // there is not a match.
    actual_registry_len = GetRegistryLength(registry);
    EXPECT_EQ(0, actual_registry_len) << hostname;
  }
}

TEST_F(DomainRegistryTest, Examples) {
  EXPECT_EQ(3, GetRegistryLength("www.google.com"));
  EXPECT_EQ(3, GetRegistryLength("WWW.gOoGlE.cOm"));
  EXPECT_EQ(3, GetRegistryLength("..google.com"));
  EXPECT_EQ(4, GetRegistryLength("google.com."));
  EXPECT_EQ(5, GetRegistryLength("a.b.co.uk"));
  EXPECT_EQ(0, GetRegistryLength("a.b.co..uk"));
  EXPECT_EQ(0, GetRegistryLength("C:"));
  EXPECT_EQ(0, GetRegistryLength("google.com.."));
  EXPECT_EQ(0, GetRegistryLength("192.168.0.1"));
  EXPECT_EQ(0, GetRegistryLength("bar"));
  EXPECT_EQ(0, GetRegistryLength("co.uk"));
  EXPECT_EQ(0, GetRegistryLength("foo.bar"));
  EXPECT_EQ(0, GetRegistryLength("bar"));

  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("foo.bar"));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("bar"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("www.google.com"));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("com"));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("co.uk"));
}

}  // namespace

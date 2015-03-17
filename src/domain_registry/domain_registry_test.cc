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

extern "C" {
#include "domain_registry/private/assert.h"
}  // extern "C"

#include "testing/gtest/include/gtest/gtest.h"

// Include the generated file that contains the actual registry tables.
#include "registry_tables_genfiles/test_registry_tables.h"

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
    if (test_entry->is_exception_rule != 0) {
      // For exception rules, the returned registry length isn't the
      // same as the length of the registry. For instance if we have
      // rule !foo.bar and receive input www.foo.bar, we would expect
      // a registry length of 3. However, passing in just that
      // registry ("bar") would no longer trigger the exception
      // rule. Instead we need to pass in the registry plus the
      // exception component (e.g. "foo.bar"). To find
      // "foo.bar" we need to search backwards to the previous dot.

      // First verify that passing in just the registry (e.g. "bar")
      // produces a registry length of 0:
      EXPECT_EQ(0, GetRegistryLength(registry)) << hostname << ", " << registry;

      // Skip over the dot that precedes the registry before beginning
      // to search for the next dot.
      registry -= 2;

      // Walk backwards until we find the next dot.
      while (*registry != '.') {
        EXPECT_GT(registry, hostname) << hostname << ", " << registry;
        --registry;
      }
      // The registry starts immediately after the dot.
      ++registry;

      EXPECT_EQ('.', *(registry - 1)) << hostname << ", " << registry;
    }

    // When we pass in just the registry and ask for the registry
    // length, we expect to get the registry length.
    actual_registry_len = GetRegistryLength(registry);
    EXPECT_EQ(expected_registry_len, actual_registry_len)
        << hostname << ", " << registry;
  }
}

TEST_F(DomainRegistryTest, Basic) {
  EXPECT_EQ(0, GetRegistryLength(NULL));
  EXPECT_EQ(0, GetRegistryLength(""));
  EXPECT_EQ(0, GetRegistryLength(" "));
  EXPECT_EQ(0, GetRegistryLength("  "));
  EXPECT_EQ(0, GetRegistryLength("."));
  EXPECT_EQ(0, GetRegistryLength(".."));
  EXPECT_EQ(0, GetRegistryLength("..."));
  EXPECT_EQ(0, GetRegistryLength(". ."));
  EXPECT_EQ(0, GetRegistryLength(". . "));
  EXPECT_EQ(0, GetRegistryLength(" ."));
  EXPECT_EQ(0, GetRegistryLength(" . "));

  // The domain registry provider does not do any hostname-part
  // validation, so it assumes space and other characters are valid
  // hostnames. It is the responsibility of the caller to verify the
  // validity of the hostname.
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries(NULL));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries(""));
  EXPECT_EQ(1, GetRegistryLengthAllowUnknownRegistries(" "));
  EXPECT_EQ(2, GetRegistryLengthAllowUnknownRegistries("  "));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("."));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries(".."));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("..."));
  EXPECT_EQ(2, GetRegistryLengthAllowUnknownRegistries(". ."));
  EXPECT_EQ(1, GetRegistryLengthAllowUnknownRegistries(". . "));
  EXPECT_EQ(2, GetRegistryLengthAllowUnknownRegistries(" ."));
  EXPECT_EQ(1, GetRegistryLengthAllowUnknownRegistries(" . "));
}

TEST_F(DomainRegistryTest, Examples) {
  EXPECT_EQ(3, GetRegistryLength("www.google.com"));
  EXPECT_EQ(3, GetRegistryLength("WWW.gOoGlE.cOm"));
  EXPECT_EQ(3, GetRegistryLength("..google.com"));
  EXPECT_EQ(4, GetRegistryLength("google.com."));
  EXPECT_EQ(5, GetRegistryLength("a.b.co.uk"));
  EXPECT_EQ(2, GetRegistryLength("a.b.co..uk"));
  EXPECT_EQ(0, GetRegistryLength("C:"));
  EXPECT_EQ(0, GetRegistryLength("google.com.."));
  EXPECT_EQ(3, GetRegistryLength("bar"));
  EXPECT_EQ(3, GetRegistryLength("example.bar"));
  EXPECT_EQ(5, GetRegistryLength("co.uk"));
  EXPECT_EQ(3, GetRegistryLength("foo.bar"));
  EXPECT_EQ(0, GetRegistryLength("foo.臺灣"));
  EXPECT_EQ(11, GetRegistryLength("foo.xn--nnx388a"));
  EXPECT_EQ(22, GetRegistryLength("test.pagespeedmobilizer.com"));
  EXPECT_EQ(13, GetRegistryLength("www.amazonaws.com"));
  EXPECT_EQ(36, GetRegistryLength("foo.ap-northeast-1.compute.amazonaws.com"));
  EXPECT_EQ(11, GetRegistryLength("foo.anything.il"));
  EXPECT_EQ(5, GetRegistryLength("foo.co.il"));
  EXPECT_EQ(14, GetRegistryLength("foo.blogspot.co.il"));

  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("foo.bar"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("bar"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("www.google.com"));
  EXPECT_EQ(3, GetRegistryLengthAllowUnknownRegistries("com"));
  EXPECT_EQ(5, GetRegistryLengthAllowUnknownRegistries("co.uk"));
  EXPECT_EQ(0, GetRegistryLengthAllowUnknownRegistries("foo.臺灣"));
  EXPECT_EQ(11, GetRegistryLengthAllowUnknownRegistries("foo.xn--nnx388a"));

  // It is an error to pass in an IP address but we include some tests
  // to verify that they do not cause crashes. The actual return
  // values are not specified so we don't expect specific values for
  // these calls.
  GetRegistryLength("192.168.0.1");
  GetRegistryLength("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  GetRegistryLengthAllowUnknownRegistries("192.168.0.1");
  GetRegistryLengthAllowUnknownRegistries(
      "2001:0db8:85a3:0000:0000:8a2e:0370:7334");
}

// Passes in all combinations of inputs that are three characters in
// length, to verify that none of those inputs can cause crashes.
TEST_F(DomainRegistryTest, BruteForceCrashFuzzer) {
  const size_t kNumChars = 3;
  const size_t kMaxValue = 1 << (8 * kNumChars);
  unsigned char buf[kNumChars + 1];
  memset(buf, 0, kNumChars + 1);
  for (size_t i = 0; i < kMaxValue; ++i) {
    size_t offset = kNumChars;
    for (size_t val = i; val > 0; val /= 256) {
      --offset;
      buf[offset] = val % 256;
    }
    ASSERT_EQ(0, buf[kNumChars]);  // Make sure we are still null-terminated.
    const char* candidate = reinterpret_cast<char*>(&(buf[offset]));
    GetRegistryLength(candidate);
    GetRegistryLengthAllowUnknownRegistries(candidate);
  }
}

class AssertHandlerTest : public ::testing::Test {
 protected:
  static void TestAssertHandler(
      const char* file, int line, const char* cond_str) {
    g_assert_file = file;
    g_assert_line = line;
    g_assert_cond_str = cond_str;
  }

  virtual void SetUp() {
    ClearAssertion();
  }

  void ClearAssertion() {
    g_assert_file = NULL;
    g_assert_line = -1;
    g_assert_cond_str = NULL;
  }

  void AssertNoAssertion() {
    AssertAssertion(NULL, -1, NULL);
  }

  void AssertAssertion(
      const char* file, int line, const char* cond_str) {
    ASSERT_EQ(file, g_assert_file);
    ASSERT_EQ(line, g_assert_line);
    ASSERT_EQ(cond_str, g_assert_cond_str);
  }

  static const char* g_assert_file;
  static int g_assert_line;
  static const char* g_assert_cond_str;

  static const char* const kFilename;
  static const char* const kCondStr;
};

const char* AssertHandlerTest::g_assert_file = NULL;
int AssertHandlerTest::g_assert_line = -1;
const char* AssertHandlerTest::g_assert_cond_str = NULL;

// dummy values
const char* const AssertHandlerTest::kFilename = "filename";
const char* const AssertHandlerTest::kCondStr = "cond str";

TEST_F(AssertHandlerTest, OverrideAssertHandler) {
  SetDomainRegistryAssertHandler(TestAssertHandler);
  DoAssert(kFilename, 1, kCondStr, 1);
  AssertNoAssertion();
  DoAssert(kFilename, 1, kCondStr, 0);
  AssertAssertion(kFilename, 1, kCondStr);
}

}  // namespace

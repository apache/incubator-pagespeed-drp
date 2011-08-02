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

#include <string.h>

extern "C" {
#include "domain_registry/private/string_util.h"
}  // extern "C"

#include "testing/gtest/include/gtest/gtest.h"

namespace {

const char* kHostname = "fOo.BaR.cOM";
const size_t kHostnameLen = strlen(kHostname);

TEST(StringUtilTest, IsWildcardComponent) {
  ASSERT_TRUE(IsWildcardComponent("*"));
  ASSERT_FALSE(IsWildcardComponent(""));
  ASSERT_FALSE(IsWildcardComponent("com"));
  ASSERT_TRUE(IsWildcardComponent("*foo"));
}

TEST(StringUtilTest, IsExceptionComponent) {
  ASSERT_TRUE(IsExceptionComponent("!foo"));
  ASSERT_FALSE(IsExceptionComponent(""));
  ASSERT_FALSE(IsExceptionComponent("foo"));
  ASSERT_TRUE(IsExceptionComponent("!"));
}

TEST(StringUtilTest, ReplaceChar) {
  char* hostname = strdup(kHostname);
  ReplaceChar(hostname, '.', 0);
  ASSERT_STREQ("fOo", hostname);
  ASSERT_STREQ("BaR", hostname + 4);
  ASSERT_STREQ("cOM", hostname + 8);
  free(hostname);
}

TEST(StringUtilTest, ToLowerASCII) {
  char* hostname = strdup(kHostname);
  ToLowerASCII(hostname, hostname + kHostnameLen);
  ASSERT_STREQ("foo.bar.com", hostname);
  free(hostname);
}

TEST(StringUtilTest, HostnamePartCmp) {
  ASSERT_GT(0, HostnamePartCmp("zza", "zzb"));
  ASSERT_LT(0, HostnamePartCmp("zzb", "zza"));
  ASSERT_EQ(0, HostnamePartCmp("aaa", "aaa"));
  ASSERT_GT(0, HostnamePartCmp("*", "zzz"));
  ASSERT_LT(0, HostnamePartCmp("zzz", "*"));
}

}  // namespace

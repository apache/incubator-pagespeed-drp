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
//
// Performance test, which can be used for A/B testing to measure the
// performance impact of a change. Run the test before making the change,
// then after making the change. Look at the difference in runtimes
// before and after to determine the performance impact.

#include <stdio.h>

#include "domain_registry/domain_registry.h"
#include "domain_registry/testing/test_entry.h"

// Include the generated file that contains the actual registry tables.
#include "registry_tables_genfiles/test_registry_tables.h"

static const size_t kTestTableLen = sizeof(kTestTable) / sizeof(kTestTable[0]);
static const size_t kNumIters = 10000;

int main(int argc, char** argv) {
  InitializeDomainRegistry();

  size_t num_iters, i;
  for (num_iters = 0; num_iters < kNumIters; ++num_iters) {
    for (i = 0; i < kTestTableLen; ++i) {
      const struct TestEntry* test_entry = kTestTable + i;
      const char* hostname = test_entry->hostname;
      size_t expected_registry_len = test_entry->registry_len;
      size_t actual_registry_len = GetRegistryLength(hostname);
      if (expected_registry_len != actual_registry_len) {
        fprintf(stderr, "Mismatch for %s. Expected %d, actual %d.\n",
                hostname,
                (int)expected_registry_len,
                (int)actual_registry_len);
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

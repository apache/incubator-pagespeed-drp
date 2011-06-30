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

#ifndef DOMAIN_REGISTRY_PRIVATE_STRING_UTIL_H_
#define DOMAIN_REGISTRY_PRIVATE_STRING_UTIL_H_

#include <stdio.h>
#include <string.h>

#include "domain_registry/private/assert.h"

static const char kUpperLowerDistance = 'A' - 'a';

static inline int IsWildcardComponent(const char* component) {
  if (component[0] == '*') {
    // Wildcards should always appear by themselves. It is an error
    // for the wildcard character to appear adjacent to a non-NULL byte.
    DCHECK(component[1] == 0);
    return 1;
  }
  return 0;
}

static inline int IsExceptionComponent(const char* component) {
  if (component[0] == '!') {
    // Exceptions should always appear with a string immediately
    // after. It is an error for the exception character to appear by
    // itself.
    DCHECK(component[1] != 0);
    return 1;
  }
  return 0;
}

static inline void ReplaceChar(char* value, char old, char newval) {
  while ((value = strchr(value, old)) != NULL) {
    *value = newval;
    ++value;
  }
}

static inline void ToLower(char* buf, const char* end) {
  for (; buf < end; ++buf) {
    char c = *buf;
    if (c >= 'A' && c <= 'Z') {
      *buf = c - kUpperLowerDistance;
    }
  }
}

static inline int HostnamePartCmp(const char *a, const char *b) {
  // Optimization: do not invoke strcmp() unless the first characters
  // in each string match. Since we are performing a binary search, we
  // expect most invocations to strcmp to not have matching arguments,
  // and thus not invoke strcmp. This reduces overall runtime by 5-10%
  // on a Linux laptop running a -O2 optimized build.
  int ret = *(unsigned char *)a - *(unsigned char *)b;
  // NOTE: we could invoke strcmp on a+1,b+1 if we are
  // certain that neither a nor b are the empty string. For now we
  // take the more conservative approach.
  if (ret == 0) return strcmp(a, b);
  return ret;
}

#endif  // DOMAIN_REGISTRY_PRIVATE_STRING_UTIL_H_

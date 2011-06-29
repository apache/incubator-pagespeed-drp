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

#include <string.h>

#include "domain_registry/private/assert.h"
#include "domain_registry/private/string_util.h"
#include "domain_registry/private/trie_search.h"

// Get a pointer to the beginning of the valid registry. If rule_part
// is an exception component, this will seek past the
// rule_part. Otherwise this will simply return the component itself.
static const char* GetDomainRegistryStr(const char* rule_part,
                                        const char* component) {
  if (IsExceptionComponent(rule_part)) {
    return component + strlen(component) + 1;
  } else {
    return component;
  }
}

// Iterates the hostname-parts between start and end in reverse order,
// separated by the character specified by sep. For instance if the
// string between start and end is "foo\0bar\0com" and sep is the null
// character, we will return a pointer to "com", then "bar", then
// "foo".
static const char* GetNextHostnamePart(const char* start,
                                       const char* end,
                                       char sep,
                                       void** ctx) {
  if (*ctx == NULL) {
    *ctx = (void*) end;

    // Special case: a single trailing dot indicates a fully-qualified
    // domain name. Skip over it.
    if (*(end - 1) == 0) {
      *ctx = (void*) (end - 1);
    }
  }
  const char* last = *ctx;
  const char* i;
  if (start > last) return NULL;
  for (i = last - 1; i >= start; --i) {
    if (*i == sep) {
      *ctx = (void*) i;
      return i + 1;
    }
  }
  if (last != start && *start != 0) {
    // Special case: If we didn't find a match, but the context
    // indicates that we haven't visited the first component yet, and
    // there is a non-NULL first component, then visit the first
    // component.
    *ctx = (void*) start;
    return start;
  }
  return NULL;
}

// Iterate over all hostname-parts between value and value_end, where
// the hostname-parts are separated by character sep.
static const char* IterateAllHostnameParts(const char* value,
                                           const char* value_end,
                                           const char sep) {
  void *ctx = NULL;
  const struct TrieNode* current = NULL;
  const char* component = NULL;
  const char* last_valid = NULL;

  // Iterate over the hostname components one at a time, e.g. if value
  // is foo.com, we will first visit component com, then component foo.
  while ((component =
          GetNextHostnamePart(value, value_end, sep, &ctx)) != NULL) {
    if (component[0] == 0 ||
        IsExceptionComponent(component) ||
        IsWildcardComponent(component)) {
      // Inputs that contain empty components, wildcards, or
      // exceptions are invalid.
      return NULL;
    }
    current = FindRegistryNode(component, current);
    if (current == NULL) {
      break;
    }
    if (current->is_terminal == 1) {
      last_valid = GetDomainRegistryStr(
          GetHostnamePart(current->string_table_offset), component);
    } else {
      last_valid = NULL;
    }
    if (HasLeafChildren(current)) {
      // The child nodes are in the leaf node table, so perform a
      // search in that table.
      component = GetNextHostnamePart(value, value_end, sep, &ctx);
      if (component == NULL) {
        break;
      }
      const char* leaf_node = FindRegistryLeafNode(component, current);
      if (leaf_node == NULL) {
        break;
      }
      return GetDomainRegistryStr(leaf_node, component);
    }
  }

  return last_valid;
}

static size_t GetRegistryLengthImpl(
    const char* value, const char* value_end, const char sep) {
  while (*value == sep && value < value_end) {
    // Skip over leading separators.
    ++value;
  }
  const char* last_valid = IterateAllHostnameParts(value, value_end, sep);
  if (last_valid == NULL) {
    // Didn't find a match.
    return 0;
  }
  if (last_valid == value) {
    // Special case: if the value is an exact match, it is itself a
    // top-level registry. However, in this case, we want to return 0,
    // to indicate that it's not allowed to set cookies, etc on the
    // top-level registry.
    return 0;
  }
  if (last_valid < value || last_valid >= value_end) {
    // Error cases.
    DCHECK(last_valid >= value);
    DCHECK(last_valid < value_end);
    return 0;
  }
  size_t match_len = value_end - last_valid;
  if (match_len >= (value_end - value)) {
    return 0;
  }
  return match_len;
}

size_t GetRegistryLength(const char* hostname) {
  // Replace dots between hostname parts with the null byte. This
  // allows us to index directly into the string and refer to each
  // hostname-part as if it were its own null-terminated string.
  char* buf = strdup(hostname);
  ReplaceChar(buf, '.', '\0');

  const char* buf_end = buf + strlen(hostname);
  DCHECK(*buf_end == 0);

  // Normalize the input by converting all characters to lowercase.
  ToLower(buf, buf_end);
  size_t registry_length = GetRegistryLengthImpl(buf, buf_end, '\0');
  free(buf);
  return registry_length;
}

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
static const char* GetNextHostnamePartImpl(const char* start,
                                           const char* end,
                                           char sep,
                                           void** ctx) {
  const char* last;
  const char* i;
  
  if (*ctx == NULL) {
    *ctx = (void*) end;

    // Special case: a single trailing dot indicates a fully-qualified
    // domain name. Skip over it.
    if (*(end - 1) == 0) {
      *ctx = (void*) (end - 1);
    }
  }
  last = *ctx;
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

static const char* GetNextHostnamePart(const char* start,
                                       const char* end,
                                       char sep,
                                       void** ctx) {
  const char* hostname_part = GetNextHostnamePartImpl(start, end, sep, ctx);
  if (IsInvalidComponent(hostname_part)) {
    return NULL;
  }
  return hostname_part;
}

// Iterate over all hostname-parts between value and value_end, where
// the hostname-parts are separated by character sep.
static const char* GetRegistryForHostname(const char* value,
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
    const char* leaf_node;

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
      leaf_node = FindRegistryLeafNode(component, current);
      if (leaf_node == NULL) {
        break;
      }
      return GetDomainRegistryStr(leaf_node, component);
    }
  }

  return last_valid;
}

static size_t GetRegistryLengthImpl(
    const char* value,
    const char* value_end,
    const char sep,
    int allow_unknown_registries) {
  const size_t value_len = value_end - value;
  const char* registry;
  size_t match_len;

  while (*value == sep && value < value_end) {
    // Skip over leading separators.
    ++value;
  }
  registry = GetRegistryForHostname(value, value_end, sep);
  if (registry == NULL) {
    // Didn't find a match. If unknown registries are allowed, see if
    // the root hostname part is not in the table. If so, consider it to be a
    // valid registry, and return its length.
    if (allow_unknown_registries != 0) {
      void* ctx = NULL;
      const char* root_hostname_part =
          GetNextHostnamePart(value, value_end, sep, &ctx);
      // See if the root hostname-part is in the table. If it's not in
      // the table, then consider the unknown registry to be a valid
      // registry.
      if (root_hostname_part != NULL &&
          FindRegistryNode(root_hostname_part, NULL) == NULL) {
        registry = root_hostname_part;
      }
    }
    if (registry == NULL) {
      return 0;
    }
  }
  if (registry == value) {
    // Special case: if the value is an exact match, it is itself a
    // top-level registry. However, in this case, we want to return 0,
    // to indicate that it's not allowed to set cookies, etc on the
    // top-level registry.
    return 0;
  }
  if (registry < value || registry >= value_end) {
    // Error cases.
    DCHECK(registry >= value);
    DCHECK(registry < value_end);
    return 0;
  }
  match_len = value_end - registry;
  if (match_len >= value_len) {
    return 0;
  }
  return match_len;
}

size_t GetRegistryLength(const char* hostname) {
  const char* buf_end;
  size_t registry_length;

  // Replace dots between hostname parts with the null byte. This
  // allows us to index directly into the string and refer to each
  // hostname-part as if it were its own null-terminated string.
  char* buf = strdup(hostname);
  ReplaceChar(buf, '.', '\0');

  buf_end = buf + strlen(hostname);
  DCHECK(*buf_end == 0);

  // Normalize the input by converting all characters to lowercase.
  ToLower(buf, buf_end);
  registry_length = GetRegistryLengthImpl(buf, buf_end, '\0', 0);
  free(buf);
  return registry_length;
}

size_t GetRegistryLengthAllowUnknownRegistries(const char* hostname) {
  const char* buf_end;
  size_t registry_length;
  
  // Replace dots between hostname parts with the null byte. This
  // allows us to index directly into the string and refer to each
  // hostname-part as if it were its own null-terminated string.
  char* buf = strdup(hostname);
  ReplaceChar(buf, '.', '\0');

  buf_end = buf + strlen(hostname);
  DCHECK(*buf_end == 0);

  // Normalize the input by converting all characters to lowercase.
  ToLower(buf, buf_end);
  registry_length = GetRegistryLengthImpl(buf, buf_end, '\0', 1);
  free(buf);
  return registry_length;
}

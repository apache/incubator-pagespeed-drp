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

#include "domain_registry/private/registry_types.h"
#include "domain_registry/private/trie_node.h"

//  com: 0
//  foo: 4
//    *: 8
// !baz: 10
//  bar: 15
static const char kSimpleStringTable[] = "com\0foo\0*\0!baz\0bar\0";

static const struct TrieNode kSimpleNodeTable[] = {
  // TrieNode is a tuple with 4 fields (in order):
  // 1. string_table_offset
  // 2. first_child_offset. Note that leaf table offsets start at 5.
  // 3. num_children
  // 4. is_terminal
  { 0,  7, 1, 0 },  // com       (1 leaf child at offset 2)
  { 4,  2, 3, 0 },  // foo       (3 non-leaf children at offset 2)
  { 10, 0, 0, 1 },  // !baz.foo  (0 children)
  { 8,  5, 3, 1 },  // *.foo     (3 leaf children at offset 0)
  { 15, 6, 2, 1 },  // bar.foo   (2 leaf children at offset 1)
};

static const REGISTRY_U16 kSimpleLeafNodeTable[] = {
  10,  // !baz
  8,   // *
  4,   // foo
};

static const size_t kSimpleNumRootChildren = 2;
static const size_t kSimpleLeafNodeTableOffset = 5;

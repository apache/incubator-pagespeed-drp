# Copyright 2011 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'targets': [
    {
      'target_name': 'domain_registry_lib',
      'type': 'static_library',
      'dependencies': [
        'assert_lib',
      ],
      'sources': [
        'private/registry_search.c',
        'private/registry_types.h',
        'private/string_util.h',
        'private/trie_node.h',
        'private/trie_search.c',
        'private/trie_search.h',
      ],
      'include_dirs': [
        '..',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '..',
        ],
      },
    },
    {
      'target_name': 'init_registry_tables_lib',
      'type': 'static_library',
      'dependencies': [
        '../registry_tables_generator/registry_tables_generator.gyp:generate_registry_tables',
        'domain_registry_lib',
      ],
      'sources': [
        'private/init_registry_tables.c',
      ],
      'include_dirs': [
        '..',
      ],
    },

    # The following targets are "private" and should not be referenced
    # from outside this package.
    {
      'target_name': 'domain_registry',
      'type': 'executable',
      'dependencies': [
        'domain_registry_lib',
        'init_registry_tables_lib',
      ],
      'sources': [
        'example.c',
      ],
    },
    {
      'target_name': 'assert_lib',
      'type': 'static_library',
      'sources': [
        'private/assert.c',
        'private/assert.h',
      ],
      'include_dirs': [
        '..',
      ],
    },
    {
      'target_name': 'domain_registry_test',
      'type': 'executable',
      'dependencies': [
        '../registry_tables_generator/registry_tables_generator.gyp:generate_registry_tables',
        'assert_lib',
        'domain_registry_lib',
        'init_registry_tables_lib',
        '<(DEPTH)/testing/gtest.gyp:gtest',
        '<(DEPTH)/testing/gtest.gyp:gtest_main',
      ],
      'sources': [
        'domain_registry_test.cc',
        'private/registry_search_test.cc',
        'private/string_util_test.cc',
        'private/trie_search_test.cc',
      ],
      'conditions': [
        ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
          'cflags': [ '-pthread' ],
          'ldflags': [ '-pthread' ],
        }],
      ],
    },
    {
      'target_name': 'domain_registry_perf_test',
      'suppress_wildcard': 1,
      'type': 'executable',
      'dependencies': [
        '../registry_tables_generator/registry_tables_generator.gyp:generate_registry_tables',
        'domain_registry_lib',
        'init_registry_tables_lib',
      ],
      'sources': [
        'domain_registry_perf_test.c',
      ],
    },
  ],
}

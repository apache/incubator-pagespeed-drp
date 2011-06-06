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
  'variables': {
    'domain_registry_provider_relpath%': '.',
    'domain_registry_provider_root': '<(DEPTH)/<(domain_registry_provider_relpath)',
    'registry_tables_generator_root': '<(domain_registry_provider_root)/registry_tables_generator',
    'chromium_code': 1,
    'out_dir': '<(SHARED_INTERMEDIATE_DIR)/registry_tables_generator_out',
    'executable': '<(registry_tables_generator_root)/registry_tables_generator.py',
    'in_dat_file': '<(DEPTH)/third_party/effective_tld_names/effective_tld_names.dat',
    'out_registry_file': '<(out_dir)/<(domain_registry_provider_relpath)/registry_tables.c',
    'out_registry_test_file': '<(out_dir)/<(domain_registry_provider_relpath)/test_registry_tables.c',
    'src_py_files': [
      '<(registry_tables_generator_root)/registry_tables_generator.py',
      '<(registry_tables_generator_root)/node_table_builder.py',
      '<(registry_tables_generator_root)/string_table_builder.py',
      '<(registry_tables_generator_root)/table_serializer.py',
      '<(registry_tables_generator_root)/test_table_builder.py',
      '<(registry_tables_generator_root)/trie_node.py',
    ],
  },
  'targets': [
    {
      'target_name': 'registry_tables_generator',
      'type': 'none',
      'hard_dependency': 1,
      'sources': [
        '<(in_dat_file)',
      ],
      'rules': [
        {
          'rule_name': 'registry_tables_generator',
          'extension': 'dat',
          'inputs': [
            '<@(src_py_files)',
            '<(executable)',
            '<(in_dat_file)',
          ],
          'outputs': [
            '<(out_registry_file)',
            '<(out_registry_test_file)',
          ],
          'action': [
            'python',
            '<(executable)',
            '<(in_dat_file)',
            '<(out_registry_file)',
            '<(out_registry_test_file)',
          ],
          'message': 'Generating C code from <(RULE_INPUT_PATH)',
        },
      ],
    },
  ],
}

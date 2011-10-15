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
    'variables': {
      'domain_registry_provider_dat_file_path%': '../third_party/effective_tld_names/effective_tld_names.dat',
      'domain_registry_provider_out_dir%': '<(SHARED_INTERMEDIATE_DIR)/registry_tables_generator_out',
    },

    'chromium_code': 1,
    'out_dir%': '<(domain_registry_provider_out_dir)',
    'executable': 'registry_tables_generator.py',
    'in_dat_file%': '<(domain_registry_provider_dat_file_path)',
    'out_registry_file': '<(domain_registry_provider_out_dir)/registry_tables_genfiles/registry_tables.h',
    'out_registry_test_file': '<(domain_registry_provider_out_dir)/registry_tables_genfiles/test_registry_tables.h',
    'src_py_files': [
      'registry_tables_generator.py',
      'node_table_builder.py',
      'string_table_builder.py',
      'table_serializer.py',
      'test_table_builder.py',
      'trie_node.py',
    ],
  },
  'targets': [
    {
      'target_name': 'generate_registry_tables',
      'type': 'none',
      'hard_dependency': 1,
      'sources': [
        '<(in_dat_file)',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<(out_dir)',
        ]
      },
      'rules': [
        {
          'rule_name': 'generate_registry_tables',
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

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

#ifndef DOMAIN_REGISTRY_DOMAIN_REGISTRY_H_
#define DOMAIN_REGISTRY_DOMAIN_REGISTRY_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Call once at program startup to enable domain registry
// search. Calls to GetRegistryLength will crash if this is not
// called.
void InitializeDomainRegistry(void);

// Finds the length in bytes of the registrar portion of the host in
// the given hostname.  Returns 0 if the hostname is invalid or has no
// host (e.g. a file: URL). Returns 0 if the hostname has multiple
// trailing dots, is an IP address, has no subcomponents, or is itself
// a recognized registry identifier. If no matching rule is found in
// the effective-TLD data, returns 0.
//
// Examples:
//   www.google.com       -> 3                 (com)
//   WWW.gOoGlE.cOm       -> 3                 (com, case insensitive)
//   ..google.com         -> 3                 (com)
//   google.com.          -> 4                 (com)
//   a.b.co.uk            -> 5                 (co.uk)
//   a.b.co..uk           -> 0                 (multiple dots in registry)
//   C:                   -> 0                 (no host)
//   google.com..         -> 0                 (multiple trailing dots)
//   192.168.0.1          -> 0                 (IP address)
//   bar                  -> 0                 (no subcomponents)
//   co.uk                -> 0                 (host is a registry)
//   foo.bar              -> 0                 (not a valid top-level registry)
size_t GetRegistryLength(const char* hostname);

// Like GetRegistryLength, but allows unknown registries as well. If
// the hostname is part of a known registry, the return value will be
// identical to that of GetRegistryLength. If the hostname is not part
// of a known registry (e.g. foo.bar) then the return value will
// assume that the rootmost hostname-part is the registry.
//
// Examples:
//   foo.bar              -> 3                 (bar)
//   bar                  -> 0                 (host is a registry)
//   www.google.com       -> 3                 (com)
//   com                  -> 0                 (host is a registry)
//   co.uk                -> 0                 (host is a registry)
size_t GetRegistryLengthAllowUnknownRegistries(const char* hostname);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // DOMAIN_REGISTRY_DOMAIN_REGISTRY_H_

// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SAMPLING_HEAP_PROFILER_MODULE_CACHE_H_
#define BASE_SAMPLING_HEAP_PROFILER_MODULE_CACHE_H_

#include <map>
#include <string>
#include <vector>

#include "base/base_export.h"
#include "base/files/file_path.h"
#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

namespace base {

class BASE_EXPORT ModuleCache {
 public:
  // Module represents the module (DLL or exe) and its validness state.
  // This struct is used for sampling data transfer from NativeStackSampler
  // to ProfileBuilder as well as by SamplingHeapProfiler.
  struct BASE_EXPORT Module {
    Module();
    Module(uintptr_t base_address,
           const std::string& id,
           const FilePath& filename);
    Module(uintptr_t base_address,
           const std::string& id,
           const FilePath& filename,
           size_t size);
    ~Module();

    // Points to the base address of the module.
    uintptr_t base_address;

    // An opaque binary string that uniquely identifies a particular program
    // version with high probability. This is parsed from headers of the loaded
    // module.
    // For binaries generated by GNU tools:
    //   Contents of the .note.gnu.build-id field.
    // On Windows:
    //   GUID + AGE in the debug image headers of a module.
    std::string id;

    // The filename of the module.
    FilePath filename;

    // The validness of the module.
    bool is_valid;

    // Size of the module.
    size_t size;
  };

  ModuleCache();
  ~ModuleCache();

  const Module& GetModuleForAddress(uintptr_t address);
  std::vector<const Module*> GetModules() const;

 private:
  // TODO(alph): Refactor corresponding functions to use public API instead,
  // and drop friends.

  // Creates a Module object for the specified memory address. If the address
  // does not belong to a module returns an invalid module.
  static Module CreateModuleForAddress(uintptr_t address);
  friend class NativeStackSamplerMac;

#if defined(OS_MACOSX)
  // Returns the size of the _TEXT segment of the module loaded
  // at |module_addr|.
  static size_t GetModuleTextSize(const void* module_addr);
  friend bool MayTriggerUnwInitLocalCrash(uint64_t);
#endif

#if defined(OS_WIN)
  static Module CreateModuleForHandle(HMODULE module_handle);
  friend class NativeStackSamplerWin;
#endif

  std::map<uintptr_t, Module> modules_cache_map_;
};

}  // namespace base

#endif  // BASE_SAMPLING_HEAP_PROFILER_MODULE_CACHE_H_

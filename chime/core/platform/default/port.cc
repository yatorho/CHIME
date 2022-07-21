// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include <cstdint>
#include <cstdlib>

#include "chime/core/memory/mem.h"
#include "chime/core/platform/cpu_info.h"
#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/numa.h"
#include "chime/core/platform/sys_info.h"
#include "hwloc/bitmap.h"

#if defined(__linux__)
#include <sched.h>
#include <sys/sysinfo.h>
#else
#include <sys/syscall.h>
#endif  // __linux__

#if defined(__WIN32__) || defined(__WIN64__)
#include <windows.h>
#endif  // __WIN32__ || __WIN64__

#if (__x86_64__ || __i386__)
#include <cpuid.h>
#endif  // __x86_64__ || __i386__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>

#if CHIME_USE_NUMA
#include "hwloc.h"  // from @hwloc
#endif              // CHIME_USE_NUMA

namespace chime {
namespace port {

std::string HostName() {
  char hostname[1024];
  gethostname(hostname, sizeof(hostname));
  hostname[sizeof(hostname) - 1] = '\0';
  return std::string(hostname);
}

std::string JobName() {
  const char *job_name_cs = std::getenv("CE_JOB_NAME");
  if (job_name_cs != nullptr) return std::string(job_name_cs);
  return std::string("");
}

int NumSchedulableCPUs() {
#if defined(__linux__)
  cpu_set_t cpuset;
  if (sched_getaffinity(0, sizeof(cpu_set_t), &cpuset) == 0) {
    return CPU_COUNT(&cpuset);
  }
  perror("sched_getaffinity");
#endif  // __linux__
  const int DEFAULT_CORES = 4;
  fprintf(stderr, "can't determine number of schedulable CPUs, using %d\n",
          DEFAULT_CORES);
  return DEFAULT_CORES;
}

int MaxParallelism() { return NumSchedulableCPUs(); }

int MaxParallelism(int numa_node) {
  if (numa_node != port::NUMA_NO_AFFINITY) {
    /// Assume that CPUs are equally distributed over available NUMA nodes.
    /// This may not be true, but there isn't currently a better way of
    /// determining the number of CPUs specific to the requested node.
    return NumSchedulableCPUs() / port::NUMANumNodes();
  }
  return NumSchedulableCPUs();
}

int NumTotalCPUs() {
#if defined(__linux__)
  return get_nprocs();
#elif defined(__WIN32__)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#endif  // __linux__
  return port::UNKNOWN_CPU;
}

int GetCurrentCPU() {
#if defined(__linux__)
  return sched_getcpu();
#elif defined(__WIN32__)
  return GetCurrentProcessorNumber();
#endif  // __linux__
  return port::UNKNOWN_CPU;
}

/// Returns num of hyperthreads per physical core
int NumHyperthreadsPerCore() {
#if defined(__linux__)
  return sysconf(_SC_NPROCESSORS_ONLN) / NumTotalCPUs();
#elif defined(__WIN32__)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#endif  // __linux__
  return 1;
}

#if CHIME_USE_NUMA
namespace {

static hwloc_topology_t hwloc_topology_handle;

bool HaveHwlocTopology() {
  static bool init = []() {
    if (hwloc_topology_init(&hwloc_topology_handle)) {
      LOG(ERROR) << "Call to hwloc_topology_init failed";
      return false;
    }
    if (hwloc_topology_load(hwloc_topology_handle)) {
      LOG(ERROR) << "Call to hwloc_topology_load failed";
      return false;
    }
    return true;
  }();
  return init;
}

/// Returns the first hwloc object of given type whose os_index matches `index`.
hwloc_obj_t GetHwlocTypeIndex(hwloc_obj_type_t tp, int index) {
  hwloc_obj_t obj = nullptr;
  unsigned int uindex = static_cast<unsigned int>(index);
  if (index >= 0) {
    while ((obj = hwloc_get_next_obj_by_type(hwloc_topology_handle, tp, obj)) !=
           nullptr) {
      // LOG(INFO) << "os_index: " << obj->os_index;
      if (obj->os_index == uindex) break;
    }
  }
  return obj;
}

}  // namespace
#endif  // CHIME_USE_NUMA

int NUMANumNodes() {
#if CHIME_USE_NUMA
  if (HaveHwlocTopology()) {
    int num_numa_nodes =
        hwloc_get_nbobjs_by_type(hwloc_topology_handle, HWLOC_OBJ_NUMANODE);
    return std::max(1, num_numa_nodes);
  } else
    return 1;
#else
  return 1;
#endif  // CHIME_USE_NUMA
}

void NUMASetThreadNodeAffinity(int node) {
#if CHIME_USE_NUMA
  if (HaveHwlocTopology()) {
    // Find the corresponding NUMA node topology object.
    hwloc_obj_t obj = GetHwlocTypeIndex(HWLOC_OBJ_NUMANODE, node);
    if (obj) {
      hwloc_set_cpubind(hwloc_topology_handle, obj->cpuset,
                        HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_STRICT);
    } else {
      LOG(ERROR) << "Could not find NUMA node " << node;
    }
  }
#endif  // CHIME_USE_NUMA
}

int NUMAGetThreadNodeAffinity() {
  int node_index = port::NUMA_NO_AFFINITY;
#if CHIME_USE_NUMA
  if (HaveHwlocTopology()) {
    hwloc_cpuset_t thread_cpuset = hwloc_bitmap_alloc();
    hwloc_get_cpubind(hwloc_topology_handle, thread_cpuset,
                      HWLOC_CPUBIND_THREAD);
    hwloc_obj_t obj = nullptr;
    // Return the first NUMA node whose cpuset is a (non-proper) superset of
    // that of current thread.
    while ((obj = hwloc_get_next_obj_by_type(
                hwloc_topology_handle, HWLOC_OBJ_NUMANODE, obj)) != nullptr) {
      if (hwloc_bitmap_isincluded(thread_cpuset, obj->cpuset)) {
        node_index = obj->os_index;
        break;
      }
    }
    hwloc_bitmap_free(thread_cpuset);
  }
#endif  // CHIME_USE_NUMA
  return node_index;
}

bool NUMAEnabled() { return NUMANumNodes() > 1; }

void *AlignedMalloc(size_t size, size_t alignment) {
  void *ptr = nullptr;
  const size_t required_alignment = sizeof(void *);
  // posix_memalign requires that the requested alignment be at least
  // sizeof(void*). In this case, fall back on malloc which should return
  // memory aligned to at least the size of a pointer.
  if (alignment < required_alignment) return Malloc(size);
  if (size % alignment != 0) {
    size += alignment - (size % alignment);
  }
  if (posix_memalign(&ptr, alignment, size) != 0) {
    return nullptr;
  }
  return ptr;
}

void AlignedFree(void *ptr) { Free(ptr); }

void *Malloc(size_t size) { return ::malloc(size); }

void *Realloc(void *ptr, size_t new_size) { return ::realloc(ptr, new_size); }

void Free(void *ptr) { ::free(ptr); }

void *NUMAMalloc(int node, size_t size, int minimum_alignment) {
#if CHIME_USE_NUMA
  if (HaveHwlocTopology()) {
    hwloc_obj_t numa_node = GetHwlocTypeIndex(HWLOC_OBJ_NUMANODE, node);
    if (numa_node != nullptr) {
      return hwloc_alloc_membind(hwloc_topology_handle, size,
                                 numa_node->nodeset, HWLOC_MEMBIND_BIND,
                                 HWLOC_MEMBIND_BYNODESET);
    } else {
      LOG(ERROR) << "Could not find hwloc NUMA node " << node;
    }
  }
#endif  // CHIME_USE_NUMA
  return AlignedMalloc(size, minimum_alignment);
}

size_t GetAllocatedSize(const void *ptr, size_t requested, size_t alignment) {
  size_t allocated = requested;
  const size_t required_alignment = sizeof(void *);
  if (alignment < required_alignment) alignment = required_alignment;
  if (requested % alignment != 0) {
    allocated += alignment - (requested % alignment);
  }
  return allocated;
}

void NUMAFree(void *ptr, size_t size) {
#if CHIME_USE_NUMA
  if (HaveHwlocTopology()) {
    hwloc_free(hwloc_topology_handle, ptr, size);
    return;
  }
#endif  // CHIME_USE_NUMA
  Free(ptr);
}

int NUMAGetMemAffinity(const void *ptr) {
  int node = port::NUMA_NO_AFFINITY;
#if CHIME_USE_NUMA
  if (HaveHwlocTopology() && ptr) {
    hwloc_nodeset_t nodeset = hwloc_bitmap_alloc();
    if (!hwloc_get_area_memlocation(hwloc_topology_handle, ptr, 4, nodeset,
                                    HWLOC_MEMBIND_BYNODESET)) {
      hwloc_obj_t obj = nullptr;
      while ((obj = hwloc_get_next_obj_by_type(
                  hwloc_topology_handle, HWLOC_OBJ_NUMANODE, obj)) != nullptr) {
        if (hwloc_bitmap_isincluded(nodeset, obj->nodeset)) {
          node = obj->os_index;
          break;
        }
      }
    }
  }
#endif  // CHIME_USE_NUMA
  return node;
}

MemoryInfo GetMemoryInfo() {
  MemoryInfo mem_info = {INT64_MAX, INT64_MAX};
#if defined(__linux__)
  struct sysinfo info;
  int err = sysinfo(&info);
  if (err == 0) {
    mem_info.total = info.totalram;
    mem_info.free = info.freeram;
  }
#endif  // __linux__
  return mem_info;
}

MemoryBandwidthInfo GetMemoryBandwidthInfo() {
  MemoryBandwidthInfo membw_info = {INT64_MAX};
  return membw_info;
}

}  // namespace port
}  // namespace chime

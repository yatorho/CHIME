// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_CPU_INFO_H_
#define CHIME_CORE_PLATFORM_CPU_INFO_H_


namespace chime {
namespace port {

/// Returns an estimate of the number of schedulable CPUs for this
/// process. Usually, it's constant throughout the lifetime of a
/// process, but it might change if the underlying cluster management
/// software can change it dynamically. If the underlying call fails, a default
/// value (e.g. `4`) may be returned.
int NumSchedulableCPUs();

/// Returns an estimate for the maximum parallelism for this process.
/// Applications should avoid running more than this number of threads with
/// intensive workloads concurrently to avoid performance degradation and
/// contention. This value is either the number of schedulable CPUs, or a value
/// specific to the underlying cluster management. Applications should assume
/// this value can change throughout the lifetime of the process. This function
/// must not be called during initialization, i.e., before main() has started.
int MaxParallelism();

/// Returns an estimate for the maximum parallelism for this process on the
/// provided numa node, or any numa node if `numa_node` is NUMA_NO_AFFINITY.
/// See MaxParallelism() for more information.
int MaxParallelism(int numa_node);

static constexpr int UNKNOWN_CPU = -1;

// Returns the total number of CPUs on the system. This number should
// not change even if the underlying cluster management software may
// change the number of schedulable CPUs. Unlike `NumSchedulableCPUs`, if the
// underlying call fails, an invalid value of -1 will be returned;
// the user must check for validity.
int NumTotalCPUs();

int GetCurrentCPU();

/// Returns an estimate of the number of hyperthreads per core.
int NumHyperthreadsPerCore();

/// Returns nominal CPU frequency in Hz of each processor.
double NominalCPUFrequency();

}  // namespace port
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_CPU_INFO_H_

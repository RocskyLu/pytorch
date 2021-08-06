#pragma once
#include <torch/csrc/autograd/profiler_kineto.h>
#include <torch/csrc/jit/mobile/module.h>

namespace torch {
namespace jit {
namespace mobile {
class TORCH_API KinetoEdgeCPUProfiler {
 public:
  // This profiler only profiles KINETO events
  // No GPU_FALLBACK or NVTX
  /*
   * @param m is the instance of mobile Module which is being profiled.
   *        Note that this implies that KinetoEdgeCPUProfiler can be used
   *        to profile specific Module (see usage below), unliked ProfilerKineto
   *        which can profile pytorch runtime in arbitrary scope.
   * @param fname is the name of the file to which chrome trace is written.
   * @param report_input_shapes: whether to record shapes of op's inputs.
   * @param with_stack: whether to record model's python stacktrace for the op.
   * @param with_flops: whether to report flops corresponding to the op.
   * @param with_module_hierarchy: whether to report original python module
   *        hierarchy to which the op belongs.
   *
   * Usage pattern for this profiler must be as follows:
   *
   * {
   *   KinetoEdgeCPUProfiler(m, filename, args);
   *   m.forward(...);
   * }
   *
   * The reason being that KinetoEdgeCPUProfiler has a dependency on Module
   * and thus it must not outlive it.
   *
   * Thus, when KinetoEdgeCPUProfiler is used as RAII to do profiling
   * within certain scope. In that scope, the captured reference to
   * Module will outlive KinetoEdgeCPUProfiler. This is gauranteed because
   * KinetoEdgeCPUProfiler must be constructed later than Module, on stack.
   *
   * An example of the anti-pattern and wrong usage is:
   *
   * std::shared_ptr<KinetoMobileCPUProfiler> profiler(m, filename, args);
   * m.forward(...);
   *
   * Since KinetoEdgeCPUProfiler object would then be constructed on heap
   * with its lifetime managed manually or via smart pointers.
   */
  KinetoEdgeCPUProfiler(
      const torch::jit::mobile::Module& m,
      const std::string& fname,
      const bool report_input_shapes = false,
      const bool profile_memory = false,
      const bool with_stack = false,
      const bool with_flops = false,
      const bool with_module_hierarchy = false);

  ~KinetoEdgeCPUProfiler();

 private:
  /*
   * We store a reference to Module to make such dependency explicit, since
   * a Module reference is already stored in a functor.
   */
  const mobile::Module& m_;
  std::string trace_file_name_;
};
} // namespace mobile
} // namespace jit
} // namespace torch

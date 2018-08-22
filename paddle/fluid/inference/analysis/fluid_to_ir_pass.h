// Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "paddle/fluid/inference/analysis/ir_pass_manager.h"
#include "paddle/fluid/inference/analysis/pass.h"

namespace paddle {
namespace inference {
namespace analysis {

class FluidToIrPass final : public DataFlowGraphPass {
 public:
  FluidToIrPass() = default;

  bool Initialize(Argument *argument) override {
    ANALYSIS_ARGUMENT_CHECK_FIELD(argument);
    if (argument->origin_program_desc) {
      LOG(WARNING) << "argument's origin_program_desc is already set, might "
                      "duplicate called";
    }
    // set fluid model program path
    if (!argument->fluid_model_program_path) {
      ANALYSIS_ARGUMENT_CHECK_FIELD(argument->fluid_model_dir);
      argument->fluid_model_program_path.reset(
          new std::string(*argument->fluid_model_dir + "/__model__"));
    }
    ANALYSIS_ARGUMENT_CHECK_FIELD(argument->fluid_model_program_path);
    // Load program.
    auto program = LoadProgramDesc(*argument->fluid_model_program_path);
    argument->origin_program_desc.reset(
        new framework::proto::ProgramDesc(program));
    // Create main data flow graph.
    if (!argument->main_dfg) {
      argument->main_dfg.reset(new DataFlowGraph);
    }
    argument->Set("ir_program_desc", new framework::ProgramDesc(program));
    argument_ = argument;
    return true;
  }

  bool Finalize() override { return true; }

  void Run(DataFlowGraph *graph) override {
    // Call all the IR Passes
    IRPassManager ir_passes(argument_->Get<framework::ProgramDesc>("ir_program_desc"), nullptr);
    ir_passes.Apply(std::vector<std::string>(
        {// Manual update the passes here.
         "graph_viz_pass", "infer_clean_graph_pass", "graph_viz_pass",
         "fc_fuse_pass", "graph_viz_pass"}));

    PADDLE_ENFORCE(argument_->main_dfg.get());
    argument_->main_dfg->Build(ir_passes.graph());
  }

  void EnableParamModify(const std::string &model_dir,
                         const std::string &prog_file,
                         const std::string &param_file);

  std::string repr() const override { return "fluid-to-ir-pass"; }

 private:
  // Load parameters from a single file or from a directory.
  void LoadParams(framework::Scope *scope, const std::string &dir,
                  const std::string &prog_file, const std::string &param_file);

 private:
  Argument *argument_{nullptr};
};

}  // namespace analysis
}  // namespace inference
}  // namespace paddle

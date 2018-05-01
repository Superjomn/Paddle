/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include <iosfwd>
#include "paddle/fluid/inference/analysis/helper.h"
#include "paddle/fluid/inference/analysis/node.h"

namespace paddle {
namespace inference {
namespace analysis {

class Pass {
 public:
  // Virtual method overridden by subclasses to do only necessary initialization
  // before any pass is run.
  virtual bool Initialize() = 0;

  // Virtual method overriden by subclasses to do any necessary clean up after
  // all passes have run.
  virtual bool Finalize() = 0;

  // Get a Pass appropriate to print the Node this pass operates on.
  virtual Pass *CreatePrinterPass(std::ostream &os,
                                  const std::string &banner) const = 0;

 private:
  PADDLE_DISALLOW_COPY_AND_ASSIGN(Pass);
};

// NodePass process on any Node types.
class NodePass : public Pass {
 public:
  virtual void Run(Node &node) = 0;
};

// NodePass process on any Function node types.
class FunctionPass : Pass {
 public:
  virtual void Run(Function &node) = 0;
};

// NodePass process on any FunctionBlock node types.
class FunctionBlockPass : Pass {
 public:
  virtual void Run(FunctionBlock &node) = 0;
};

}  // namespace analysis
}  // namespace inference
}  // namespace paddle

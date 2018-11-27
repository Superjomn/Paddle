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
#include "paddle/fluid/operators/impl/load_combine.h"
#include <fstream>
#include "paddle/fluid/framework/data_type_transform.h"
#include "paddle/fluid/framework/op_registry.h"
#include "paddle/fluid/platform/device_context.h"

namespace paddle {
namespace operators {

class LoadCombineOp : public framework::OperatorBase {
 public:
  LoadCombineOp(const std::string &type,
                const framework::VariableNameMap &inputs,
                const framework::VariableNameMap &outputs,
                const framework::AttributeMap &attrs)
      : OperatorBase(type, inputs, outputs, attrs) {}

 private:
  void RunImpl(const framework::Scope &scope,
               const platform::Place &place) const override {
    auto filename = Attr<std::string>("file_path");
    auto load_as_fp16 = Attr<bool>("load_as_fp16");

    std::ifstream fin(filename);
    PADDLE_ENFORCE(static_cast<bool>(fin),
                   "Cannot open file %s for load_combine op", filename);

    auto out_var_names = Outputs("Out");
    PADDLE_ENFORCE_GT(
        static_cast<int>(out_var_names.size()), 0,
        "The number of output variables should be greater than 0.");

    impl::LoadParamsFromStream(out_var_names, place, load_as_fp16, &fin,
                               &scope);
  }
};

class LoadCombineOpProtoMaker : public framework::OpProtoAndCheckerMaker {
 public:
  void Make() override {
    AddOutput(
        "Out",
        "(vector) The output LoDTensors that will be read from the input file.")
        .AsDuplicable();
    AddAttr<bool>(
        "load_as_fp16",
        "(boolean, default false)"
        "If true, the tensor will be first loaded and then "
        "converted to float16 data type. Otherwise, the tensor will be "
        "directly loaded without data type conversion.")
        .SetDefault(false);
    AddAttr<std::string>("file_path",
                         "(string) "
                         "LoDTensors will be loaded from \"file_path\".")
        .AddCustomChecker(
            [](const std::string &path) { return !path.empty(); });
    AddComment(R"DOC(
LoadCombine Operator.

LoadCombine operator loads LoDTensor variables from a file. The file should 
contain one or more LoDTensors serialized using the SaveCombine operator. The 
LoadCombine operator applies a deserialization strategy to appropriately load 
the LodTensors, and this strategy complements the serialization strategy used 
in the SaveCombine operator. Hence, the LoadCombine operator is tightly coupled
with the SaveCombine operator, and can only deserialize one or more LoDTensors 
that were saved using the SaveCombine operator.

)DOC");
  }
};
}  // namespace operators
}  // namespace paddle
namespace ops = paddle::operators;

REGISTER_OPERATOR(load_combine, ops::LoadCombineOp,
                  ops::LoadCombineOpProtoMaker);

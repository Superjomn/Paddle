# Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function

import unittest
import numpy as np

from op_test import OpTest


# Correct: General.
class TestUnsqueezeOp(OpTest):
    def setUp(self):
        self.init_test_case()
        self.op_type = "unsqueeze"
        self.inputs = {"X": np.random.random(self.ori_shape).astype("float32")}
        self.init_attrs()
        self.outputs = {"Out": self.inputs["X"].reshape(self.new_shape)}

    def test_check_output(self):
        self.check_output()

    def test_check_grad(self):
        self.check_grad(["X"], "Out")

    def init_test_case(self):
        self.ori_shape = (3, 5)
        self.axes = (1, 2)
        self.new_shape = (3, 1, 1, 5)

    def init_attrs(self):
        self.attrs = {"axes": self.axes, "inplace": False}


# Correct: Single input index.
class TestUnsqueezeOp1(TestUnsqueezeOp):
    def init_test_case(self):
        self.ori_shape = (3, 5)
        self.axes = (-1, )
        self.new_shape = (3, 5, 1)


# Correct: Mixed input axis.
class TestUnsqueezeOp2(TestUnsqueezeOp):
    def init_test_case(self):
        self.ori_shape = (3, 5)
        self.axes = (0, -1)
        self.new_shape = (1, 3, 5, 1)


# Correct: There is duplicated axis.
class TestUnsqueezeOp3(TestUnsqueezeOp):
    def init_test_case(self):
        self.ori_shape = (3, 2, 5)
        self.axes = (0, 3, 3)
        self.new_shape = (1, 3, 2, 1, 1, 5)


# Correct: Reversed axes.
class TestUnsqueezeOp4(TestUnsqueezeOp):
    def init_test_case(self):
        self.ori_shape = (3, 2, 5)
        self.axes = (3, 1, 1)
        self.new_shape = (3, 1, 1, 2, 5, 1)


# Correct: Inplace.
class TestUnsqueezeOpInplace1(TestUnsqueezeOp):
    def init_test_case(self):
        self.ori_shape = (3, 5)
        self.axes = (0, 2)
        self.new_shape = (1, 3, 1, 5)

    def init_attrs(self):
        self.attrs = {"axes": self.axes, "inplace": True}


# Correct: Inplace. There is mins index.
class TestUnsqueezeOpInplace2(TestUnsqueezeOp):
    def init_test_case(self):
        self.ori_shape = (3, 5)
        self.axes = (0, -2)
        self.new_shape = (1, 3, 1, 5)

    def init_attrs(self):
        self.attrs = {"axes": self.axes, "inplace": True}


# Correct: Inplace. There is duplicated axis.
class TestUnsqueezeOpInplace3(TestUnsqueezeOp):
    def init_test_case(self):
        self.ori_shape = (3, 2, 5)
        self.axes = (0, 3, 3)
        self.new_shape = (1, 3, 2, 1, 1, 5)

    def init_attrs(self):
        self.attrs = {"axes": self.axes, "inplace": True}


if __name__ == "__main__":
    unittest.main()

/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GELU_H_
#define MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GELU_H_

#include <vector>
#include "src/runtime/kernel/arm/base/gelu_base.h"
#include "src/lite_kernel.h"

namespace mindspore::kernel {
class GeLUCPUKernel : public GeLUBaseCPUKernel {
 public:
  GeLUCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx,
                const mindspore::lite::PrimitiveC *primitive)
      : GeLUBaseCPUKernel(parameter, inputs, outputs, ctx, primitive) {}
  ~GeLUCPUKernel() override = default;

  int Init() override;
  int ReSize() override;
  int Run() override;
  int GeLU(int task_id);

 private:
  float *input_ptr_ = nullptr;
  float *output_ptr_ = nullptr;
  int64_t elements_num_ = 0;
  int64_t count_unit_ = 0;
};
}  // namespace mindspore::kernel

#endif  // MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GELU_H_

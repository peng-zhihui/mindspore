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
#include <vector>
#include "include/errorcode.h"
#include "src/kernel_registry.h"
#include "src/runtime/kernel/arm/fp32/TensorListReserve.h"

using mindspore::kernel::KERNEL_ARCH::kCPU;
using mindspore::lite::KernelRegistrar;
using mindspore::lite::RET_ERROR;
using mindspore::lite::RET_NULL_PTR;
using mindspore::lite::RET_OK;
using mindspore::schema::PrimitiveType_TensorListReserve;

namespace mindspore::kernel {

int TensorListReserveCPUKernel::Init() { return RET_OK; }

int TensorListReserveCPUKernel::Run() {
  auto output = reinterpret_cast<lite::TensorList *>(out_tensors_[0]);
  output->set_tensors_data_type(element_dtype_);
  return RET_OK;
}

int TensorListReserveCPUKernel::ReSize() { return RET_OK; }

kernel::LiteKernel *CpuTensorListReserveFp32KernelCreator(const std::vector<lite::Tensor *> &inputs,
                                                          const std::vector<lite::Tensor *> &outputs,
                                                          OpParameter *op_parameter, const lite::InnerContext *ctx,
                                                          const kernel::KernelKey &desc,
                                                          const mindspore::lite::PrimitiveC *primitive) {
  if (op_parameter == nullptr) {
    MS_LOG(ERROR) << "Input op_parameter is nullptr!";
    return nullptr;
  }
  if (ctx == nullptr) {
    MS_LOG(ERROR) << "Input context is nullptr!";
    free(op_parameter);
    return nullptr;
  }
  MS_ASSERT(desc.type == schema::PrimitiveType_TensorListSetItem);
  auto *kernel = new (std::nothrow) TensorListReserveCPUKernel(op_parameter, inputs, outputs, ctx, primitive);
  if (kernel == nullptr) {
    MS_LOG(ERROR) << "new TensorListReserveCPUKernel fail!";
    free(op_parameter);
    return nullptr;
  }
  auto ret = kernel->Init();
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "Init kernel failed! name: " << op_parameter->name_ << ", type: "
                  << schema::EnumNamePrimitiveType(static_cast<schema::PrimitiveType>(op_parameter->type_));
    delete kernel;
    return nullptr;
  }
  return kernel;
}

REG_KERNEL(kCPU, kNumberTypeFloat32, PrimitiveType_TensorListReserve, CpuTensorListReserveFp32KernelCreator)
}  // namespace mindspore::kernel

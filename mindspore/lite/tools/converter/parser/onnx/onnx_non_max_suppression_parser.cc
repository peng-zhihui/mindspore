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

#include "tools/converter/parser/onnx/onnx_non_max_suppression_parser.h"
#include <memory>

namespace mindspore {
namespace lite {
STATUS OnnxNonMaxSuppressionParser::Parse(const onnx::GraphProto &onnx_graph, const onnx::NodeProto &onnx_node,
                                          schema::CNodeT *op) {
  MS_LOG(DEBUG) << "onnx EluParser";
  if (op == nullptr) {
    MS_LOG(ERROR) << "op is null";
    return RET_NULL_PTR;
  }
  op->primitive = std::make_unique<schema::PrimitiveT>();
  if (op->primitive == nullptr) {
    MS_LOG(ERROR) << "op->primitive is null";
    return RET_NULL_PTR;
  }

  std::unique_ptr<schema::NonMaxSuppressionT> attr = std::make_unique<schema::NonMaxSuppressionT>();
  if (attr == nullptr) {
    MS_LOG(ERROR) << "new op failed";
    return RET_NULL_PTR;
  }
  if (onnx_node.input_size() > 2) {
    auto it = std::find_if(onnx_graph.initializer().begin(), onnx_graph.initializer().end(),
                           [&](const onnx::TensorProto &it) { return it.name() == onnx_node.input(2); });
    if (it != onnx_graph.initializer().end()) {
      attr->maxOutBoxPerClass = it->int64_data(0);
    }
  }

  if (onnx_node.input_size() > 3) {
    auto it = std::find_if(onnx_graph.initializer().begin(), onnx_graph.initializer().end(),
                           [&](const onnx::TensorProto &it) { return it.name() == onnx_node.input(3); });
    if (it != onnx_graph.initializer().end()) {
      attr->iouThreshold = it->float_data(0);
    }
  }

  if (onnx_node.input_size() > 4) {
    auto it = std::find_if(onnx_graph.initializer().begin(), onnx_graph.initializer().end(),
                           [&](const onnx::TensorProto &it) { return it.name() == onnx_node.input(4); });
    if (it != onnx_graph.initializer().end()) {
      attr->scoreThreshold = it->float_data(0);
    }
  }

  for (const auto &onnx_node_attr : onnx_node.attribute()) {
    const auto &attribute_name = onnx_node_attr.name();
    if (attribute_name == "center_point_box") {
      if (onnx_node_attr.has_i()) {
        attr->centerPointBox = onnx_node_attr.i();
      }
    }
  }

  op->primitive->value.type = schema::PrimitiveType_Elu;
  op->primitive->value.value = attr.release();
  return RET_OK;
}

OnnxNodeRegistrar g_onnxNonMaxSuppressionParser("NonMaxSuppression", new OnnxNonMaxSuppressionParser());
}  // namespace lite
}  // namespace mindspore
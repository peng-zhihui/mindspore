/**
 * Copyright 2020 Huawei Technologies Co., Ltd

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef MINDSPORE_CCSRC_BACKEND_OPTIMIZER_SOMAS_SOMAS_SOLVER_PRE_H_
#define MINDSPORE_CCSRC_BACKEND_OPTIMIZER_SOMAS_SOMAS_SOLVER_PRE_H_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>
#include "backend/session/kernel_graph.h"

using std::unordered_map;
using std::vector;

namespace mindspore {
namespace somas {
enum Status { FAILED, SUCCESS };
enum AlgorithmType { kManyObjects = 0, kSingleObject, kNumAlgorithmTypes };
enum SortingType {
  kGreaterSizeSmallerIndex = 0,
#ifdef SOMAS_DEBUG
  kGreaterSizeGreaterIndex,
  kGreaterSizeSmallerConstraintsSmallerIndex,
  kGreaterSizeSmallerConstraintsGreaterIndex,
  kGreaterSizeGreaterConstraintsSmallerIndex,
  kGreaterSizeGreaterConstraintsGreaterIndex,
#endif
  kNumSortingTypes
};
enum FittingType {
  kBest = 0,
  kSmallest,
#ifdef SOMAS_DEBUG
  kLargest,
  kWorst,
#endif
  kNumFittingTypes
};

class DynamicBitSet {
  const size_t bit_width_ = 64;
  size_t bit_size_;
  std::vector<uint64_t> bit_;

  inline size_t GetIndex(size_t index) { return index / bit_width_; }

  inline uint64_t GetBitMask(size_t index) { return (((uint64_t)0x1) << (bit_width_ - 1 - (index % bit_width_))); }

  inline void Reset(uint64_t val) {
    bit_.clear();
    for (size_t i = 0; i < bit_size_; i++) {
      bit_.push_back(val);
    }
  }

 public:
  explicit DynamicBitSet(size_t count) {
    bit_size_ = (count + bit_width_ - 1) / bit_width_;
    Reset(0x0);
  }

  void SetBitTrue(size_t index, bool log = false) {
    if (log) {
      MS_LOG(INFO) << GetIndex(index) << " " << GetBitMask(index);
    }
    bit_[GetIndex(index)] |= GetBitMask(index);
  }

  void SetBitFalse(size_t index) { bit_[GetIndex(index)] &= (~GetBitMask(index)); }

  bool IsBitTrue(size_t index) { return (bit_[GetIndex(index)] & GetBitMask(index)) != 0x0; }

  void Log() {
    std::cout << "Start Print Bitset ";
    for (size_t i = 0; i < bit_size_; i++) {
      std::cout << " bit [" << std::dec << i << "] = " << std::hex << bit_[i] << std::dec;
    }
    std::cout << std::endl;
  }

  friend void Union(DynamicBitSet *a, DynamicBitSet *b) {
    for (size_t i = 0; i < (*a).bit_size_; i++) {
      (*a).bit_[i] |= (*b).bit_[i];
    }
  }
};

struct SomasSolverTensorDesc {
  size_t index_;
  size_t size_;
  size_t offset_;
  bool lifelong_;
  size_t constraints_;
  using SomasSolverTensorDescPtr = std::shared_ptr<SomasSolverTensorDesc>;
  SomasSolverTensorDescPtr right_;
  SomasSolverTensorDescPtr left_;
  bool blocked_;

  SomasSolverTensorDesc() = default;

  SomasSolverTensorDesc(size_t index, size_t size, size_t offset, bool blifelong)
      : index_(index), size_(size), offset_(offset), lifelong_(blifelong) {
    constraints_ = 0;
    right_ = NULL;
    left_ = NULL;
    blocked_ = false;
  }

  void Update(size_t index, size_t size, size_t offset, bool blifelong, size_t constraints) {
    index_ = index;
    size_ = size;
    offset_ = offset;
    lifelong_ = blifelong;
    constraints_ = constraints;
  }

  friend std::ostream &operator<<(std::ostream &out, const SomasSolverTensorDescPtr n) {
    out << n->index_ << " " << n->size_ << " " << n->offset_ << "\n";
    return out;
  }
  friend std::istream &operator>>(std::istream &in, SomasSolverTensorDescPtr n) {
    in >> n->index_ >> n->size_ >> n->offset_;
    return in;
  }
};
using SomasSolverTensorDescPtr = std::shared_ptr<SomasSolverTensorDesc>;

class SomasSolverPre {
 public:
  SomasSolverPre() = default;
  ~SomasSolverPre() = default;

  SomasSolverPre(const SomasSolverPre &) = delete;
  SomasSolverPre &operator=(const SomasSolverPre &) = delete;

  size_t GetMaxOffset() { return max_offset_; }

  Status Solving(const session::KernelGraph *graph, std::unordered_map<size_t, SomasSolverTensorDescPtr> *tensors,
                 std::vector<DynamicBitSet> *pConstraints, const vector<vector<size_t>> &continuous_v,
                 bool bVerifySolution,  // true -> Check continuous and non overlapping constraints solution
                 bool ball = true,      // true -> run full set of heuristics, false -> run single heuristic specified
                 SortingType sorting = kGreaterSizeSmallerIndex, FittingType fitting = kBest,
                 AlgorithmType algorithm = kManyObjects);

  void Log(const session::KernelGraph *graph, const unordered_map<size_t, SomasSolverTensorDescPtr> &tensors,
           std::vector<DynamicBitSet> *pConstraints_v, const vector<vector<size_t>> &continuous_v);

 private:
  size_t max_offset_;
};
using SomasSolverPrePtr = std::shared_ptr<SomasSolverPre>;
}  // namespace somas
}  // namespace mindspore

#endif  // MINDSPORE_CCSRC_BACKEND_OPTIMIZER_SOMAS_SOMAS_SOLVER_PRE_H_

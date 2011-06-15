/* 
 *  Copyright (c) 2011 Daisuke Okanohara
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#ifndef DAG_VECTOR_HPP_
#define DAG_VECTOR_HPP_

#include <vector>
#include <stdint.h>
#include "rank_vector.hpp"

namespace dag{

/**
 * Direct Accessible Gamma code Vector
 */
class dag_vector{
public:
  /**
   * Constructor
   */
  dag_vector(): size_(0), max_shift_num_(0){
  }

  /**
   * Destructor
   */
  ~dag_vector(){
  }

  /**
   * Add element in a gamma code
   * @param val an element to be added
   */
  void push_back(uint64_t val){
    uint64_t val1 = val+1;
    uint64_t shift = 0;
    for (; (val1 >> shift) > 1; ++shift){}
    resize(shift);
    for (size_t i = 0; i < shift; ++i){
      bitvals_[i].push_back((val1 >> i) & 0x1LLU);
      bitunaries_[i].push_back(1);
    }
    if (shift == bitunaries_.size()){
      max_shift_num_++;
    } else {
      bitunaries_[shift].push_back(0);
    }
    size_++;
  }

  /**
   * Lookup the ind-th element
   * @param ind the index
   * @return the ind-th element
   */
  uint64_t operator[] (uint64_t ind) const{
    uint64_t val = 0;
    for (uint64_t shift = 0; shift < bitunaries_.size(); ++shift){
      if (!bitunaries_[shift].get_bit(ind)){
        return val + ((1LLU) << shift) - 1;
      }
      ind = bitunaries_[shift].rank(ind);
      val += bitvals_[shift].get_bit(ind) << shift;
    }
    val += (1LLU) << bitunaries_.size(); 

    return val-1;
  }

  /**
   * Compute the prefix sum: the sum of [0...ind-1] values.
   * O(log max_val) time.
   * @param ind the index
   * @return the sum of v[0] v[1] ... v[ind-1]
   */
  uint64_t sum(uint64_t ind) const{
    uint64_t orig_ind = ind;
    uint64_t ret = 0;
    for (uint64_t shift = 0; shift < bitunaries_.size(); ++shift){
      uint64_t ones = bitunaries_[shift].rank(ind);
      ret += (ind - ones) << shift;
      ret += bitvals_[shift].rank(ones) << shift;
      ind = ones;
    }
    ret += ind << bitunaries_.size();
    return ret - orig_ind;
  }

  /**
   * Compute the prefix sum and the value in O(log max_val) time.
   * @param ind the index
   * @return the pair of the prefix sum (sum of v[0] v[1] ... v[ind-1]) and v[ind]
   */
  std::pair<uint64_t, uint64_t> sumval(uint64_t ind) const{
    uint64_t orig_ind = ind;
    uint64_t sum = 0;
    uint64_t val = 0;
    bool val_finish = false;
    for (uint64_t shift = 0; shift < bitunaries_.size(); ++shift){
      uint64_t ones = bitunaries_[shift].rank(ind);
      sum += (ind - ones) << shift;
      if (!val_finish && !bitunaries_[shift].get_bit(ind)){
        val += (1LLU) << shift;
        val_finish = true;
      }
      sum += bitvals_[shift].rank(ones) << shift;
      if (!val_finish){
        val += bitvals_[shift].get_bit(ones) << shift;
      }
      ind = ones;
    }
    if (!val_finish ){
      val += (1LLU) << bitunaries_.size(); 
    }
    sum += ind << bitunaries_.size();
    return std::make_pair(sum - orig_ind, val-1);
  }

  /**
   * Return the number of elements
   * @return the number of elements
   */
  uint64_t size() const{
    return size_;
  }

  /**
   * Swap the content
   * @param dagv the dag_vector to be swapped
   */
  void swap(dag_vector& dagv){
    bitvals_.swap(dagv.bitvals_);
    bitunaries_.swap(dagv.bitunaries_);
    std::swap(size_, dagv.size_);
    std::swap(max_shift_num_, dagv.max_shift_num_);
  }

  /**
   * Clear the content
   */
  void clear() {
    std::vector<rank_vector>().swap(bitvals_);
    std::vector<rank_vector>().swap(bitunaries_);
    size_ = 0;
    max_shift_num_ = 0;
  }

  /**
   * Get the number of allocated bytes 
   */
  uint64_t get_alloc_byte_num() const{
    uint64_t byte_num = 0;
    for (size_t i = 0; i < bitvals_.size(); ++i){
      uint64_t block_num = (bitvals_[i].size() + 64 - 1) / 64;
      byte_num += sizeof(uint64_t) * block_num
        + sizeof(uint64_t) * (block_num / 4)
        + sizeof(uint8_t) * block_num;
    }
    for (size_t i = 0; i < bitunaries_.size(); ++i){
      uint64_t block_num = (bitunaries_[i].size() + 64 - 1) / 64;
      byte_num += sizeof(uint64_t) * block_num
        + sizeof(uint64_t) * (block_num / 4)
        + sizeof(uint8_t) * block_num;
    }
    return byte_num;
  }

  class const_iterator : public std::iterator<std::random_access_iterator_tag, uint64_t, size_t> {
  public:
    const_iterator(const dag_vector* v) : v_(v) {
      bitval_poses_.resize(v_->bitvals_.size());
      bitunary_poses_.resize(v_->bitunaries_.size());
      max_shift_pos_ = 0;
      cur_shift_ = cur_shift();
    }

    const_iterator& end(){
      for (size_t i = 0; i < bitval_poses_.size(); ++i){
        bitval_poses_[i] = v_->bitvals_[i].size();

      }

      for (size_t i = 0; i < bitunary_poses_.size(); ++i){
        bitunary_poses_[i] = v_->bitunaries_[i].size();
      }

      max_shift_pos_ = v_->max_shift_num_;
      cur_shift_ = bitval_poses_.size();
      return *this;
    }

    const_iterator& operator++(){
      for (size_t i = 0; i < bitunary_poses_.size(); ++i){
        ++bitunary_poses_[i];
        if (i == cur_shift_){
          break;
        }
        ++bitval_poses_[i];
      }
      if (cur_shift_ == bitunary_poses_.size()){
        ++max_shift_pos_;
      }
      cur_shift_ = cur_shift(); 
      return *this;
    }
    const_iterator operator++(int){
      const_iterator tmp(*this);
      ++*this;
      return tmp;
    }

    const_iterator& operator--(){
      for (size_t i = 0; ; ++i){
        --bitunary_poses_[i];
        if (i == cur_shift_){
          break;
        }
        --bitval_poses_[i];
      }
      if (cur_shift_ == bitunary_poses_.size()){
        --max_shift_pos_;
      }

      cur_shift_ = cur_shift();
      return *this;
    }

    const_iterator operator--(int){
      const_iterator tmp(*this);
      --*this;
      return tmp;
    }

    size_t operator-(const const_iterator& it) const{
      if (bitunary_poses_.size() > 0){
        return bitunary_poses_[0] - it.bitunary_poses_[0];
      }  else {
        return max_shift_pos_ - it.max_shift_pos_;
      }
    }

    bool operator==(const const_iterator& it) const{
      if (v_ != it.v_) return false;
      if (bitval_poses_.size() != it.bitval_poses_.size()) return false; 
      for (size_t i = 0; i < bitval_poses_.size(); ++i){
        if (bitval_poses_[i] != it.bitval_poses_[i]) return false;
      }
      if (bitunary_poses_.size() != it.bitunary_poses_.size()) return false; 
      for (size_t i = 0; i < bitunary_poses_.size(); ++i){
        if (bitunary_poses_[i] != it.bitunary_poses_[i]) return false;
      }
      if (max_shift_pos_ != it.max_shift_pos_){
        return false;
      }
      return true;
    }

    bool operator!=(const const_iterator& it) const{
      return !(*this == it);
    }

    uint64_t operator*() const {
      uint64_t val = 0;
      uint64_t shift = 0;
      for ( ; shift < cur_shift_; ++shift){ 
        val += v_->bitvals_[shift].get_bit(bitval_poses_[shift]) << shift;
      }
      val += (1LLU) << shift;
      return val-1;
    }

  private:
    uint64_t cur_shift() const{
      for (uint64_t shift = 0; shift < v_->bitunaries_.size(); ++shift){
        if (!v_->bitunaries_[shift].get_bit(bitunary_poses_[shift])){
          return shift;
        }
      }
      return v_->bitunaries_.size();
    }

    const dag_vector* v_;  
    std::vector<uint64_t> bitval_poses_;
    std::vector<uint64_t> bitunary_poses_;
    uint64_t max_shift_pos_;
    uint64_t cur_shift_;
  };

  const_iterator begin() const{
    return const_iterator(this);
  }
  
  const_iterator end() const{
    const_iterator it = const_iterator(this);
    return it.end();
  }

private:
  void resize(uint64_t shift){
    uint64_t old_shift = bitunaries_.size();    
    if (shift > old_shift){
      bitunaries_.resize(shift);
      bitvals_.resize(shift);
      for (size_t i = 0; i < max_shift_num_; ++i){
        bitunaries_[old_shift].push_back(0);
      }
      max_shift_num_ = 0;
    }
  }

  std::vector<rank_vector> bitunaries_; /// unary codes
  std::vector<rank_vector> bitvals_;    /// value codes
  uint64_t size_;                       /// the number of codes
  uint64_t max_shift_num_;              /// the number of codes whose have the largest lengths
};


}

#endif // DAG_VECTOR_HPP_

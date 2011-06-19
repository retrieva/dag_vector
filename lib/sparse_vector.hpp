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

#ifndef SPARSE_VECTOR_HPP_
#define SPARSE_VECTOR_HPP_

#include "dag_vector.hpp"
#include "bit_vector.hpp"

namespace dag{

/**
 * Sparse Vector supporting rank/select operations
 */
class sparse_vector{
public:
  /**
   * Constructor
   */
  sparse_vector() {
    clear();
  }

  /**
   * Constructor given low_width
   * @param low_width the width of low array
   */
  sparse_vector(uint64_t low_width) {
    clear();
    low_width_ = low_width;
    mask_ = (low_width_ == BLOCK_SIZE) ? 0xFFFFFFFFFFFFFFFF : (1 << low_width_) - 1;    
  }

  /**
   * Constructor given one_num and num
   * @param one_num the number of expected ones in the array
   * @param num the number of expected bits in the array
   */
  sparse_vector(uint64_t one_num, uint64_t num){
    clear();
    if (one_num == 0) return;
    uint64_t dist = num / one_num;
    for (low_width_ = 0; dist >> low_width_; ++low_width_) {}
    mask_ = (low_width_ == BLOCK_SIZE) ? 0xFFFFFFFFFFFFFFFF : (1 << low_width_) - 1;    
  }

  /**
   * Desctructor
   */
  ~sparse_vector(){
  }

  /**
   * Clear sparse vector
   */
  void clear(){
    high_one_.clear();
    high_zero_.clear();
    low_.clear();
    one_num_ = 0;
    low_width_ = BLOCK_SIZE;
    prev_val_ = 0;
    run_ones_ = 0;
  }

  /**
   * Add element to the end of the array
   * @param val an element to be added
   */
  void push_back(uint64_t val){
    ++one_num_;
    uint64_t high = (low_width_ == BLOCK_SIZE) ? 0 : val >> low_width_;
    uint64_t prev_high = (low_width_ == BLOCK_SIZE) ? 0 : prev_val_ >> low_width_;
    uint64_t dist = high - prev_high;
    high_one_.push_back(dist);
    high_bv_.push_back(1LLU << dist, dist+1); // write 00..01
    if (dist > 0){
      high_zero_.push_back(run_ones_);
      run_ones_ = 0;
    }
    for (uint64_t i = 1; i < dist; ++i){
      high_zero_.push_back(0);
    }
    ++run_ones_;
    prev_val_ = val;
    low_.push_back(val & mask_, low_width_);
  }

  /**
   * Return the position of (rank+1)-th one in the array
   * @param rank the rank of the position
   * @return the position of (ran+1)-th one in the array
   */
  uint64_t select(uint64_t rank) const{
    uint64_t rank1 = rank + 1;
    uint64_t high = high_one_.prefix_sum(rank1);
    uint64_t low = low_.get_bits(rank * low_width_, low_width_);
    return (high << low_width_) + low;
  }

  /**
   * Return the pos-th bit
   * @param pos the position in the array
   * @return the bit at the pos-th position.
   */
  uint64_t get_bit(uint64_t pos) const{
    uint64_t high = (low_width_ == BLOCK_SIZE) ? 0 : pos >> low_width_;
    uint64_t low  = pos & mask_;
    uint64_t low_pos = high_zero_.prefix_sum(high);
    uint64_t cur_rank = low_pos + high;
    for (; cur_rank < high_bv_.size(); cur_rank++, low_pos++){
      if (!high_bv_.get_bit(cur_rank)) {
        break;
      }
      uint64_t cur_low = low_.get_bits(low_pos * low_width_, low_width_);
      if (cur_low >= low){
        if (cur_low == low){
          return 1;
        } else {
          return 0;
        }
      }
    }
    return 0;
  }

  /**
   * Return the number of ones in B_[0...pos-1]
   * @param pos the position in the array 
   * @return the number of ones in B_[0...pos-1]
   */
  uint64_t rank(uint64_t pos) const{
    uint64_t high = (low_width_ == BLOCK_SIZE) ? 0 : pos >> low_width_;
    uint64_t low  = pos & mask_;
    uint64_t low_pos = high_zero_.prefix_sum(high);
    uint64_t cur_rank = low_pos + high;
    for (; cur_rank < high_bv_.size(); cur_rank++, low_pos++){
      if (!high_bv_.get_bit(cur_rank)) {
        break;
      }
      uint64_t cur_low = low_.get_bits(low_pos * low_width_, low_width_);
      if (cur_low >= low){
        break;
      }
    }
    return cur_rank;
  }

  /**
   * 
   */
  uint64_t one_num() const {
    return one_num_;
  }

  uint64_t low_width() const {
    return low_width_;
  }

  void compress(){
    uint64_t size = high_one_.size();
    uint64_t total_sum = high_one_.sum();
    uint64_t dist = total_sum / size;
    uint64_t new_low_width = 0;
    for (new_low_width = 0; dist >> new_low_width; ++new_low_width) {}
    if (new_low_width == low_width_) return;
    
    sparse_vector new_sparse_vector(new_low_width);
    dag_vector::const_iterator old_it = high_one_.begin();
    uint64_t high = 0;
    for (uint64_t i = 0; i < one_num_; ++i, ++old_it){
      high += *old_it;
      uint64_t low = low_.get_bits(i * low_width_, low_width_);
      uint64_t val = (high << low_width_) + low;
      new_sparse_vector.push_back(val);
    }
    swap(new_sparse_vector);
  }

  uint64_t get_alloc_byte_num() const{
    return high_one_.get_alloc_byte_num()
      + high_zero_.get_alloc_byte_num()
      + low_.get_alloc_byte_num()
      + high_bv_.get_alloc_byte_num()
      + sizeof(uint64_t) * 3;
  }

  void swap(sparse_vector& ds){
    high_one_.swap(ds.high_one_);
    high_zero_.swap(ds.high_zero_);
    low_.swap(ds.low_);
    std::swap(one_num_, ds.one_num_);
    std::swap(low_width_, ds.low_width_);
    std::swap(prev_val_, ds.prev_val_);
    std::swap(run_ones_, ds.run_ones_);
    std::swap(mask_, ds.mask_);
  }

  class const_iterator : public std::iterator<std::random_access_iterator_tag, uint64_t, size_t> {
  public:
    const_iterator(const sparse_vector& ds) : 
      low_(ds.low_),
      high_bv_(ds.high_bv_),
      pos_(0),
      high_pos_(0),
      high_prefix_val_(0),
      one_num_(ds.one_num()),
      low_width_(ds.low_width()){
      advance();
    }
    
    const_iterator(const sparse_vector& ds, uint64_t) : 
      low_(ds.low_),
      high_bv_(ds.high_bv_),
      pos_(ds.one_num()),
      high_pos_(high_bv_.size()),
      high_prefix_val_(0),
      one_num_(ds.one_num()),
      low_width_(ds.low_width()){
    } 

    const_iterator& operator++(){
      ++pos_;
      advance();
      return *this;
    }

    const_iterator operator++(int){
      const_iterator tmp(*this);
      ++*this;
      return tmp;
    }

    /*
    const_iterator& operator--(){
      --dvit_;
      --pos_;
      high_ -= *dvit_;
      return *this;
    }

    const_iterator operator--(int){
      const_iterator tmp(*this);
      --*this;
      return tmp;
    }
    */

    size_t operator-(const const_iterator& it) const{
      return pos_ - it.pos_;
    }

    bool operator==(const const_iterator& it) const{
      return pos_ == it.pos_;
    }

    bool operator!=(const const_iterator& it) const{
      return !(*this == it);
    }

    uint64_t operator*() const {
      return (high_prefix_val_ << low_width_) + low_.get_bits(pos_ * low_width_, low_width_);
    }

  private:
    void advance(){
      for (;;++high_prefix_val_){
        if (high_bv_.get_bit(high_pos_++)){
          break;
        }
      }
    }

    const bit_vector& low_;
    const bit_vector& high_bv_;
    uint64_t pos_;
    uint64_t high_pos_;
    uint64_t high_prefix_val_;
    uint64_t one_num_;
    const uint64_t low_width_;
  };

  const_iterator begin() const{
    return const_iterator(*this);
  }
  
  const_iterator end() const{
    return const_iterator(*this, one_num_);
  }

private:
  static const uint64_t BLOCK_SIZE = 64;

  dag_vector high_one_;
  dag_vector high_zero_;
  bit_vector high_bv_;
  bit_vector low_;
  uint64_t one_num_;
  uint64_t low_width_;
  uint64_t prev_val_;
  uint64_t run_ones_;
  uint64_t mask_;
};

}

#endif // SPARSE_VECTOR_HPP_

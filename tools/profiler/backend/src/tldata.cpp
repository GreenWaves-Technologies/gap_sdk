/*
 * Copyright (C) 2019 Green_waves Technologies
 * All rights reserved.
 */

#include <cmath>
#include <iostream>
#include <algorithm>
#include "tldata.hpp"

template <typename T>
void print_dWT(const Data_with_time<T>& dwt, bool newline){
  if (dwt.mixed_up) std::cout << ANSI_COLOR_RED;
  else std::cout << ANSI_COLOR_GREEN;
  std::cout << "[" << dwt.begin << "; " << dwt.end << "; ";
  if (dwt.mixed_up)
    std::cout << "mixed-up";
  else std::cout << dwt.d ;
  std::cout << "]" << ANSI_COLOR_RESET;
  if (newline) std::cout << std::endl;
}

template <typename T>
TLData<T>::TLData(uint imin, uint resolution){
  this->imin = imin;
  this->resolution = resolution;
  last_event = 0;
  data.resize(imax);
}

/* item must be added in chronological order! */
template <typename T>
void TLData<T>::add_item(uint64_t begin, uint64_t end, T item){
  uint i;
  /* first find index i to add item to data[i] */
  if (end < begin){
    std::cout << "[-] Warning: end = " << end;
    std::cout << " < begin = " << begin << " ; item not added" << std::endl;
    return;
  }
  if (begin == end){
    i = imin;
  }
  else{
    i = (uint) std::log2((double) (end - begin));
    if (i < imin) i = imin;
    if (i >= imax) i = imax - 1; //should not happen
  }

  //std::cout << "adding [" << begin << "; " << end << "; " << item << "] at level " << i << std::endl;
  if (data[i].size() > 0 && data[i].back().end > begin && !data[i].back().mixed_up){
    std::cout << "[-] Warning: TLData.add_item: order not respected" << std::endl;
    std::cout << "    last item was: ";
    print_dWT(data[i].back());
    std::cout << "    currently trying to add ";
    print_dWT(Data_with_time<T> {.d = item, .begin = begin, .end = end, .mixed_up = false});
    return;
  }
  last_event = end;

  /* then insert the new item */
  Data_with_time<T> dwt = {.d = item, .begin = begin, .end = end, .mixed_up = false};
  if (!data[i].empty() && data[i].back().mixed_up && data[i].back().end > begin){
    if (data[i].back().begin - begin < (((uint64_t) 1) << i)){
      data[i].back() = dwt;
    } else {
      data[i].back().end = begin;
      data[i].push_back(dwt);
    }
  } else{
    data[i].push_back(dwt);
  }

  /* add mixed up data above, if needed */
  for (i++; i < imax; i++){
    uint64_t step = (((uint64_t) 1) << i);
    // assumes it does not overflow
    end = begin + step;
    if (data[i].empty() ){
      data[i].push_back({.d = item, .begin = begin, .end = end, .mixed_up = true});
    }
    else if (! data[i].back().mixed_up){
      // or data[i].back().end + step < begin ? to force a bit of space...
      if (data[i].back().end < begin){
        data[i].push_back({.d = item, .begin = begin, .end = end, .mixed_up = true});
      }
    }
    // data is mixed up, do we need to merge?
    else if (data[i].back().end < begin && begin - data[i].back().end > step){
      // no, append
      data[i].push_back({.d = item, .begin = begin, .end = end, .mixed_up = true});
    }
    else if (data[i].back().end > end){
      // should not happen
      std::cout << "[-] Warning: corrupted timestamp in TLData" << std::endl;
      std::cout << "    at level " << i << ": ";
      print_dWT(data[i].back());
    }
    else {
      // yes, merge
      data[i].back().end = end;
    }
  }
}

template <typename T>
void TLData<T>::overview() const {
  std::cout << "------- TLData overview ---------" << std::endl;
  uint s = 0;
  for (uint i = imin; i < imax; i++){
    std::cout << "level " << i << ": " << data[i].size() << " items";
    s += data[i].size();
    uint t = 0;
    for (uint j = 0; j < data[i].size(); j++) t += data[i][j].mixed_up;
    std::cout << " (" << t << " mixed_up, " << ( data[i].size() - t) << " real)";
    for(uint j = 0; j < 50 && j < data[i].size(); j++){
      std::cout << " " << j << ":";
      print_dWT(data[i][j], false);
    }
    std::cout << std::endl;
  }
  std::cout << "Total = " << s << std::endl;
  std::cout << "--------------------------------" << std::endl;
}


template <typename T>
TLData_iterator<T> TLData<T>::between(uint64_t begin, uint64_t end,
        double zoom_factor) const{
  return TLData_iterator<T>(this, begin, end, 1. / zoom_factor);
}


template <typename T>
TLData_iterator<T>::TLData_iterator(const TLData<T>* ref, uint64_t t0,
          uint64_t t1, uint64_t timeunits_per_pixel){
  this->ref = ref;
  this->t0 = t0;
  this->t1 = t1;
  this->timeunits_per_pixel = timeunits_per_pixel;
  current_i = std::min(std::max((uint) std::log2(timeunits_per_pixel), ref->imin),
                      ref->imax - 1);
  initial_line = true;
  binary_search_j();
  current_j = jmin;
  update_current();
  is_done = (current_i >= ref->imax);
}

template <typename T>
void TLData_iterator<T>::update_current(){
  //for (uint i  = current_i; i < ref->imax; i++)
  //  std::cout << "level " <<  i << ": " << ref->data[i].size() << std::endl;

  while (1){
    for (; current_j < jmax && !initial_line
            && ref->data[current_i][current_j].mixed_up; current_j++);
    if (current_j < jmax) return;
    initial_line = false;
    current_i++;
    if (current_i >= ref->imax) {
      is_done = true;
      return;
    }
    binary_search_j();
    current_j = jmin;
  }
}

template <typename T>
TLData_iterator<T>& TLData_iterator<T>::operator++(){
  current_j++;
  update_current();
  //std::cout << "iter level " << current_i << ": " << jmin << " " << jmax << std::endl;
  return *this;
}

template <typename T>
const Data_with_time<T>& TLData_iterator<T>::operator*() const{
  //std::cout << "accessing i = " << current_i << " j = " << current_j << std::endl;
  //std::cout << ref->data.size() << " " << ref->data[current_i].size();
  //std::cout << std::endl;
  return ref->data[current_i][current_j];
}

template <typename T>
void TLData_iterator<T>::binary_search_j(){
  //std::cout << "bin search i = " << current_i << "; " << ref->data[current_i].size() << " item on the line" << std::endl;
  //std::cout << "query is " << t0 << " " << t1 << std::endl;
  uint left = 0;
  uint J = ref->data[current_i].size();
  uint right = J;
  if (J == 0 || ref->data[current_i][0].begin >= t1){
    jmin = 0; jmax = 0;
    return;
  }
  while (right - left > 1){
    uint j = (right + left) / 2;
    if (ref->data[current_i][j].begin > t0) right = j;
    else left = j;
  }
  jmin = left;
  // here, ref->data[current_i][jmin].begin < t1 always true
  if (ref->data[current_i][jmin].end <= t0) jmin++;
  if (jmin == J || ref->data[current_i][jmin].begin > t1){
    jmax = jmin;
    return;
  }
  // now ref->data[current_i][jmin] intersects [t0, t1] for sure
  // so at that point, we are sure the interval is not empty
  if (jmin == J - 1){
    jmax = J;
    return;
  }
  if (ref->data[current_i][jmin + 1].begin >= t1){
    jmax = jmin + 1;
    return;
  }
  left = jmin + 1;
  right = ref->data[current_i].size();
  while (right - left > 1){
    uint j = (right + left) / 2;
    if (ref->data[current_i][j].begin >= t1) right = j;
    else left = j;
  }
  // end of loop: ref->data[current_i][left].begin < t1
  jmax = left + 1;
}


/*
  this is not optimal! everything is stored in a deque, to be able to iterate
  over it later. With a custom iterator we could have only the current element
  in memory when iterating
*/
template <typename T>
std::deque<Data_with_time<decompressed_data_t<T> > > decompress(
                            TLData_iterator<compressed_data_t<T> > iter){
  std::deque<Data_with_time<decompressed_data_t<T> > > r;
  for(; !iter.done(); ++iter){
    Data_with_time<compressed_data_t<T> > dwt = *iter;
    Data_with_time<decompressed_data_t<T> > to_be_pushed;

    /* first point: do we need to decompress?
       (no need to decompress something that is going to appear mixed up!)*/
    uint rounds = (uint) dwt.d.rle_coeff * (uint) dwt.d.n_items;
    if (dwt.end <= dwt.begin || rounds == 0) continue;
    uint64_t tw = dwt.end - dwt.begin;
    if (dwt.mixed_up || tw / rounds < iter.get_tu_p_pxl()){
      //easy case: data is mixed up, no need to decompress! :)
      to_be_pushed.begin = dwt.begin;
      to_be_pushed.end = dwt.end;
      to_be_pushed.mixed_up = true;
      to_be_pushed.d.value = dwt.d.value;
      to_be_pushed.d.n_items_in_avg = 1; // does not matter
      r.push_back(to_be_pushed);
      continue;
    }

    // ok, go for the hard work, we have to decompress
    uint64_t t0 = iter.get_t0();
    uint64_t t1 = iter.get_t1();
    /*
      recall: dwt represents 'rounds' consecutive events, starting from begin
      to end, which have the same duration
      we are interested in events that are in range [t0, t1]

      the following remark can be used to improve perfs:
      the equation dwt.begin + i * tw / rounds >= t0 is equivalent to
      i >= (t0 - dwt.begin) * rounds / tw

      Similarly, we also want: i < (t1 - dwt.begin) * rounds / tw
    */
    uint i0 = (t0 > dwt.begin ? (t0 - dwt.begin) * rounds / tw : 0);
    uint i1 = std::min(rounds,
                  (uint) (t1 > dwt.begin ? (t1 - dwt.begin) * rounds / tw + 1: 0));
    to_be_pushed.d.value = dwt.d.value;
    to_be_pushed.d.n_items_in_avg = dwt.d.n_items;
    to_be_pushed.mixed_up = dwt.mixed_up;
    for (uint i = i0; i < i1; i++) {
      to_be_pushed.begin = dwt.begin + (uint64_t) i * tw / rounds;
      to_be_pushed.end = dwt.begin + (uint64_t) (i + 1) * tw / rounds;
      r.push_back(to_be_pushed);
    }
  }
  return r;
}

template class TLData_iterator<const char*>;
template class TLData<const char*>;
template class TLData_iterator<uint32_t>;
template class TLData<uint32_t>;
template class TLData_iterator< compressed_data_t<uint32_t> >;
template class TLData< compressed_data_t<uint32_t> >;
template std::deque<Data_with_time<decompressed_data_t<uint32_t> > >
    decompress(TLData_iterator<compressed_data_t<uint32_t> >);

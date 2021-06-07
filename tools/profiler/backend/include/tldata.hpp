/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TLDATA_HPP
#define TLDATA_HPP

#include <stdlib.h>
#include <vector>
#include <cstdint>
#include <deque>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define MEMBER_SIZE(type, member) sizeof(((type *)0)->member)

/**
  @brief to save memory, this structure stores several T values in a compressed
         manner.
  */
template <typename T> class compressed_data_t {
public:
  T value;
  uint8_t rle_coeff;  /**< data represents actually rle_coeff consecutive equivalent events */
  uint16_t n_items;    /**< value is actually the average of n_items events */
};


/**
  @brief stores ready-to-use decompressed data + compression method(s) that were
         used for the compression (thus gives a hint of the loss of precision
         due to compression)
*/
template <typename T> class decompressed_data_t {
public:
  T value;
  uint16_t n_items_in_avg; /**< 'value' is the 'mean of n_items_in_avg' elements */
};

template <typename T>
std::ostream &operator<<(std::ostream &os, compressed_data_t<T> const &m) {
    return os << "(" << (int) m.rle_coeff << " x " << m.value << ")";
}

/**
@brief a wrapper class used by the TLData class to store data of any type
*/
template <typename T> class Data_with_time{
public:
  T d;            /**< data stored by the structure */
  uint64_t begin; /**< data is displayed from this timestamp in timeline */
  uint64_t end;   /**< data is displayed to this timestamp in timeline */
  bool mixed_up;   /**< if true, means that this data is a mix of different events*/
};

template <typename T> class TLData;

/**
 * @brief iterator over a TLData to get all the visible elements in a given
 *        time window at a given scale.
 *
 * Items in an inverval [t0, t1] are not necessarily yielded in order
 */
template <typename T> class TLData_iterator{
public:
  TLData_iterator(const TLData<T>* ref, uint64_t t0, uint64_t t1,
      uint64_t timeunits_per_pixel);
  bool done() const {return is_done;};
  TLData_iterator<T> & operator++();
  const Data_with_time<T>& operator*() const;
  uint64_t get_t0() const {return t0;}
  uint64_t get_t1() const {return t1;}
  uint64_t get_tu_p_pxl() const {return timeunits_per_pixel;}

private:
  void binary_search_j();
  void update_current();
  const TLData<T>* ref;
  bool is_done;
  uint current_i;
  uint current_j;
  uint jmin, jmax;
  uint64_t t0, t1;
  uint64_t timeunits_per_pixel;
  bool initial_line;
};

/**
 * @brief data structure to efficiently get the elements to display in a given
 *        time window.
 *
 * It relies on these two facts:
 *    - when zoom factor is high (i.e. window very narrow), only a few items are
 *      in the window, even if the timeline is very long
 *    - when zoom factor is low, a lot of items are mixed up, because resolution
 *      is limited. So the number of items to display is not that high in that
 *      case either.
 *
 * Moreover some assumptions are made to be more efficient:
 *    1. items do not overlap, i.e. end(item i) < begin(item i + 1). You can use
 *      several TLData instances to respect that (one instance per core).
 *    2. items are added in chronological order
 *
 * Complexity:
 *    - This implementation provide a O(1) insertion time (on average).
 *    - Iterate over all visible items in a given time window costs O(M + log(N))
 * where M is the number of visible items in the window and N is the total
 * number of items stored in the data structure.
 *
 * It is almost optimal, because the complexity cannot be lower than O(M)
 * and the term log(N) (multiplied by a hiden constant) is not high compared to
 * M in practice.
 *
 * Internal details:
 * The underlying data structure is a 2D array. We group into a vector
 * events with a similar duration. Events whose duration is in range
 * [2^i, 2^(i+1) ) are stored in the vector data[i]. Each data[i] is sorted by
 * chronological order (see assumption 2). Moreover we limit i to be lower than
 * the class member imax (with 64 bit integers, imax is at most 63). So we have
 * imax vectors.
 *
 * When a time window and a screen resolution are given, we first compute the
 * lowest i0 such that an event of duration 2^i0 is visible. Then for each
 * i0 <= i < imax, we binary search the events that intersect the time window.
 * You can easily verify the complexity mentionned above, but you can also
 * understand the idea as follows: at large scale (i.e. i0 large), we avoid
 * the iteration over numerous short events, so it is fast. At small scale
 * (i.e. i0 low), iterating is fast as well because the time window is narrow.
 *
 * The last difficulty comes from the fact that a cluster of non-visible events
 * can be visible. To avoid a costly iteration over short events at large scale,
 * we directly add clusters of shorter events into each data[i]. This is the
 * meaning of the mixed_up field in the Data_with_time_structure. If data[i][j] has
 * mixed_up set to true, it means that data[i][j] does not correspond to a event,
 * it is just a "marker" to say: "between data[i][j].begin and data[i][j].end
 * there is a bunch of short events, but at that scale, you cannot see precisely
 * what is inside". If you do the maths, you'll see that the memory overhead
 * with these markers is at most 100% (i.e. mem usage is multiplied by 2).
 *
 * Possible improvements:
 *    - what if there are so many events that they cannot fit in memory? The
 *      structure could use hard drive storage when the number of items inside
 *      becomes too large
 *
 */
template <typename T> class TLData {
public:
  /**
   * @param imin: merge all events whose duration is in range [1, 2^imin]
   * @param resolution: width of the timeline, in pixels (can be the resolution
            of the screen).
   */
  TLData(uint imin = 10, uint resolution = 1920);

  /**
   * @param begin: beginning of the event (timestamp)
     @param end: end of the event
     @param item: data that represents the event to store
     @param level_coeff (optional): to force an item to be at a higher/lower
            place in the 2D array. For example, if level_coeff=4, item would
            be visible iff it is greater than 4 pixels (and not 1). Values
            between 0 and 1 can also be used to force compressed items as mixed
            up items.
    */
  void add_item(uint64_t begin, uint64_t end, T item);

  /**
      @param begin
      @param end
      @param zoom_factor number of pixels per time unit. So it defines the
             minimal duration for an event to be visible
      @return an iterator to get all visible items that are between begin and
              end
    */
  TLData_iterator<T> between(uint64_t begin, uint64_t end, double zoom_factor) const;

  /**
      print into standard output a summary of data stored. Useful to debug
    */
  void overview() const;

  /**
    @return the timestamp of the end of the last added event
    */
  uint64_t get_end_of_last_event() const {return last_event; }

private:
  friend class TLData_iterator<T>;
  /* data[i] contains items that are visible iff 1 pixel < 2**i time units */
  std::vector< std::vector< Data_with_time<T> > > data;
  uint imin;    /**< items of duration < 2^imin are merged */
  const uint imax = 55; /**< max duration for an event is 2^imax */
  uint resolution;      /**< resolution of the screen */
  uint64_t last_event;   /**< last added event */
};

/**
  function to print a Data_with_time structure
  @param dwt structure to print
  @param newline if true, print a newline after the data
 */
template <typename T> void print_dWT(const Data_with_time<T>& dwt, bool newline=true);
template <typename T> std::deque<Data_with_time<decompressed_data_t<T> > >
                        decompress(TLData_iterator<compressed_data_t<T> > iter);
#endif  //TLDATA_HPP

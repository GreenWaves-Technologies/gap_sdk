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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#pragma once


/*
 * Class used to model a DC fifo
 *
 * This class can be used to model a DC fifo, to exchange data between 2 clock domains.
 * In particular, it models the timing behavior of the fifo due to the stages required
 * for the synchronization of the 2 clock domains.
 * 
 * Timing behavior
 * The RTL of the fifo is full as soon as there is not more than 3 free elements, due
 * to the synhcronization stages. However the full signal needs to bre resynchronized
 * by the 2 cycles, so the sender see the fifo full actually 2 cycles after.
 * If we take the extreme case where the fifo size if 4 (which is the minimal),
 * when a data is pushed, if the data is not read during the same cycle, the fifo is
 * considered full, even though the sender can still push 2 elemets, due to the resynchronization.
 * Once the fifo is full, the sender can not push anymore until it is unstalled. The
 * receiver takes 5 cycles (of the receiver domain) for the first pop and can then do a pop
 * every 4 cycles. Once all the data has been popped, the fifo is considered unstalled at the first
 * rising edge of the sender domain, and the sender can push a new data one cycle after.
 * This means the fifo has a good behavior when receiver frequency is high enough to sustain the
 * pop during the same cycle as the push (which is a ratio of x6), but bandwidth quickly decreases
 * as soon as there is any stall, since the fifo needs to be empty before the sender can
 * continue.
 */
class Dc_fifo
{
public:
    /**
     * @brief Constructor
     *
     * @param size Size of the FIFO, should be at least 4.
     * @param top Pointer to the component containing the FIFO.
     * @param trace_path Prefix for traces declared by the FIFO.
     * @param snd_clock Clock domain of the sender side.
     * @param rcv_clock Clock domain of the receiver side.
     * @param rcv_event Receiver event which must be enqueued when a new data is available.
     * @param unstall_event Sender event which must be enqueued when the FIFO becomes available
     *                      after it has been stalled because it was full.
     */
    Dc_fifo(int size, vp::component *top, std::string trace_path, vp::clock_engine *snd_clock,
        vp::clock_engine *rcv_clock, vp::clock_event *rcv_event, vp::clock_event *unstall_event);

    /**
     * @brief Reset the FIFO.
     *
     * @param active True if the reset is active.
     */
    void reset(bool active);

    /**
     * @brief Push a data to the FIFO.
     *
     * The data is enqueued to the FIFO and made available to the receiver side, afer some time
     * which depends on the ratio between the frequency of the receiver domain and the one
     * of the sender domain.
     * The FIFO must not be full when the data is pushed.
     * Once the data is available on receiver side, the receiver event (rcv_event) will be
     * executer.
     *
     * @param data Data to be pushed to the FIFO.
     * @param size Size of the data to be pushed.
     */
    void push(uint8_t *data, int size);

    /**
     * @brief Pop a data from the FIFO.
     *
     * The data is removed from the FIFO and returned.
     * At least one data must be available when this method is called.
     *
     * @param size Size of the data popped is returned here.
     *
     * @return Return the data popped from the FIFO.
     */
    uint8_t *pop(int *size);

    /**
     * @brief Tell if the FIFO is full.
     *
     * This method should be called by the sender to know if it can push a data.
     *
     * @return True if the FIFO is full otherwise false.
     */
    bool is_full();

    /**
     * @brief Tell if the FIFO is empty.
     *
     * This method can be called by the receiver to know if it can pop a data.
     *
     * @return True if the FIFO is empty otherwise false.
     */
    bool is_empty();

    /**
     * @brief Set the sender clock domain divider
     *
     * This can be used to set a divider which is applied to the number of cycles used
     * to enqueue events to model the timing behavior of the FIFO.
     * This is useful when the clock domain frequency does not reflect the real frequency but a
     * multiple divided by the specified value.
     *
     * @param div divider value of the sender clock domain.
     */
    void set_snd_div(int div) { this->snd_div = div; }

private:
    // Sender FSM
    static void handle_snd_fsm(void *__this, vp::clock_event *event);
    // Receiver FSM
    static void handle_rcv_fsm(void *__this, vp::clock_event *event);
    // Handler called when the FIFO is unstalled, to wakeup the sender
    static void handle_snd_stall(void *__this, vp::clock_event *event);
    // Called everytime something is changed to check what should be done next.
    void check_state();
    // Return the real number of cycles, after the divider is applied
    int64_t get_snd_cycle_count(int64_t cycles);

    vp::component *top;                    // Pointer to top component containing the FIFO
    std::queue<uint8_t *> rcv_data_queue;  // Queue for storing data
    std::queue<int>       rcv_size_queue;  // Queue for storing data size
    int size;                              // Number of elements of the FIFO
    int nb_elems;                          // Current number of data pushed to the FIFO
    int64_t nb_elems_check;                // Absolute cycle count where the number of elements
                                           // should be checked to see of the FIFO is fulled
    vp::clock_event *rcv_fsm_event;        // Event for receiver FSM
    vp::clock_event *snd_fsm_event;        // Event for sender FSM
    vp::clock_engine *snd_clock;           // Sender clock domain
    vp::clock_event *snd_stall_event;      // Event used to unstall the sender
    int snd_div;                           // Sender clock domain divider
    vp::clock_engine *rcv_clock;           // Receiver clock domain
    vp::clock_event *rcv_event;            // Receiver event called when a data is ready to be popped
    vp::clock_event *unstall_event;        // Sender event called when the FIFO is unstalled and the sender
                                           // can push again data.
    vp::trace trace;                       // DC FIFO trace.
    bool stalled;                          // True when the FIFO is stalled and data can not be pushed anymore
    bool stalled_sync;                     // Intermediate value for synchronizing the stall signal.
};

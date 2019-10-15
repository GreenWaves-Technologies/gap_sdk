/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __RT_RT_ERROR_H__
#define __RT_RT_ERROR_H__


/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup Error Error handling
 *
 * The following API can be used to analyze errors reported by the whole API. There are 2 mechanisms involved:
 *   - Asynchronous calls errors. These errors can happen when an event is specified when calling the API and the error occurs after the call has returned with a success. It is then possible to register a callback so that these delayed errors are notified.
 *   - Synchronous calls errors. These errors can happen when no event is specified when calling the API. In this case, the API call is returning RT_STATUS_ERR and the error is available through a function call. 
 *
 * In both cases, it is possible to get an error number which is made of an error group telling which part of the API reported an error, and an error code which is the actual error.
 */

/**        
 * @addtogroup Error
 * @{        
 */

/**@{*/

/** \enum rt_status_e
 * \brief Error code.
 *
 * This is used to report an error code.
 */
typedef enum {
  RT_STATUS_OK      = 0,     /*!< No error. */
  RT_STATUS_ERR     = -1,    /*!< Error. */
} rt_status_e;


/** \enum rt_error_group_e
 * \brief Group reporting an error.
 *
 * This is used to know which API group has reported the error. This error code is then defined by a group specific enumeration.
 */
typedef enum {
  RT_ERROR_FS       = 1,     /*!< File-system error. */
} rt_error_group_e;




/** \brief Declare a callback for handling asynchronous errors.
 *
 * Errors which happen due an asynchronous API call can be notified by registering a callback on an event scheduler.
 * Asynchronous calls are always associated an event. All errors happenning for events associated to the specified scheduler will be notified through the specified callback.
 * 
 * \param sched      The scheduler where the callback must be attached. All errors associated to events from this scheduler will be notified through this callback.
 * \param callback   The callback function to be called when there is an error.
 * \param arg        The callback argument.
 */
void rt_error_conf(rt_event_sched_t *sched, rt_error_callback_t callback, void *arg);



/** \brief Return the error registered by the last synchronous API call.
 *
 * \return The error code.
 */
static inline int rt_error_current();



/** \brief Return the group part of an error.
 * 
 * This is useful for quickly identifying in which part of the API the error occured. 
 *
 * \param  error The error returned by the API call.
 * \return The error group.
 */
static inline rt_error_group_e rt_error_group(int error);



/** \brief Return the error code part of an error.
 *
 * Once the error group is known, this is useful to know the exact error which occured. Note that this part is specific to the error group and is documented in the section for the considered API call.
 * 
 * \param  error The error returned by the API call.
 * \return The error code.
 */
static inline int rt_error_code(int error);



/** \brief Return a string explaining the error.
 * 
 * This is useful for quickly debugging an error. This is reporting useful information only if the runtime is compiled with message support. 
 *
 * \param  error The error returned by the API call.
 * \return The error string.
 */
char *rt_error_str(int error);


//!@}

/**        
 * @}     
 */


/// @cond IMPLEM



#define RT_ERROR_GROUP_BITS 8

void __rt_error_report(rt_event_t *event, int error, void *object);

static inline void __rt_error_register(int error)
{
	__rt_thread_current->error = error;
}

static inline rt_error_group_e rt_error_group(int error)
{
	return (rt_error_group_e) (error & ((1<<RT_ERROR_GROUP_BITS)-1));
}

static inline int rt_error_code(int error)
{
	return error >> RT_ERROR_GROUP_BITS;
}

static inline int __rt_error(int group, int error)
{
	return group | (error << RT_ERROR_GROUP_BITS);
}

static inline int rt_error_current()
{
	return __rt_thread_current->error;
}


/// @endcond

#endif

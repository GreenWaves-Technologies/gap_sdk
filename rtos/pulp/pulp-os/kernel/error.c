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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

static void __rt_error_cb_stub(void *arg)
{
	// This stub is called once the error event is scheduled
	// Just call the error callback and after that enqueue the user event
	// 
	rt_event_t *event = (rt_event_t *)arg;
	rt_event_sched_t *sched = rt_event_internal_sched();
	sched->error_cb(sched->error_arg, event, event->implem.data[0], (void *)event->implem.data[1]);
	__rt_event_unblock(event);
}

void rt_error_conf(rt_event_sched_t *sched, rt_error_callback_t cb, void *arg)
{
	// The error callback is stored in the scheduler as it is only used
	// when an event is specified
	sched = rt_event_internal_sched();
	sched->error_cb = cb;
	sched->error_arg = arg;
}

void __rt_error_report(rt_event_t *event, int error, void *object)
{
	rt_warning("Error reported (error: 0x%x, event: %p, object: %p): %s\n", error, event, object, rt_error_str(error));

	if (event == NULL)
	{
		// The case where no event is specified is the synchronous execution
		// The API call will report RT_STATUS_ERR and the caller can get the real
		// error here
		__rt_error_register(error);
	}
	else
	{
		// Otherwise an event calling the error callback must be posted
		rt_event_sched_t *sched = rt_event_internal_sched();

		if (sched->error_cb)
		{
			// As the specified event may still be used (e.g. for a blocking wait),
			// we use another event to enqueue the error callback execution
			// but we store the error information inside the user event as the error event
			// won't be valid once inside the stub.
			rt_event_t *error_event = rt_event_get(sched, __rt_error_cb_stub, (void *)event);
			event->implem.data[0] = error;
			event->implem.data[1] = (unsigned int)object;
			__rt_event_enqueue(error_event);
		}
	}
}

char *rt_error_str(int error)
{
	// This function just needs to dispatch the error to the right group function
	char *error_str = NULL;
#ifdef __RT_USE_IO
	switch (rt_error_group(error))
	{
		case RT_ERROR_FS: 
			return error_str = __rt_fs_error_str(error);
	}
#endif
	if (error_str == NULL) error_str = "Unknown error";
	return error_str;
}

/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "trace_dumper.hpp"
#include <string>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
	std::string path = argv[1];
	trace_dumper_server *server = new trace_dumper_server(path);

	if (server->open())
	{
		printf("Failed to open file %s wit error: %s\n", path.c_str(), strerror(errno));
		return -1;
	}

	while(1)
	{
		trace_packet packet;
		if (server->get_packet(&packet))
			break;
		packet.dump();
	}

	return 0;
}
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
# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from abc import ABC, abstractmethod

from nntool.importer.common.handler_options import HandlerOptions


class ImporterBase(ABC):

    @abstractmethod
    def create_graph(self, filename, opts):
        pass

    # @abstractmethod
    # def create_graph_from_model(self, model, opts):
    #     pass

    def get_opts(self, opts):
        if opts is None:
            opts = HandlerOptions.get_default_handler_options()
        else:
            default_opts = HandlerOptions.get_default_handler_options()
            default_opts.update(opts)
            opts = default_opts
        return opts

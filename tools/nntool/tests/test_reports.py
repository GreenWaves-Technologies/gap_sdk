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


from importer.importer import create_graph
from quantization.symmetric.symmetric_quantizer import SymmetricQuantizer
from reports.activation_reporter import ActivationReporter
from reports.error_reporter import ErrorReporter
from reports.filter_reporter import (FilterDetailedStatsReporter,
                                     FilterStatsReporter)
from reports.graph_reporter import GraphReporter
from reports.quantization_reporter import QuantizationReporter
from reports.temps_reporter import TempsReporter
from stats.activation_stats_collector import ActivationStatsCollector
from stats.error_stats_collector import ErrorStatsCollector
from stats.filter_stats_collector import (FilterDetailedStatsCollector,
                                          FilterStatsCollector)
from stats.temps_stats_collector import TempsStatsCollector
from utils.data_importer import import_data
from utils.new_param_state import load_state
from utils.tabular import TextTableRenderer

def test_activation_report(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    stats_collector = ActivationStatsCollector()
    stats_collector.collect_stats(G, [input_tensor])
    report = ActivationReporter().report(G, stats_collector.reduce_stats())
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_filter_report(mnist_graph):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    stats_collector = FilterStatsCollector()
    stats = stats_collector.collect_stats(G)
    report = FilterStatsReporter().report(G, stats)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_filter_detailed_report(mnist_graph):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    stats_collector = FilterDetailedStatsCollector()
    stats = stats_collector.collect_stats(G)
    report = FilterDetailedStatsReporter().report(G, stats)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_error_report(mnist_unfused_8bit_state, mnist_images):
    G = load_state(mnist_unfused_8bit_state)
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    stats_collector = ErrorStatsCollector()
    stats_collector.collect_stats(G, [input_tensor])
    stats_collector.collect_stats(G, [input_tensor])
    report = ErrorReporter().report(G, stats_collector.reduce_stats())
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_temps_report(mnist_graph):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    stats_collector = TempsStatsCollector()
    stats = stats_collector.collect_stats(G)
    report = TempsReporter().report(G, stats)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_temps_report_quantized(mnist_unfused_8bit_state):
    G = load_state(mnist_unfused_8bit_state)
    G.add_dimensions()
    stats_collector = TempsStatsCollector(qrecs=G.quantization)
    stats = stats_collector.collect_stats(G)
    report = TempsReporter().report(G, stats)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_graph_report(mnist_graph):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    report = GraphReporter().report(G, None)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_simple_quantization(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    stats_collector = ActivationStatsCollector()
    stats_collector.collect_stats(G, [input_tensor])
    astats = stats_collector.reduce_stats()
    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)
    quantizer = SymmetricQuantizer(astats, fstats, force_width=8)
    qrecs = quantizer.quantize(G)
    assert len(qrecs) == 11 # One more for saved quantizer
    report = QuantizationReporter().report(G, qrecs)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

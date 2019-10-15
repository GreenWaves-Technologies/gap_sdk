# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.


from importer.importer import create_graph
from quantization.simple_quantizer import SimpleQuantizer
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

def test_filter_report(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    stats_collector = FilterStatsCollector()
    stats = stats_collector.collect_stats(G)
    report = FilterStatsReporter().report(G, stats)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_filter_detailed_report(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    stats_collector = FilterDetailedStatsCollector()
    stats = stats_collector.collect_stats(G)
    report = FilterDetailedStatsReporter().report(G, stats)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_error_report(value_cache, mnist_unfused_8bit_state, mnist_images):
    G = load_state(mnist_unfused_8bit_state, value_cache=value_cache)
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    stats_collector = ErrorStatsCollector()
    stats_collector.collect_stats(G, [input_tensor])
    stats_collector.collect_stats(G, [input_tensor])
    report = ErrorReporter().report(G, stats_collector.reduce_stats())
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_temps_report(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    stats_collector = TempsStatsCollector()
    stats = stats_collector.collect_stats(G)
    report = TempsReporter().report(G, stats)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

def test_temps_report_quantized(value_cache, mnist_unfused_8bit_state):
    G = load_state(mnist_unfused_8bit_state, value_cache=value_cache)
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
    quantizer = SimpleQuantizer(astats, fstats, force_width=8)
    qrecs = quantizer.quantize(G)
    assert len(qrecs) == 10
    report = QuantizationReporter().report(G, qrecs)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))

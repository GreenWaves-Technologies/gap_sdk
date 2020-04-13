import csv
import io
import os

class Jenkins_plot(object):

  def __init__(self, filename):
    self.results = {}
    self.filename = filename

  def append(self, name, value):
    self.results[name] = value

  def gen(self):
    with open(self.filename, "w") as csv_file:
      csv_file.write('"%s"\n' % ', '.join(self.results.keys()))
      csv_file.write('%s\n' % ', '.join(self.results.values()))



class Bench(object):

  def __init__(self):
    self.results = []

  def append(self, line):
    self.results.append(line)

  def gen(self):
    pmsis_os = os.environ['PMSIS_OS']
    os_index = ['pulpos', 'zephyr', 'freertos', 'mbed', 'ali'].index(pmsis_os)

    results = []
    try:
      with open(os.environ['BENCH_CSV_FILE']) as csvfile:
        csv_reader = csv.reader(csvfile)
        for line in csv_reader:
          results.append(line)
    except:
      pass

    bench = ''
    capture = False

    # for line in output.splitlines():
    #   if line == 'Benchmark stop':
    #     capture = False
    #   if capture:
    #     bench += line + '\n'
    #   if line == 'Benchmark start':
    #     capture = True

    test_results = []
    for line in csv.reader(io.StringIO('\n'.join(self.results))):
      test_results.append(line)

    line_index = 0

    found = False
    for line in results:
      if line[0] == test_results[0][0]:
        found = True
        for test_line in test_results:

          if os_index >= len(line) - 2:
            line = line + [ '' ] * (os_index + 3 - len(line))

          line[os_index+2] = test_line[2]
          results[line_index] = line

          line_index += 1

          if line_index < len(results):
            line = results[line_index]
        break
      line_index += 1

    if not found:
      for test_line in test_results:

        line = test_line[0:2]

        if os_index >= len(line) - 2:
          line = line + [ '' ] * (os_index + 3 - len(line))

        line[os_index+2] = test_line[2]
        results.append(line)

    with open(os.environ['BENCH_CSV_FILE'], 'w') as csvfile:
      csv_writer = csv.writer(csvfile)

      for line in results:
        csv_writer.writerow(line)




def process_bench(output):

  pmsis_os = os.environ['PMSIS_OS']
  os_index = ['pulpos', 'zephyr', 'freertos', 'mbed', 'ali'].index(pmsis_os)

  results = []
  try:
    with open(os.environ['BENCH_CSV_FILE']) as csvfile:
      csv_reader = csv.reader(csvfile)
      for line in csv_reader:
        results.append(line)
  except:
    pass

  bench = ''
  capture = False

  for line in output.splitlines():
    if line == 'Benchmark stop':
      capture = False
    if capture:
      bench += line + '\n'
    if line == 'Benchmark start':
      capture = True

  test_results = []
  for line in csv.reader(io.StringIO(bench)):
    test_results.append(line)

  line_index = 0

  found = False
  for line in results:
    if line[0] == test_results[0][0]:
      found = True
      for test_line in test_results:

        if os_index >= len(line) - 2:
          line = line + [ '' ] * (os_index + 3 - len(line))

        line[os_index+2] = test_line[2]
        results[line_index] = line

        line_index += 1

        if line_index < len(results):
          line = results[line_index]
      break
    line_index += 1

  if not found:
    for test_line in test_results:

      line = test_line[0:2]

      if os_index >= len(line) - 2:
        line = line + [ '' ] * (os_index + 3 - len(line))

      line[os_index+2] = test_line[2]
      results.append(line)

  with open(os.environ['BENCH_CSV_FILE'], 'w') as csvfile:
    csv_writer = csv.writer(csvfile)

    for line in results:
      csv_writer.writerow(line)

Most usefull commands
---------------------

For activating instruction traces with debug symbols: ::

  pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run --trace=insn --debug-syms

For activating all traces: ::

  pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run --trace=.*

For activating VCD traces (traces are dumped to the file *all.vcd): ::

  pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run --event=.*
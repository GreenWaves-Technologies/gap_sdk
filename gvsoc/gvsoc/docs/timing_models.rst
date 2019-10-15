Timing models
-------------

Timing models are always active, there is no specific option to set to activate them. They are mainly timing the core model so that the main stalls are modeled. This includes branch penalty, load-use penalty an so on. The rest of the architecture is slightly timed. Remote accesses are assigned a fixed cost and are impacted by bandwidth limitation, although this still not reflect exactly the HW (the bus width may be different). L1 contentions are modeled with no priority. DMA is modeled with bursts, which gets assigned a cost. All UDMA interfaces are finely modeled.

# PULP Microcontroller Software Interface Standard (PMSIS)

This module implements [PMSIS_API](https://greenwaves-technologies.com/manuals/BUILD/PMSIS_API/html/md__home_yao_Projects_SDK_gap_sdk_github_rtos_pmsis_pmsis_api_docs_mainpage.html). PMSIS consists of software layer used by GreenWaves-Technologies to support multiple RTOS and boards.

This layer abstracts the hardware and for some cases the OS.
Using this layer for an application allows to create application independent of RTOS used, and in most cases independent of board and chip version.

```
===============================
|          USER APP           |
===============================
|          | RTOS | GAP_LIBS  |
|          --------------------
|          PMSIS              |  <--- this part
===============================
|          HARDWARE           |
===============================
```

This modules consists of submodules organized as follows :
* **PMSIS**
 * `pmsis_implem_gap8` : chip specific implementation of PMSIS.
   * cluster  - Cluster specific drivers
   * drivers  - Chip drivers(IO interfaces, timers,...)
   * include  - Headers and definitions
 * `pmsis_rtos` : memory management, event tasks, event kernel,...

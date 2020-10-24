# APB SOC controller

## Description

## Registers

| Register Name       | Offset | Size | Register Type | Host Access Type | Block Access Type | Default    | Reset | Description | Note |
| ---                 | ---    | ---  | ---           | ---              | ---               | ---        | ---   | --- | --- |
| INFO                | 0x000  | 32   | Status        | R                | W                 | 0x18       | 1     | Core information register. | |
| FC_BOOT             | 0x004  | 32   | Config        | R/W              | R/W               | 0x1A000000 | 1     | Boot address               | |
| FC_FETCH            | 0x008  | 32   | Config        | R/W              | R/W               | 0x0        | 1     | FC Fetch enable            | |
| CORESTATUS          | 0x0A0  | 32   | Status        | R/W              | R/W               | 0x0        | 1     | EOC and chip status register | |



### INFO

#### Fields

| Field Name | Offset | Size | Host Access Type | Block Access Type | Default | Description |
| ---        | ---    | ---  | ---              | ---               | ---     | ---         |
| NB_CL      | 0      | 16   | R                | W                 | 0x0008  | Number of clusters |
| NB_CORES   | 16     | 16   | R                | W                 | 0x0001  | Number of cores |


### FC_BOOT

#### Fields

| Field Name | Offset | Size | Host Access Type | Block Access Type | Default    | Description |
| ---        | ---    | ---  | ---              | ---               | ---        | ---         |
| ADDR       | 0      | 32   | R/W              | R                 | 0x1A000000 | FC Boot Address |


### FC_FETCH

#### Fields

| Field Name | Offset | Size | Host Access Type | Block Access Type | Default | Description |
| ---        | ---    | ---  | ---              | ---               | ---     | ---         |
| FC_FE      | 0      | 1    | R/W              | R                 | 0x0     | FC Fetch Enable |

### CORESTATUS

#### Fields

| Field Name | Offset | Size | Host Access Type | Block Access Type | Default | Description |
| ---        | ---    | ---  | ---              | ---               | ---     | ---         |
| STATUS     | 0      | 32   | R/W              | R/W               | 0x0     | Chip status register. The SW can store the exit value value so that the external loader can get it. |
| EOC        | 31     | 1    | R/W              | R/W               | 0x0     | End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished. |

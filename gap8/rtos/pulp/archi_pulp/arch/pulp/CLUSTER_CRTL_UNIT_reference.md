# Cluster control unit

## Description

## Registers

| Register Name       | Offset | Size | Register Type | Host Access Type | Block Access Type | Default    | Description | Note |
| ---                 | ---    | ---  | ---           | ---              | ---               | ---        | --- | --- |
| FETCH_EN            | 0x08   | 32   | Status        | R/W              | R                 | 0x0        | Cluster cores fetch enable configuration register. | |
| CLOCK_GATE          | 0x20   | 32   | Config        | R/W              | R                 | 0x0        | Cluster clock gate configuration register. | |
| BOOT_ADDR0          | 0x40   | 32   | Config        | R/W              | R                 | 0x0        | Cluster core 0 boot address configuration register. | |

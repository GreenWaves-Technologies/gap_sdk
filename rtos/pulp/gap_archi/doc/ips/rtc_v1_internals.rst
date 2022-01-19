.. 
   Input file: fe/ips/rtc_dolphin/docs/RTC_reference_internals.md

Register map for indirectly accessed registers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +----------------------------------------------------------+------+-----+------------------------------------------+
    |                           Name                           |Offset|Width|               Description                |
    +==========================================================+======+=====+==========================================+
    |:ref:`RTC_SR<rtc_internals__RTC_SR>`                      |     0|   32|RTC status register                       |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`RTC_CR<rtc_internals__RTC_CR>`                      |     1|   32|RTC control register                      |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`RTC_ICR<rtc_internals__RTC_ICR>`                    |     8|   32|RTC interrupt control register            |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`RTC_IMR<rtc_internals__RTC_IMR>`                    |     9|   32|RTC interrupt mask register               |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`RTC_IFR<rtc_internals__RTC_IFR>`                    |    10|   32|RTC interrupt flag register               |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`CALENDAR_CONTROL<rtc_internals__CALENDAR_CONTROL>`  |    16|   32|RTC calendar control register             |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`CALENDAR_TIME<rtc_internals__CALENDAR_TIME>`        |    18|   32|RTC calendar time register                |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`CALENDAR_DATE<rtc_internals__CALENDAR_DATE>`        |    19|   32|RTC calendar date register                |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`ALARM_CONTROL<rtc_internals__ALARM_CONTROL>`        |    24|   32|RTC alarm control register                |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`ALARM1_TIME<rtc_internals__ALARM1_TIME>`            |    26|   32|RTC alarm 1 time register                 |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`ALARM1_DATE<rtc_internals__ALARM1_DATE>`            |    27|   32|RTC alarm 1 date register                 |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`COUNTDOWN_CONTROL<rtc_internals__COUNTDOWN_CONTROL>`|    32|   32|RTC countdown control register            |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`COUNTDOWN1_INIT<rtc_internals__COUNTDOWN1_INIT>`    |    33|   32|RTC countdown 1 initialisation register   |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`COUNTDOWN1_TIMER<rtc_internals__COUNTDOWN1_TIMER>`  |    34|   32|RTC countdown 1 timer register            |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`CKIN_DIV1<rtc_internals__CKIN_DIV1>`                |    40|   32|RTC clock divider 1 register              |
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`CKREF_CONF<rtc_internals__CKREF_CONF>`              |    42|   32|RTC reference clock configuration register|
    +----------------------------------------------------------+------+-----+------------------------------------------+
    |:ref:`RTC_TEST_REG_A<rtc_internals__RTC_TEST_REG_A>`      |    48|   32|RTC test register                         |
    +----------------------------------------------------------+------+-----+------------------------------------------+

.. _rtc_internals__RTC_SR:

RTC_SR
""""""

RTC status register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+----------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                      Description                                       |
    +=====+===+====+=====+========================================================================================+
    |    0|R  |INT |0x0  |RTC interrupt status: b0: no interruption has been requested; b1: interruption requested|
    +-----+---+----+-----+----------------------------------------------------------------------------------------+

.. _rtc_internals__RTC_CR:

RTC_CR
""""""

RTC control register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+-------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                                                                Description                                                                |
    +=====+===+========+=====+===========================================================================================================================================+
    |    0|R/W|SB      |0x0  |RTC standby configuration: b0: RTC is in active mode; b1: RTC is in standby mode                                                           |
    +-----+---+--------+-----+-------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|CAL_EN  |0x0  |Calibration process activation (unused, must remain b0)                                                                                    |
    +-----+---+--------+-----+-------------------------------------------------------------------------------------------------------------------------------------------+
    |    8|R/W|SOFT_RST|0x0  |Soft reset command: b0: no reset; b1: reset the calendar, alarm, countdown and clock generation features and associated registers (CALENDAR|
    +-----+---+--------+-----+-------------------------------------------------------------------------------------------------------------------------------------------+

.. _rtc_internals__RTC_ICR:

RTC_ICR
"""""""

RTC interrupt control register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                                                                                                             Description                                                                                                                             |
    +=====+===+====+=====+=====================================================================================================================================================================================================================================================================+
    |1:0  |R/W|MODE|0x0  |RTC interrupt signal mode configuration: b00: RTC interrupt is a high level signal; b01: RTC interrupt is a low level signal; b10: RTC interrupt is a high level pulse of 1 REF SLOW clock period; b11: RTC interrupt is a low level pulse of 1 REF SLOW clock period|
    +-----+---+----+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _rtc_internals__RTC_IMR:

RTC_IMR
"""""""

RTC interrupt mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+---------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                                      Description                                      |
    +=====+===+==========+=====+=======================================================================================+
    |    0|R/W|ALARM_MASK|0x1  |Alarm 1 interrupt mask configuration: b0: interrupt enabled; b1: interrupt disabled    |
    +-----+---+----------+-----+---------------------------------------------------------------------------------------+
    |    4|R/W|TIMER_MASK|0x1  |Timer 1 interrupt mask configuration: b0: interrupt enabled; b1: interrupt disabled    |
    +-----+---+----------+-----+---------------------------------------------------------------------------------------+
    |   12|R/W|CAL_MASK  |0x1  |Calibration interrupt mask configuration: b0: interrupt enabled; b1: interrupt disabled|
    +-----+---+----------+-----+---------------------------------------------------------------------------------------+

.. _rtc_internals__RTC_IFR:

RTC_IFR
"""""""

RTC interrupt flag register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                                             Description                                             |
    +=====+===+==========+=====+=====================================================================================================+
    |    0|R  |ALARM_FLAG|0x0  |Alarm 1 interrupt status flag: b0: nothing; b1: calendar has reached the date and time set in alarm 1|
    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------+
    |    4|R  |TIMER_FLAG|0x0  |Timer 1 interrupt status flag: b0: nothing; b1: countdown timer 1 has reached value 0                |
    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------+
    |   12|R  |CAL_FLAG  |0x0  |Calibration interrupt status flag (unused): b0: nothing; b1: calibration process has ended           |
    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------+

.. _rtc_internals__CALENDAR_CONTROL:

CALENDAR_CONTROL
""""""""""""""""

RTC calendar control register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+--------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                  Description                                   |
    +=====+===+====+=====+================================================================================+
    |    0|R/W|SB  |0x0  |Calendar standby configuration: b0: calendar is active; b1: calendar is disabled|
    +-----+---+----+-----+--------------------------------------------------------------------------------+

.. _rtc_internals__CALENDAR_TIME:

CALENDAR_TIME
"""""""""""""

RTC calendar time register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------+
    |Bit #|R/W|Name|Reset|         Description          |
    +=====+===+====+=====+==============================+
    |3:0  |R/W|SEC0|0x0  |Calendar time: seconds digit 0|
    +-----+---+----+-----+------------------------------+
    |7:4  |R/W|SEC1|0x0  |Calendar time: seconds digit 1|
    +-----+---+----+-----+------------------------------+
    |11:8 |R/W|MIN0|0x0  |Calendar time: minutes digit 0|
    +-----+---+----+-----+------------------------------+
    |15:12|R/W|MIN1|0x0  |Calendar time: minutes digit 1|
    +-----+---+----+-----+------------------------------+
    |19:16|R/W|HOU0|0x0  |Calendar time: hours digit 0  |
    +-----+---+----+-----+------------------------------+
    |23:20|R/W|HOU1|0x0  |Calendar time: hours digit 1  |
    +-----+---+----+-----+------------------------------+

.. _rtc_internals__CALENDAR_DATE:

CALENDAR_DATE
"""""""""""""

RTC calendar date register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+----------------------------+
    |Bit #|R/W|Name|Reset|        Description         |
    +=====+===+====+=====+============================+
    |3:0  |R/W|DAY0|0x1  |Calendar date: day digit 0  |
    +-----+---+----+-----+----------------------------+
    |7:4  |R/W|DAY1|0x0  |Calendar date: day digit 1  |
    +-----+---+----+-----+----------------------------+
    |11:8 |R/W|MON0|0x1  |Calendar date: month digit 0|
    +-----+---+----+-----+----------------------------+
    |15:12|R/W|MON1|0x0  |Calendar date: month digit 1|
    +-----+---+----+-----+----------------------------+
    |19:16|R/W|YEA0|0x0  |Calendar date: year digit 0 |
    +-----+---+----+-----+----------------------------+
    |23:20|R/W|YEA1|0x0  |Calendar date: year digit 1 |
    +-----+---+----+-----+----------------------------+

.. _rtc_internals__ALARM_CONTROL:

ALARM_CONTROL
"""""""""""""

RTC alarm control register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                                                         Description                                                                          |
    +=====+===+====+=====+==============================================================================================================================================================+
    |    0|R/W|SB  |0x1  |Alarm 1 standby configuration: b0: alarm is active; b1: alarm is inactive. In single mode, this bitfield is automatically updated to b1 after the event occurs|
    +-----+---+----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|MODE|0x0  |Alarm 1 mode configuration: b0: single; b1: repeat                                                                                                            |
    +-----+---+----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |19:16|R/W|CFG |0x6  |Alarm 1 repeat configuration: b0011: every second; b0100: every minute; b0101: every hour; b0110: every day; b0111: every month; b1000: every year            |
    +-----+---+----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _rtc_internals__ALARM1_TIME:

ALARM1_TIME
"""""""""""

RTC alarm 1 time register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-----------------------------+
    |Bit #|R/W|Name|Reset|         Description         |
    +=====+===+====+=====+=============================+
    |3:0  |R/W|SEC0|0x0  |Alarm 1 time: seconds digit 0|
    +-----+---+----+-----+-----------------------------+
    |7:4  |R/W|SEC1|0x0  |Alarm 1 time: seconds digit 1|
    +-----+---+----+-----+-----------------------------+
    |11:8 |R/W|MIN0|0x0  |Alarm 1 time: minutes digit 0|
    +-----+---+----+-----+-----------------------------+
    |15:12|R/W|MIN1|0x0  |Alarm 1 time: minutes digit 1|
    +-----+---+----+-----+-----------------------------+
    |19:16|R/W|HOU0|0x0  |Alarm 1 time: hours digit 0  |
    +-----+---+----+-----+-----------------------------+
    |23:20|R/W|HOU1|0x0  |Alarm 1 time: hours digit 1  |
    +-----+---+----+-----+-----------------------------+

.. _rtc_internals__ALARM1_DATE:

ALARM1_DATE
"""""""""""

RTC alarm 1 date register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+---------------------------+
    |Bit #|R/W|Name|Reset|        Description        |
    +=====+===+====+=====+===========================+
    |3:0  |R/W|DAY0|0x0  |Alarm 1 date: day digit 0  |
    +-----+---+----+-----+---------------------------+
    |7:4  |R/W|DAY1|0x0  |Alarm 1 date: day digit 1  |
    +-----+---+----+-----+---------------------------+
    |11:8 |R/W|MON0|0x0  |Alarm 1 date: month digit 0|
    +-----+---+----+-----+---------------------------+
    |15:12|R/W|MON1|0x0  |Alarm 1 date: month digit 1|
    +-----+---+----+-----+---------------------------+
    |19:16|R/W|YEA0|0x0  |Alarm 1 date: year digit 0 |
    +-----+---+----+-----+---------------------------+
    |23:20|R/W|YEA1|0x0  |Alarm 1 date: year digit 1 |
    +-----+---+----+-----+---------------------------+

.. _rtc_internals__COUNTDOWN_CONTROL:

COUNTDOWN_CONTROL
"""""""""""""""""

RTC countdown control register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                                                                        Description                                                                                         |
    +=====+===+====+=====+============================================================================================================================================================================================+
    |    0|R/W|SB  |0x1  |Countdown timer 1 standby configuration: b0: countdown timer is active; b1: countdown timer is inactive. In single mode, this bitfield is automatically updated to b1 after the event occurs|
    +-----+---+----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|MODE|0x0  |Countdown timer 1 mode configuration: b0: single; b1: repeat                                                                                                                                |
    +-----+---+----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _rtc_internals__COUNTDOWN1_INIT:

COUNTDOWN1_INIT
"""""""""""""""

RTC countdown 1 initialisation register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+----------+-------------------------------+
    |Bit #|R/W|Name|  Reset   |          Description          |
    +=====+===+====+==========+===============================+
    |31:0 |R/W|VAL |0xFFFFFFFF|Countdown timer 1 initial value|
    +-----+---+----+----------+-------------------------------+

.. _rtc_internals__COUNTDOWN1_TIMER:

COUNTDOWN1_TIMER
""""""""""""""""

RTC countdown 1 timer register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+----------+-------------------------------+
    |Bit #|R/W|Name|  Reset   |          Description          |
    +=====+===+====+==========+===============================+
    |31:0 |R  |VAL |0x00000000|Countdown timer 1 current value|
    +-----+---+----+----------+-------------------------------+

.. _rtc_internals__CKIN_DIV1:

CKIN_DIV1
"""""""""

RTC clock divider 1 register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+------+-------------------------------------+
    |Bit #|R/W|Name|Reset |             Description             |
    +=====+===+====+======+=====================================+
    |15:0 |R/W|VAL |0x8000|Countdown timer 1 clock divider value|
    +-----+---+----+------+-------------------------------------+

.. _rtc_internals__CKREF_CONF:

CKREF_CONF
""""""""""

RTC reference clock configuration register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+--------+-----------------------------------------+
    |Bit #|R/W|Name| Reset  |               Description               |
    +=====+===+====+========+=========================================+
    |21:0 |R/W|VAL |0x0F4240|Countdown 1 reference clock configuration|
    +-----+---+----+--------+-----------------------------------------+

.. _rtc_internals__RTC_TEST_REG_A:

RTC_TEST_REG_A
""""""""""""""

RTC test register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-----------------+
    |Bit #|R/W|  Name  |  Reset   |   Description   |
    +=====+===+========+==========+=================+
    |21:0 |R/W|RESERVED|0x00001E3D|Reserved for test|
    +-----+---+--------+----------+-----------------+

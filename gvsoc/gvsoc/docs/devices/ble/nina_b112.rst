NINA B112
---------

The Nina B112 model mimicks basic functions of a real Ublox Nina B112 SPP
(Serial Peripheral Profile).

The real device is used to act as a UART bridge over Bluetooth.

Current model supports only a minimal subset of all AT commands.
It does not support extended data mode (EDM).

When in data mode it acts as a loopback. Once a number of bytes were received,
it sends everything back.

It supports RTS generation. When enabled, the model generates RTS for a
specific duration on two conditions:

- once a number of words have been received,
- randomly using a uniform random number generator. It generates a number
  between 0 and a high limit. If the generated number is below a threshold it
  triggers a RTS.

Serial parameters
=================

+--------------+---------------------+
| Parameter    | Supported values    |
+==============+=====================+
| Baudrate     | - 19200             |
|              | - 38400             |
|              | - 57600             |
|              | - 115200 (default)  |
|              | - 230400            |
|              | - 460800            |
|              | - 1000000           |
+--------------+---------------------+
| Data bits    | - 8 (default)       |
+--------------+---------------------+
| Stop bits    | - 1 (default)       |
+--------------+---------------------+
| Parity       | - none (default)    |
|              | - even              |
+--------------+---------------------+
| Flow control | - enabled (default) |
|              | - disabled          |
+--------------+---------------------+


AT commands
===========

By default, if a command is not supported, model will respond :code:`ERROR`.
If command arguments are invalid, it will also respond with an error.

Note: :code:`AT` is omitted.

+-------------------+----------+--------------------------------------------+
| Command           | Support  | Comments                                   |
+===================+==========+============================================+
| (empty)           | Complete |                                            |
+-------------------+----------+--------------------------------------------+
| :code:`E`         | Partial  | No effect                                  |
+-------------------+----------+--------------------------------------------+
| :code:`+UFACTORY` | Partial  | No effect                                  |
+-------------------+----------+--------------------------------------------+
| :code:`O`         | Partial  | Extended Data Mode not supported           |
+-------------------+----------+--------------------------------------------+
| :code:`+UBTUB=`   | Partial  | No effect                                  |
+-------------------+----------+--------------------------------------------+
| :code:`+UBTLE=`   | Partial  | No effect                                  |
+-------------------+----------+--------------------------------------------+
| :code:`+UBTLN=`   | Partial  | Notes:                                     |
|                   |          |                                            |
|                   |          | - Command is accepted.                     |
|                   |          | - Local name is not saved/checked.         |
+-------------------+----------+--------------------------------------------+
| :code:`+UBTLN?`   | Complete | Returns local name                         |
+-------------------+----------+--------------------------------------------+
| :code:`+UMRS=`    | Complete | Change serial parameters:                  |
|                   |          |                                            |
|                   |          | - immediately for RX,                      |
|                   |          | - when send queue has been emptied for TX. |
+-------------------+----------+--------------------------------------------+
| :code:`+UMRS?`    | Complete | Returns serial parameters                  |
+-------------------+----------+--------------------------------------------+

JSON Configuration
==================

Here are all the parameters that can be configured:

.. code::

    {
        "behavior": {
            "loopback_size": 200
        },
        "rts": {
            "enabled" : true,
            "buffer_limit": 200,
            "duration": 200,
            "random_seed": 12345,
            "random_high": 10000,
            "random_threshold": 100
        }
    }

:code:`behavior` controls how the model reacts:

- :code:`loopback_size` is the number of words that need to be received in data
  mode before sending it back.

:code:`rts` controls the RTS generation of the model:

- :code:`enabled`: whether RTS generation is enabeled or not.
- :code:`buffer_limit`: once this number of words have been received received,
  it triggers a RTS.
- :code:`duration`: duration of the RTS in number of UART cycles.
- :code:`random_seed`: seed used by the random generator for the random RTS
  generation.
- :code:`random_high`: high limit of the random number generator.
- :code:`random_threshold`: threshold below which the random generated number
  will trigger a RTS.


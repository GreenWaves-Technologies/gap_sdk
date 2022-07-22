.. _guide_aes:

Using AES to secure your memory accesses
========================================

.. warning::

    All instructions in this document are only valid with GAP9,
    **GAP8 has no hw support** for AES.

.. note::

    SW implementations are possible, but are out of the scope of this document.

Introduction
------------

In GAP9, two different AES IPs can be found.

    - AES Dual Core (AESDC) is directly linked with Octospi IPs
    - AES is made for L2 to L2 encryption/decryption

This document will focus on AESDC.
In particular, with methods to protect your code and data stored in external memory.

Limitations
-----------

Using AESDC, AES is restricted to ECB mode. Data must also be aligned on 16 bytes.

GAP9 encrypted flash access
---------------------------

In this section, it is considered that the flash content is already encrypted.
This can be done via various PC tools which are not presented here.
The encrypted file can then be flashed with usual methods. See relevant documenation
for more information on this part.

To access the encrypted flash with seamless decryption, an aes configuration
must first be setup. This is done like so:


.. code-block:: C

    // first declare a key
    static uint8_t aes_key_ecb_128[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F,
    };
    // secondly prepare an aes configuration from it
    static pi_aes_utils_conf_t aes_configurations[] = {
    {
        /* ECB 128 */
        .enabled = 1,
        .mode = PI_AES_MODE_ECB,
        .key_len = PI_AES_KEY_128,
        .key = (uint32_t*) aes_key_ecb_128,
        .iv = NULL,
    },
    };

Detailed elements are described in AES pmsis_api reference.
In short, this is an ECB, 128 bits configuration with the key :code:`0123456789ABCDEF`.
The first thing to do in a real case would be to use the encryption key used on PC.

Secondly, the flash configuration will be updated to use this AES configuration.

.. code-block:: C

    /* Init & open flash. */
    pi_default_flash_conf_init(&flash_conf);
    // add aes configuration
    memcpy(&(flash_conf.flash.aes_conf), &aes_configurations[0], sizeof(pi_aes_utils_conf_t));
    // open normally
    pi_open_from_conf(&flash_dev, &flash_conf);
    if (pi_flash_open(&flash_dev))
    {
        printf("Error flash open !\n");
        pmsis_exit(-3);
    }

And that is all, all read (and writes) to the flash will now be seamlessly
encrypted/decrypted in HW.
Simply execute a :code:`pi_flash_read` at the adress where the encrypted binary was stored.

GAP9 encryption and decryption of RAM
-------------------------------------

The way to proceed is essentially the same as for flash configuration, except
that no flashing and no PC side preparation has to be done.

The same aes key and configuration is first prepared:

.. code-block:: C

    // first declare a key
    static uint8_t aes_key_ecb_128[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F,
    };
    // secondly prepare an aes configuration from it
    static pi_aes_utils_conf_t aes_configurations[] = {
    {
        /* ECB 128 */
        .enabled = 1,
        .mode = PI_AES_MODE_ECB,
        .key_len = PI_AES_KEY_128,
        .key = (uint32_t*) aes_key_ecb_128,
        .iv = NULL,
    },
    };

Then, this configuration is attached to the ram before opening it:

.. code-block:: C

    /* Init & open ram. */
    pi_default_ram_conf_init(&ram_conf);
    /* add aes to octospiram */
    memcpy(&(ram_conf.ram.aes_conf), &aes_configurations[0], sizeof(pi_aes_utils_conf_t));
    pi_open_from_conf(&ram_dev, &ram_conf);
    pi_ram_open(&ram_dev);

Now, considering a buffer of data :code:`secret_data` has been create bewrite it securely like so:

.. code-block:: C

    pi_ram_alloc(&ram_dev, &ram_buff, BUFFER_SIZE)
    pi_ram_write(&ram_dev, ram_buff, secret_data, (uint32_t) BUFFER_SIZE);
    // and to access it, simply use:
    pi_ram_read(&ram_dev, ram_buff, rcv_secret_data, (uint32_t) BUFFER_SIZE);

Using with XIP
--------------

If using with XIP, nothing particular has to be done.
Only one limitation applies, if two CS are used with XIP, then they must
both be encrypted. XIP does not support per CS encryption as it is seen
as a unique mapping.

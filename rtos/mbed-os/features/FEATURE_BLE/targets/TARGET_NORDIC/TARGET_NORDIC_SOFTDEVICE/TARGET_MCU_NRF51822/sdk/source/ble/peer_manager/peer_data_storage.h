/*
 * Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef PEER_DATA_STORAGE_H__
#define PEER_DATA_STORAGE_H__


#include "stdint.h"
#include "sdk_errors.h"
#include "ble_gap.h"
#include "peer_manager_types.h"
#include "fds.h"


/**
 * @defgroup peer_data_storage Peer Data Storage
 * @ingroup peer_manager
 * @{
 * @brief An internal module of @ref peer_manager. This module provides a Peer Manager-specific API
 *        to the persistent storage.
 */

#define PDS_PREPARE_TOKEN_INVALID 0
#define PDS_STORE_TOKEN_INVALID   0


typedef enum
{
    peer_id_to_instance_id  = 16384,
    instance_id_to_peer_id  = -peer_id_to_instance_id,
    peer_data_id_to_type_id = 32768,
    type_id_to_peer_data_id = -peer_data_id_to_type_id,
} pds_convert_t;


/**@brief The types of events that can come from the peer_data_storage module.
 */
typedef enum
{
    PDS_EVT_STORED,                 /**< The specified data has been successfully stored. */
    PDS_EVT_CLEARED,                /**< The specified data has been successfully cleared. */
    PDS_EVT_PEER_ID_CLEAR,          /**< The peer id has been successfully cleared. */
    PDS_EVT_ERROR_STORE,            /**< The specified data could not be stored. */
    PDS_EVT_ERROR_CLEAR,            /**< The specified data could not be cleared. */
    PDS_EVT_ERROR_PEER_ID_CLEAR,    /**< The peer id has been successfully cleared. */
    PDS_EVT_COMPRESSED,             /**< A compress procedure has finished successfully. */
} pds_evt_id_t;


/**@brief Events that can come from the peer_data_storage module.
 */
typedef struct
{
    pds_evt_id_t      evt_id;       /**< The type of event. */
    pm_peer_id_t      peer_id;      /**< The peer the event pertains to. */
    pm_peer_data_id_t data_id;      /**< The data the event pertains to. */
    pm_store_token_t  store_token;
} pds_evt_t;


/**@brief Event handler for events from the peer_data_storage module.
 *
 * @param[in]  event    The event that has happened.
 * @param[in]  peer_id  The id of the peer the event pertains to.
 * @param[in]  flags    The data the event pertains to.
 */
typedef void (*pds_evt_handler_t)(pds_evt_t const * p_event);


/**@brief Function for registering for events from the peer database.
 *
 * @note This function will initialize the module if it is not already initialized.
 *
 * @param[in]  evt_handler  Event handler to register.
 *
 * @retval NRF_SUCCESS              Registration successful.
 * @retval NRF_ERROR_NO_MEM         No more event handlers can be registered.
 * @retval NRF_ERROR_NULL           evt_handler was NULL.
 * @retval NRF_ERROR_INVALID_PARAM  Unexpected return code from @ref pm_buffer_init.
 * @retval NRF_ERROR_INVALID_STATE  FDS has not been initalized.
 */
ret_code_t pds_register(pds_evt_handler_t evt_handler);


#if 0
/**@brief Function for initializing Peer Data storage and registering a
 * callback for its events.
 *
 * @param[in] evt_handler  Event handler to register.
 *
 * @retval NRF_SUCCESS              Registration successful.
 * @retval NRF_ERROR_NO_MEM         No more event handlers can be registered.
 * @retval NRF_ERROR_NULL           evt_handler was NULL.
 * @retval NRF_ERROR_INVALID_STATE  FDS has not completed initialization.
 */
ret_code_t pds_init(pds_evt_handler_t evt_handler);
#endif

/**@brief Function for retrieving a direct pointer to peer data in persistent storage.
 *
 * @param[in]  peer_id      The id of the peer whose data to read.
 * @param[in]  data_id      Which data to get.
 * @param[out] p_data       The peer data pointer.
 * @param[out] p_token      Token that can be used to lock data in flash and check data validity.
 *
 * @retval NRF_SUCCESS              The pointer was successfully retrieved.
 * @retval NRF_ERROR_INVALID_PARAM  Invalid data_id.
 * @retval NRF_ERROR_NULL           p_data was NULL.
 * @retval NRF_ERROR_NOT_FOUND      The requested data was not found in persistent storage.
 * @retval NRF_ERROR_INVALID_STATE  Module is not initialized.
 */
ret_code_t pds_peer_data_read_ptr_get(pm_peer_id_t            peer_id,
                                      pm_peer_data_id_t       data_id,
                                      pm_peer_data_flash_t  * p_data,
                                      pm_store_token_t      * p_token);

/**@brief Function to lock the flash data (to defer compression from invalidating data)
 *
 * @param[in]   store_token     The token representing the item to lock
 *
 */
ret_code_t pds_peer_data_lock(pm_store_token_t store_token);


/**@brief Function to verify flash data integrity
 *
 * @param[in]   store_token     The token representing the item to lock
 *
 * @retval NRF_SUCCESS              The data integrity is valid.
 * @retval NRF_ERROR_NULL           The token is invalid.
 * @retval NRF_ERROR_INVALID_DATA   The data integrity is not valid.
 * @retval NRF_ERROR_INVALID_STATE  Module is not initialized.
 */
ret_code_t  pds_peer_data_verify(pm_store_token_t store_token);


/**@brief Function for retrieving peer data from persistent storage by making a copy
 *
 * @param[in]       peer_id     The id of the peer whose data to read.
 * @param[in]       data_id     Which piece of data to read.
 * @param[out]      p_data      Pointer to the peer data.
 * @param[in,out]   p_len_words Length available to copy to (in words).
 *                              If set to NULL, then no copy will be made and the
 *                              length will be reflected in p_len_words after the call returns.
 *
 * @retval NRF_SUCCESS              The read was successful.
 * @retval NRF_ERROR_INVALID_PARAM  Invalid data_id.
 * @retval NRF_ERROR_NULL           data contained a NULL pointer.
 * @retval NRF_ERROR_NOT_FOUND      The requested data was not found in persistent storage.
 * @retval NRF_ERROR_NO_MEM         The length of stored data too large to copy out
 * @retval NRF_ERROR_INVALID_STATE  Module is not initialized.
 */
ret_code_t pds_peer_data_read(pm_peer_id_t          peer_id,
                              pm_peer_data_id_t     data_id,
                              pm_peer_data_t      * p_data,
                              fds_length_t        * p_len_words);


/**@brief Function for preparing persistent storage for a write.
 *
 * @details If this call succeeds, space is reserved in persistent storage, so the write will fit.
 *
 * @note If space has already been prepared for this peer_id/data_id pair, no new space will be
 *       reserved, unless the previous reservation had too small size.
 *
 * @param[in]  p_peer_data      Data to prepare for. The data needs not be ready, but length and type
 *                              values must.
 * @param[out] p_prepare_token  A token identifying the prepared memory area.
 *
 * @retval NRF_SUCCESS               The call was successful.
 * @retval NRF_ERROR_INVALID_PARAM   Invalid data ID.
 * @retval NRF_ERROR_INVALID_LENGTH  Data length above the maximum allowed.
 * @retval NRF_ERROR_NO_MEM          No space available in persistent storage.
 * @retval NRF_ERROR_INVALID_STATE   Module is not initialized.
 */
ret_code_t pds_peer_data_write_prepare(pm_peer_data_const_t const * p_peer_data,
                                       pm_prepare_token_t         * p_prepare_token);


/**@brief Function for undoing a previous call to @ref pds_peer_data_write_prepare.
 *
 * @param[in]  prepare_token  A token identifying the prepared memory area to cancel.
 *
 * @retval NRF_SUCCESS               The call was successful.
 * @retval NRF_ERROR_NOT_FOUND       Invalid peer ID and/or prepare token.
 * @retval NRF_ERROR_INVALID_STATE   Module is not initialized.
 */
ret_code_t pds_peer_data_write_prepare_cancel(pm_prepare_token_t prepare_token);


/**@brief Function for writing prepared (reserved) peer data to persistent storage.
 *
 * @details Writing happens asynchronously. Expect a @ref PDS_EVT_STORED or @ref PDS_EVT_ERROR_STORE
 *          event.
 *
 * @param[in]  peer_id        The id of the peer the data pertains to.
 * @param[in]  p_peer_data    The peer data.
 * @param[in]  prepare_token  A token identifying the prepared memory area to write into. If
 *                            the prepare token is invalid, e.g. PDS_PREPARE_TOKEN_INVALID, the
 *                            prepare/write sequence will happen atomically.
 * @param[out] p_store_token  A token identifying this particular store operation. The token can be
 *                            used to identify events pertaining to this operation.
 *
 * @retval NRF_SUCCESS               The write was initiated successfully.
 * @retval NRF_ERROR_INVALID_PARAM   Invalid data ID or store_flags.
 * @retval NRF_ERROR_INVALID_LENGTH  Length of data longer than in prepare call.
 * @retval NRF_ERROR_NULL            data contained a NULL pointer.
 * @retval NRF_ERROR_NO_MEM          No space available in persistent storage. This can only happen
 *                                   if p_prepare_token is NULL.
 * @retval NRF_ERROR_BUSY            FDS or underlying modules are busy and can't take any
 *                                   more requests
 * @retval NRF_ERROR_INVALID_STATE   Module is not initialized.
 */
ret_code_t pds_peer_data_write_prepared(pm_peer_id_t                    peer_id,
                                        pm_peer_data_const_t    const * p_peer_data,
                                        pm_prepare_token_t              prepare_token,
                                        pm_store_token_t              * p_store_token);


/**@brief Function for writing peer data to persistent storage.
 *
 * @details Writing happens asynchronously. Expect a @ref PDS_EVT_STORED or @ref PDS_EVT_ERROR_STORE
 *          event.
 *
 * @param[in]  peer_id        The id of the peer the data pertains to.
 * @param[in]  p_peer_data    The peer data.
 * @param[out] p_store_token  A token identifying this particular store operation. The token can be
 *                            used to identify events pertaining to this operation.
 *
 * @retval NRF_SUCCESS               The write was initiated successfully.
 * @retval NRF_ERROR_INVALID_PARAM   Invalid data ID or store_flags.
 * @retval NRF_ERROR_NULL            Data contained a NULL pointer.
 * @retval NRF_ERROR_NO_MEM          No space available in persistent storage. This can only happen
 *                                   if p_prepare_token is NULL.
 * @retval NRF_ERROR_BUSY            FDS or underlying modules are busy and can't take any
 *                                   more requests
 * @retval NRF_ERROR_INVALID_STATE   Module is not initialized.
 */
ret_code_t pds_peer_data_write(pm_peer_id_t                 peer_id,
                               pm_peer_data_const_t const * p_peer_data,
                               pm_store_token_t           * p_store_token);


/**@brief Function for updating currently stored peer data to a new version
 *
 * @details Updating happens asynchronously.
 *          Expect a @ref PDS_EVT_STORED or @ref PDS_EVT_ERROR_STORE for the store token
 *          and a @ref PDS_EVT_ERROR_CLEAR or @ref PDS_EVT_ERROR_CLEAR for the old token
 *
 * @param[in]   peer_id             The peer which the data is associated to.
 * @param[in]   peer_data           New data.
 * @param[in]   old_token           Store token for the old data.
 * @param[out]  p_store_token       Store token for the new data.
 *
 * @retval NRF_SUCESS               The update was initiated successfully
 * @retval NRF_ERROR_NOT_FOUND      The old store token was invalid.
 * @retval NRF_ERROR_NULL           Data contained a NULL pointer.
 * @retval NRF_ERROR_NO_MEM         No space available in persistent storage.
 * @retval NRF_ERROR_BUSY           FDS or underlying modules are busy and can't take any
 *                                  more requests
 * @retval NRF_ERROR_INVALID_STATE  Module is not initialized.
 */
ret_code_t pds_peer_data_update(pm_peer_id_t                 peer_id,
                                pm_peer_data_const_t const * peer_data,
                                pm_store_token_t             old_token,
                                pm_store_token_t           * p_store_token);


/**@brief Function for clearing peer data from persistent storage.
 *
 * @details Clearing happens asynchronously. Expect a @ref PDS_EVT_CLEARED or @ref PDS_EVT_ERROR_CLEAR
 *          event.
 *
 * @param[in]  peer_id  The id of the peer the data pertains to.
 * @param[in]  data_id  Which data to clear.
 *
 * @retval NRF_SUCCESS              The clear was initiated successfully.
 * @retval NRF_ERROR_INVALID_PARAM  Data ID or was invalid.
 * @retval NRF_ERROR_NOT_FOUND      Nothing to clear for this peer ID.
 * @retval NRF_ERROR_INVALID_STATE  Module is not initialized.
 */
ret_code_t pds_peer_data_clear(pm_peer_id_t peer_id, pm_peer_data_id_t data_id);


/**@brief Function for claiming an unused peer ID.
 *
 * @return  The first unused peer ID.
 * @retval  PM_PEER_ID_INVALID  If no peer ID is available or module is not initialized.
 */
pm_peer_id_t pds_peer_id_allocate(void);


/**@brief Function for freeing a peer ID and clearing all data associated with it in persistent
 *        storage.
 *
 * @param[in]  peer_id  Peer ID to free.
 *
 * @retval NRF_SUCCESS          The clear was initiated successfully
 * @retval NRF_ERROR_BUSY       Another peer_id clear was already requested or fds queue full
 */
ret_code_t pds_peer_id_free(pm_peer_id_t peer_id);


/**@brief Function for finding out whether a peer ID is in use.
 *
 * @param[in]  peer_id  The peer ID to inquire about.
 *
 * @retval  true   peer_id is in use.
 * @retval  false  peer_id is free, or the module is not initialized.
 */
bool pds_peer_id_is_allocated(pm_peer_id_t peer_id);


/**@brief Function for getting the next peer ID in the sequence of all used peer IDs. Can be
 *        used to loop through all used peer IDs.
 *
 * @note @ref PM_PEER_ID_INVALID is considered to be before the first and after the last ordinary
 *       peer ID.
 *
 * @param[in]  prev_peer_id  The previous peer ID.
 *
 * @return  The next peer ID.
 * @return  The first ordinary peer ID  if prev_peer_id was @ref PM_PEER_ID_INVALID.
 * @retval  PM_PEER_ID_INVALID          if prev_peer_id was the last ordinary peer ID or the module
 *                                      is not initialized.
 */
pm_peer_id_t pds_next_peer_id_get(pm_peer_id_t prev_peer_id);


/**@brief Function for querying the number of valid peer IDs available. I.E the number of peers
 *        in persistent storage.
 *
 * @return  The number of valid peer IDs, or 0 if module is not initialized.
 */
uint32_t pds_n_peers(void);


/** @} */

#endif /* PEER_DATA_STORAGE_H__ */

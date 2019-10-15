/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************
 */

#include "MaximGattServer.h"
#include "mbed.h"
#include "MaximGap.h"
#include "wsf_types.h"
#include "att_api.h"

typedef struct mxmChar_s {
    uint16_t descLen;
    mxmChar_s() {}
} mxmChar_t;

typedef struct mxmService_s mxmService_t;
struct mxmService_s {
    uint16_t uuidLen;
    mxmChar_t *chars;
    attsGroup_t *attGroup;
    mxmService_t *next;
    mxmService_s() {}
};

static uint16_t currentHandle = 0x20;

static UUID cccUUID(BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG);
static const uint16_t cccSize = sizeof(uint16_t);

MaximGattServer &MaximGattServer::getInstance() {
    static MaximGattServer m_instance;
    return m_instance;
}

ble_error_t MaximGattServer::addService_(GattService &service)
{
    currentHandle = (currentHandle + 0xF) & ~0xF;
    uint16_t startHandle = currentHandle;

    mxmService_t *mxmSvc = new mxmService_t;

    // Create WiCentric attribute group
    mxmSvc->attGroup = new attsGroup_t;

    // Determine the attribute list length
    unsigned int attListLen = 1;
    for (int i = 0; i < service.getCharacteristicCount(); i++) {
        attListLen += 2;
        GattCharacteristic *p_char = service.getCharacteristic(i);

        if (p_char->getProperties() & (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE)) {
            // add a CCCD
            attListLen++;
        }
    }

    // Create WiCentric attribute list
    mxmSvc->attGroup->pAttr = (attsAttr_t*)malloc(attListLen * sizeof(attsAttr_t));;
    if (mxmSvc->attGroup->pAttr == NULL) {
        return BLE_ERROR_BUFFER_OVERFLOW;
    }

    // Create characteristics
    mxmSvc->chars = new mxmChar_t [service.getCharacteristicCount()];

    attsAttr_t *currAtt = mxmSvc->attGroup->pAttr;

    /* Service */
    currAtt->pUuid = attPrimSvcUuid;
    if (service.getUUID().shortOrLong() == UUID::UUID_TYPE_LONG) {
        mxmSvc->uuidLen = UUID::LENGTH_OF_LONG_UUID;
    } else {
        mxmSvc->uuidLen = sizeof(UUID::ShortUUIDBytes_t);
    }
    currAtt->pValue = (uint8_t*)malloc(mxmSvc->uuidLen);
    memcpy(currAtt->pValue, service.getUUID().getBaseUUID(), mxmSvc->uuidLen);
    currAtt->maxLen = mxmSvc->uuidLen;
    currAtt->pLen = &mxmSvc->uuidLen;
    currAtt->settings = 0;
    currAtt->permissions = ATTS_PERMIT_READ;

    currAtt++;

    /* Add characteristics to the service */
    for (int i = 0; i < service.getCharacteristicCount(); i++) {
        GattCharacteristic *p_char = service.getCharacteristic(i);

        /* Skip any incompletely defined, read-only characteristics. */
        if ((p_char->getValueAttribute().getValuePtr() == NULL) &&
            (p_char->getValueAttribute().getLength() == 0) &&
            (p_char->getProperties() == GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ)) {
            continue;
        }

        // Create Characteristic Attribute
        currentHandle += 2;
        currAtt->pUuid = attChUuid;

        p_char->getValueAttribute().setHandle(currentHandle);
        mxmSvc->chars[i].descLen = 1 + sizeof(currentHandle) + p_char->getValueAttribute().getUUID().getLen();
        currAtt->pValue = (uint8_t*)malloc(mxmSvc->chars[i].descLen);
        uint8_t *pValue = currAtt->pValue;
        *pValue++ = p_char->getProperties();
        memcpy(pValue, &currentHandle, sizeof(currentHandle));
        pValue += sizeof(currentHandle);
        memcpy(pValue, p_char->getValueAttribute().getUUID().getBaseUUID(), p_char->getValueAttribute().getUUID().getLen());

        currAtt->pLen = &mxmSvc->chars[i].descLen;
        currAtt->maxLen = mxmSvc->chars[i].descLen;
        currAtt->settings = 0;
        currAtt->permissions = ATTS_PERMIT_READ;
        currAtt++;

        // Create Value Attribute
        currAtt->pUuid = p_char->getValueAttribute().getUUID().getBaseUUID();
        currAtt->pValue = p_char->getValueAttribute().getValuePtr();
        currAtt->pLen = p_char->getValueAttribute().getLengthPtr();
        currAtt->maxLen = p_char->getValueAttribute().getMaxLength();
        currAtt->settings = ATTS_SET_WRITE_CBACK | ATTS_SET_READ_CBACK;

        if(p_char->getValueAttribute().hasVariableLength()) {
            currAtt->settings |= ATTS_SET_VARIABLE_LEN;
        }

        if (p_char->getValueAttribute().getUUID().shortOrLong() == UUID::UUID_TYPE_LONG) {
            currAtt->settings |= ATTS_SET_UUID_128;
        }
        currAtt->permissions = 0;
        if (p_char->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ)  { currAtt->permissions |= ATTS_PERMIT_READ; }
        if (p_char->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE) { currAtt->permissions |= ATTS_PERMIT_WRITE; }
        currAtt++;

        bool cccCreated = false;

        for (int i = 0; i < p_char->getDescriptorCount(); i++) {
            GattAttribute *p_att = p_char->getDescriptor(i);

            currentHandle++;

            currAtt->pUuid = p_att->getUUID().getBaseUUID();
            currAtt->pValue = p_att->getValuePtr();
            currAtt->pLen = p_att->getLengthPtr();
            currAtt->maxLen = p_att->getMaxLength();
            currAtt->settings = 0;
            currAtt->permissions = 0;
            if (p_att->getUUID().shortOrLong() == UUID::UUID_TYPE_LONG) {
                currAtt->settings |= ATTS_SET_UUID_128;
            }
            if (p_att->getUUID() == UUID(BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG)) {
                cccCreated = true;
                currAtt->settings |= ATTS_SET_CCC;
                currAtt->permissions |= ATTS_PERMIT_READ;
                currAtt->permissions |= ATTS_PERMIT_WRITE;

                if (cccCnt < MAX_CCC_CNT) {
                    cccSet[cccCnt].handle = currentHandle;
                    cccSet[cccCnt].valueRange = 0;
                    if (p_char->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {
                        cccSet[cccCnt].valueRange |= ATT_CLIENT_CFG_NOTIFY;
                    }
                    if (p_char->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE) {
                        cccSet[cccCnt].valueRange |= ATT_CLIENT_CFG_INDICATE;
                    }
                    cccSet[cccCnt].secLevel = DM_SEC_LEVEL_NONE;
                    cccHandles[cccCnt] = p_char->getValueAttribute().getHandle();
                    cccCnt++;
                } else {
                    return BLE_ERROR_PARAM_OUT_OF_RANGE;
                }
            }
            currAtt++;
        }

        if (!cccCreated && (p_char->getProperties() & (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE))) {
            /* There was not a CCCD included in the descriptors, but this
             * characteristic is notifiable and/or indicatable. A CCCD is
             * required so create one now.
             */
            if (cccCnt >= MAX_CCC_CNT) {
                return BLE_ERROR_PARAM_OUT_OF_RANGE;
            }

            currentHandle++;

            currAtt->pUuid = cccUUID.getBaseUUID();
            currAtt->pValue = (uint8_t*)&cccValues[cccCnt];
            currAtt->pLen = (uint16_t*)&cccSize;
            currAtt->maxLen = sizeof(uint16_t);
            currAtt->settings = ATTS_SET_CCC;
            currAtt->permissions = (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE);

            cccSet[cccCnt].handle = currentHandle;
            cccSet[cccCnt].valueRange = 0;
            if (p_char->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {
                cccSet[cccCnt].valueRange |= ATT_CLIENT_CFG_NOTIFY;
            }
            if (p_char->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE) {
                cccSet[cccCnt].valueRange |= ATT_CLIENT_CFG_INDICATE;
            }
            cccSet[cccCnt].secLevel = DM_SEC_LEVEL_NONE;
            cccHandles[cccCnt] = p_char->getValueAttribute().getHandle();

            cccCnt++;
            currAtt++;
        }
    }

    mxmSvc->attGroup->pNext = NULL;
    mxmSvc->attGroup->readCback = attsReadCback;
    mxmSvc->attGroup->writeCback = attsWriteCback;
    mxmSvc->attGroup->startHandle = startHandle;
    mxmSvc->attGroup->endHandle = currentHandle;
    AttsAddGroup(mxmSvc->attGroup);

    AttRegister(attCback);
    AttsCccRegister(cccCnt, (attsCccSet_t*)cccSet, cccCback);

    return BLE_ERROR_NONE;
}

ble_error_t MaximGattServer::read_(GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *const lengthP)
{
    if (AttsGetAttr(attributeHandle, lengthP, &buffer) != ATT_SUCCESS) {
        return BLE_ERROR_PARAM_OUT_OF_RANGE;
    }

    return BLE_ERROR_NONE;
}

ble_error_t MaximGattServer::read_(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP)
{
    // Check to see if this is a CCCD
    uint8_t idx;
    for (idx = 0; idx < cccCnt; idx++) {
        if (attributeHandle == cccSet[idx].handle) {
            if (connectionHandle == DM_CONN_ID_NONE) { // CCCDs are always 16 bits
                return BLE_ERROR_PARAM_OUT_OF_RANGE;
            }
            *((uint16_t*)buffer) = AttsCccGet(connectionHandle, idx);
            *lengthP = 2;   // CCCDs are always 16 bits
            return BLE_ERROR_NONE;
        }
    }

    // This is not a CCCD. Use the non-connection specific update method.
    return read(attributeHandle, buffer, lengthP);
}

ble_error_t MaximGattServer::write_(GattAttribute::Handle_t attributeHandle, const uint8_t buffer[], uint16_t len, bool localOnly)
{
    uint16_t connectionHandle = MaximGap::getInstance().getConnectionHandle();

    if (AttsSetAttr(attributeHandle, len, (uint8_t*)buffer) != ATT_SUCCESS) {
        return BLE_ERROR_PARAM_OUT_OF_RANGE;
    }

    if (!localOnly) {
        if (connectionHandle != DM_CONN_ID_NONE) {

            // Check to see if this characteristic has a CCCD attribute
            uint8_t idx;
            for (idx = 0; idx < cccCnt; idx++) {
                if (attributeHandle == cccHandles[idx]) {
                    break;
                }
            }
            if (idx < cccCnt) {
                // This characteristic has a CCCD attribute. Handle notifications and indications.
                uint16_t cccEnabled = AttsCccEnabled(connectionHandle, idx);
                if (cccEnabled & ATT_CLIENT_CFG_NOTIFY) {
                    AttsHandleValueNtf(connectionHandle, attributeHandle, len, (uint8_t*)buffer);
                }
                if (cccEnabled & ATT_CLIENT_CFG_INDICATE) {
                    AttsHandleValueInd(connectionHandle, attributeHandle, len, (uint8_t*)buffer);
                }
            }
        }
    }

    return BLE_ERROR_NONE;
}

ble_error_t MaximGattServer::write_(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, const uint8_t buffer[], uint16_t len, bool localOnly)
{
    // Check to see if this is a CCCD
    uint8_t idx;
    for (idx = 0; idx < cccCnt; idx++) {
        if (attributeHandle == cccSet[idx].handle) {
            if ((connectionHandle == DM_CONN_ID_NONE) || (len != 2)) { // CCCDs are always 16 bits
                return BLE_ERROR_PARAM_OUT_OF_RANGE;
            }
            AttsCccSet(connectionHandle, idx, *((uint16_t*)buffer));
            return BLE_ERROR_NONE;
        }
    }

    // This is not a CCCD. Use the non-connection specific update method.
    return write(attributeHandle, buffer, len, localOnly);
}

ble_error_t MaximGattServer::areUpdatesEnabled_(const GattCharacteristic &characteristic, bool *enabledP)
{
    uint16_t connectionHandle = MaximGap::getInstance().getConnectionHandle();

    if (connectionHandle != DM_CONN_ID_NONE) {
        uint8_t idx;
        for (idx = 0; idx < cccCnt; idx++) {
            if (characteristic.getValueHandle() == cccHandles[idx]) {
                uint16_t cccValue = AttsCccGet(connectionHandle, idx);
                if (cccValue & ATT_CLIENT_CFG_NOTIFY) {
                    *enabledP = true;
                } else {
                    *enabledP = false;
                }
                return BLE_ERROR_NONE;
            }
        }
    }

    return BLE_ERROR_PARAM_OUT_OF_RANGE;
}

ble_error_t MaximGattServer::areUpdatesEnabled_(Gap::Handle_t connectionHandle, const GattCharacteristic &characteristic, bool *enabledP)
{
    if (connectionHandle != DM_CONN_ID_NONE) {
        uint8_t idx;
        for (idx = 0; idx < cccCnt; idx++) {
            if (characteristic.getValueHandle() == cccHandles[idx]) {
                uint16_t cccValue = AttsCccGet(connectionHandle, idx);
                if (cccValue & ATT_CLIENT_CFG_NOTIFY) {
                    *enabledP = true;
                } else {
                    *enabledP = false;
                }
                return BLE_ERROR_NONE;
            }
        }
    }

    return BLE_ERROR_PARAM_OUT_OF_RANGE;
}

void MaximGattServer::cccCback(attsCccEvt_t *pEvt)
{
    if (pEvt->value & (ATT_CLIENT_CFG_NOTIFY | ATT_CLIENT_CFG_INDICATE)) {
        getInstance().handleEvent(GattServerEvents::GATT_EVENT_UPDATES_ENABLED, pEvt->handle);
    } else {
        getInstance().handleEvent(GattServerEvents::GATT_EVENT_UPDATES_DISABLED, pEvt->handle);
    }
}

void MaximGattServer::attCback(attEvt_t *pEvt)
{
    if (pEvt->hdr.status == ATT_SUCCESS) {
        getInstance().handleEvent(GattServerEvents::GATT_EVENT_DATA_SENT, pEvt->handle);
    }
}

uint8_t MaximGattServer::attsReadCback(dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset, attsAttr_t *pAttr)
{
    GattReadCallbackParams cbParams = {
        /* .connHandle = */ connId,
        /* .handle     = */ handle,
        /* .offset     = */ offset,
        /* .len        = */ *pAttr->pLen,
        /* .data       = */ pAttr->pValue
    };
    getInstance().handleDataReadEvent(&cbParams);

    return ATT_SUCCESS;
}

uint8_t MaximGattServer::attsWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset, uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
    uint8_t err;

    /* TODO: offset is not handled properly */
    if ((err = AttsSetAttr(handle, len, pValue)) != ATT_SUCCESS) {
        return err;
    }

    GattWriteCallbackParams::WriteOp_t writeOp;
    switch (operation) {
        case ATT_PDU_WRITE_REQ:
            writeOp = GattWriteCallbackParams::OP_WRITE_REQ;
            break;
        case ATT_PDU_WRITE_CMD:
            writeOp = GattWriteCallbackParams::OP_WRITE_CMD;
            break;
        case ATT_PDU_SIGNED_WRITE_CMD:
            writeOp = GattWriteCallbackParams::OP_SIGN_WRITE_CMD;
            break;
        case ATT_PDU_PREP_WRITE_REQ:
            writeOp = GattWriteCallbackParams::OP_PREP_WRITE_REQ;
            break;
        case ATT_PDU_EXEC_WRITE_REQ:
            writeOp = GattWriteCallbackParams::OP_EXEC_WRITE_REQ_NOW;
            break;
        default:
            writeOp = GattWriteCallbackParams::OP_INVALID;
            break;
    }

    GattWriteCallbackParams cbParams = {
        /* .connHandle = */ connId,
        /* .handle     = */ handle,
        /* .writeOp    = */ writeOp,
        /* .offset     = */ offset,
        /* .len        = */ len,
        /* .data       = */ pValue
    };
    getInstance().handleDataWrittenEvent(&cbParams);

    return ATT_SUCCESS;
}

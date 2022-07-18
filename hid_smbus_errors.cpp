#include "hid_smbus_errors.h"

HID_SMBUS_Errors::HID_SMBUS_Errors()
{
    m_status    = HID_SMBUS_STATUS_CLEAR;
    m_status_0  = HID_SMBUS_S0_CLEAR;
    m_str_status = "";
}

QString & HID_SMBUS_Errors::getStatus()
{
    m_str_status = "";

    switch (m_status) {
    case HID_SMBUS_SUCCESS:
        m_str_status = "HID_SMBUS_SUCCESS";
        break;
    case HID_SMBUS_DEVICE_NOT_FOUND:
        m_str_status = "HID_SMBUS_DEVICE_NOT_FOUND";
        break;
    case HID_SMBUS_INVALID_HANDLE:
        m_str_status = "HID_SMBUS_INVALID_HANDLE";
        break;
    case HID_SMBUS_INVALID_DEVICE_OBJECT:
        m_str_status = "HID_SMBUS_INVALID_DEVICE_OBJECT";
        break;
    case HID_SMBUS_INVALID_PARAMETER:
        m_str_status = "HID_SMBUS_INVALID_PARAMETER";
        break;
    case HID_SMBUS_INVALID_REQUEST_LENGTH:
        m_str_status = "HID_SMBUS_INVALID_REQUEST_LENGTH";
        break;
    case HID_SMBUS_READ_ERROR:
        m_str_status = "HID_SMBUS_READ_ERROR";
        break;
    case HID_SMBUS_WRITE_ERROR:
        m_str_status = "HID_SMBUS_WRITE_ERROR";
        break;
    case HID_SMBUS_READ_TIMED_OUT:
        m_str_status = "HID_SMBUS_READ_TIMED_OUT";
        break;
    case HID_SMBUS_WRITE_TIMED_OUT:
        m_str_status = "HID_SMBUS_WRITE_TIMED_OUT";
        break;
    case HID_SMBUS_DEVICE_IO_FAILED:
        m_str_status = "HID_SMBUS_DEVICE_IO_FAILED";
        break;
    case HID_SMBUS_DEVICE_ACCESS_ERROR:
        m_str_status = "HID_SMBUS_DEVICE_ACCESS_ERROR";
        break;
    case HID_SMBUS_DEVICE_NOT_SUPPORTED:
        m_str_status = "HID_SMBUS_DEVICE_NOT_SUPPORTED";
        break;
    case HID_SMBUS_UNKNOWN_ERROR:
        m_str_status = "HID_SMBUS_UNKNOWN_ERROR";
        break;
    }
    return m_str_status;
}

QString & HID_SMBUS_Errors::getStatus_0()
{
    m_str_status = "";

    switch(m_status_0){
    case HID_SMBUS_S0_IDLE:
        m_str_status_0 = "HID_SMBUS_S0_IDLE";
        break;
    case HID_SMBUS_S0_BUSY:
        m_str_status_0 = "HID_SMBUS_S0_BUSY";
        break;
    case HID_SMBUS_S0_COMPLETE:
        m_str_status_0 = "HID_SMBUS_S0_COMPLETE";
        break;
    case HID_SMBUS_S0_ERROR:
        m_str_status_0 = "HID_SMBUS_S0_ERROR";
        break;
    }
    return m_str_status_0;
}

QString & HID_SMBUS_Errors::getStatus_1()
{
    m_str_status_1 = "";

    if(m_status_0 == HID_SMBUS_S0_BUSY){
        switch(m_status_1){
        case HID_SMBUS_S1_BUSY_ADDRESS_ACKED:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_ADDRESS_ACKED";
            break;
        case HID_SMBUS_S1_BUSY_ADDRESS_NACKED:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_ADDRESS_NACKED";
            break;
        case HID_SMBUS_S1_BUSY_READING:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_READING";
            break;
        case HID_SMBUS_S1_BUSY_WRITING:
            m_str_status_1 = "HID_SMBUS_S1_BUSY_WRITING";
            break;
        }
    }else if(m_status_0 == HID_SMBUS_S0_ERROR){
        switch(m_status_1){
        case HID_SMBUS_S1_ERROR_TIMEOUT_NACK:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_TIMEOUT_NACK";
            break;
        case HID_SMBUS_S1_ERROR_TIMEOUT_BUS_NOT_FREE:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_TIMEOUT_BUS_NOT_FREE";
            break;
        case HID_SMBUS_S1_ERROR_ARB_LOST:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_ARB_LOST";
            break;
        case HID_SMBUS_S1_ERROR_READ_INCOMPLETE:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_READ_INCOMPLETE";
            break;
        case HID_SMBUS_S1_ERROR_WRITE_INCOMPLETE:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_WRITE_INCOMPLETE";
            break;
        case HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY:
            m_str_status_1 = "HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY";
            break;
        }
    }
    return m_str_status_1;
}

void HID_SMBUS_Errors::setStatus(HID_SMBUS_STATUS status)
{
    if (status != m_status) {
        m_status = status;
        QString str = getStatus();
        emit status_Changed(str);
    }
}

void HID_SMBUS_Errors::setStatus_0(HID_SMBUS_S0 status_0)
{
    if (status_0 != m_status_0) {
        m_status_0 = status_0;
        QString str = getStatus_0();
        emit status_0_Changed(str);
    }
}

void HID_SMBUS_Errors::setStatus_1(HID_SMBUS_S1 status_1)
{
    if (status_1 != m_status_1) {
        m_status_1 = status_1;
        QString str = getStatus_1();
        emit status_1_Changed(str);
    }
}


PRIMARY_SERVICE(service_device_information, gBleSig_DeviceInformationService_d)
    CHARACTERISTIC(char_manufacturer_name_string, gBleSig_ManufacturerNameString_d, (gGattCharPropRead_c))
        VALUE_VARLEN(value_manufacturer_name_string, gBleSig_ManufacturerNameString_d, (gPermissionFlagReadable_c), 23, 3, "NXP")
    CHARACTERISTIC(char_model_number_string, gBleSig_ModelNumberString_d, (gGattCharPropRead_c))
        VALUE_VARLEN(value_model_number_string, gBleSig_ModelNumberString_d, (gPermissionFlagReadable_c), 23, 3, "NXP")
    CHARACTERISTIC(char_serial_number_string, gBleSig_SerialNumberString_d, (gGattCharPropRead_c))
        VALUE_VARLEN(value_serial_number_string, gBleSig_SerialNumberString_d, (gPermissionFlagReadable_c), 23, 3, "NXP")
    CHARACTERISTIC(char_hardware_revision_string, gBleSig_HardwareRevisionString_d, (gGattCharPropRead_c))
        VALUE_VARLEN(value_hardware_revision_string, gBleSig_HardwareRevisionString_d, (gPermissionFlagReadable_c), 23, 3, "NXP")
    CHARACTERISTIC(char_firmware_revision_string, gBleSig_FirmwareRevisionString_d, (gGattCharPropRead_c))
        VALUE_VARLEN(value_firmware_revision_string, gBleSig_FirmwareRevisionString_d, (gPermissionFlagReadable_c), 23, 3, "NXP")
    CHARACTERISTIC(char_software_revision_string, gBleSig_SoftwareRevisionString_d, (gGattCharPropRead_c))
        VALUE_VARLEN(value_software_revision_string, gBleSig_SoftwareRevisionString_d, (gPermissionFlagReadable_c), 23, 3, "NXP")
    CHARACTERISTIC(char_system_id, gBleSig_SystemId_d, (gGattCharPropRead_c))
        VALUE(value_system_id, gBleSig_SystemId_d, (gPermissionFlagReadable_c), 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
    CHARACTERISTIC(char_ieee_1107320601_regulatory_certification_data_list, gBleSig_IeeeRcdl_d, (gGattCharPropRead_c))
        VALUE_VARLEN(value_ieee_1107320601_regulatory_certification_data_list, gBleSig_IeeeRcdl_d, (gPermissionFlagReadable_c), 23, 3, "NXP")
    CHARACTERISTIC(char_pnp_id, gBleSig_PnpId_d, (gGattCharPropRead_c))
        VALUE(value_pnp_id, gBleSig_PnpId_d, (gPermissionFlagReadable_c), 7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)

PRIMARY_SERVICE(service_battery, gBleSig_BatteryService_d)
    CHARACTERISTIC(char_battery_level, gBleSig_BatteryLevel_d, (gGattCharPropNotify_c | gGattCharPropRead_c))
        VALUE(value_battery_level, gBleSig_BatteryLevel_d, (gPermissionFlagReadable_c), 1, 0x00)
        DESCRIPTOR(descriptor_battery_level, gBleSig_CharPresFormatDescriptor_d, (gPermissionFlagReadable_c), 7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
        CCCD(cccd_battery_level)

PRIMARY_SERVICE(service_generic_attribute, gBleSig_GenericAttributeProfile_d)
    CHARACTERISTIC(char_service_changed, gBleSig_GattServiceChanged_d, (gGattCharPropIndicate_c))
        VALUE(value_service_changed, gBleSig_GattServiceChanged_d, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
        CCCD(cccd_service_changed)

PRIMARY_SERVICE(service_generic_access, gBleSig_GenericAccessProfile_d)
	CHARACTERISTIC(char_device_name, gBleSig_GapDeviceName_d, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_VARLEN(value_device_name, gBleSig_GapDeviceName_d, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 23, 17, "SmartCantonDevBox")
	CHARACTERISTIC(char_appearance, gBleSig_GapAppearance_d, (gGattCharPropRead_c))
		VALUE(value_appearance, gBleSig_GapAppearance_d, (gPermissionFlagReadable_c), 2, UuidArray(gUnknown_c))
//	CHARACTERISTIC(char_peripheral_privacy_flag, gBleSig_GapPeripheralPrivacyFlag_d, (gGattCharPropRead_c))
//		VALUE(value_peripheral_privacy_flag, gBleSig_GapPeripheralPrivacyFlag_d, (gPermissionFlagReadable_c), 1, 0x00)
//	CHARACTERISTIC(char_reconnection_address, gBleSig_GapReconnectionAddress_d, (gGattCharPropWrite_c))
//		VALUE(value_reconnection_address, gBleSig_GapReconnectionAddress_d, (gPermissionFlagWritable_c), 6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
	CHARACTERISTIC(char_peripheral_preferred_connection_parameters, gBleSig_GapPpcp_d, (gGattCharPropRead_c))
		VALUE(value_peripheral_preferred_connection_parameters, gBleSig_GapPpcp_d, (gPermissionFlagReadable_c), 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)


PRIMARY_SERVICE_UUID128(service_smartcanton_devbox_lora, uuid_service_smartcanton_devbox_lora)
	CHARACTERISTIC_UUID128(char_lora_app_eui, uuid_lora_app_eui, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_UUID128_VARLEN(value_lora_app_eui, uuid_lora_app_eui, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 8, 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_lora_app_eui, 0x2901, (gPermissionFlagReadable_c), 24, "LoRaWAN Application EUI")
	CHARACTERISTIC_UUID128(char_lora_app_key, uuid_lora_app_key, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_UUID128_VARLEN(value_lora_app_key, uuid_lora_app_key, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 16, 16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_lora_app_key, 0x2901, (gPermissionFlagReadable_c), 24, "LoRaWAN Application Key")
	CHARACTERISTIC_UUID128(char_lora_device_eui, uuid_lora_device_eui, (gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_lora_device_eui, uuid_lora_device_eui, (gPermissionFlagReadable_c), 8, 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_lora_device_eui, 0x2901, (gPermissionFlagReadable_c), 19, "LoRaWAN Device EUI")
	CHARACTERISTIC_UUID128(char_lora_confirm_mode, uuid_lora_confirm_mode, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_UUID128(value_lora_confirm_mode, uuid_lora_confirm_mode, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 1, 0x00)
		DESCRIPTOR(descriptor_lora_confirm_mode, 0x2901, (gPermissionFlagReadable_c), 26, "LoRaWAN Confirmation Mode")
	CHARACTERISTIC_UUID128(char_lora_network_join_status, uuid_lora_network_join_status, (gGattCharPropRead_c))
		VALUE_UUID128(value_lora_network_join_status, uuid_lora_network_join_status, (gPermissionFlagReadable_c), 1, 0x00)
		DESCRIPTOR(descriptor_lora_network_join_status, 0x2901, (gPermissionFlagReadable_c), 28, "LoRaWAN Network Join Status")
	CHARACTERISTIC_UUID128(char_lora_device_address, uuid_lora_device_address, (gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_lora_device_address, uuid_lora_device_address, (gPermissionFlagReadable_c), 4, 4, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_lora_device_address, 0x2901, (gPermissionFlagReadable_c), 23, "LoRaWAN Device Address")
	CHARACTERISTIC_UUID128(char_lora_network_session_key, uuid_lora_network_session_key, (gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_lora_network_session_key, uuid_lora_network_session_key, (gPermissionFlagReadable_c), 16, 16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_lora_network_session_key, 0x2901, (gPermissionFlagReadable_c), 28, "LoRaWAN Network Session Key")
	CHARACTERISTIC_UUID128(char_lora_app_session_key, uuid_lora_app_session_key, (gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_lora_app_session_key, uuid_lora_app_session_key, (gPermissionFlagReadable_c), 16, 16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_lora_app_session_key, 0x2901, (gPermissionFlagReadable_c), 24, "LoRaWAN App Session Key")
	CHARACTERISTIC_UUID128(char_lora_validate_new_configuration, uuid_lora_validate_new_configuration, (gGattCharPropWrite_c))
		VALUE_UUID128(value_lora_validate_new_configuration, uuid_lora_validate_new_configuration, (gPermissionFlagWritable_c), 1, 0x00)
		DESCRIPTOR(descriptor_lora_validate_new_configuration, 0x2901, (gPermissionFlagReadable_c), 35, "LoRaWAN Validate New Configuration")

PRIMARY_SERVICE_UUID128(service_smartcanton_devbox_gps, uuid_service_smartcanton_devbox_gps)
	CHARACTERISTIC_UUID128(char_gps_position, uuid_gps_position, (gGattCharPropNotify_c | gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_gps_position, uuid_gps_position, (gPermissionFlagReadable_c), 23, 0, "")
		CCCD(cccd_gps_position)
		DESCRIPTOR(descriptor_gps_position, 0x2901, (gPermissionFlagReadable_c), 12, "GPS Position")
	CHARACTERISTIC_UUID128(char_gps_speed, uuid_gps_speed, (gGattCharPropNotify_c |gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_gps_speed, uuid_gps_speed, (gPermissionFlagReadable_c), 23, 0, "")
		DESCRIPTOR(descriptor_gps_speed, 0x2901, (gPermissionFlagReadable_c), 9, "GPS Speed")
		CCCD(cccd_gps_speed)
	CHARACTERISTIC_UUID128(char_gps_course, uuid_gps_course, (gGattCharPropNotify_c |gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_gps_course, uuid_gps_course, (gPermissionFlagReadable_c), 23, 0, "")
		DESCRIPTOR(descriptor_gps_course, 0x2901, (gPermissionFlagReadable_c), 10, "GPS Course")
		CCCD(cccd_gps_course)
	CHARACTERISTIC_UUID128(char_gps_date, uuid_gps_date, (gGattCharPropNotify_c |gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_gps_date, uuid_gps_date, (gPermissionFlagReadable_c), 23, 0, "")
		DESCRIPTOR(descriptor_gps_date, 0x2901, (gPermissionFlagReadable_c), 8, "GPS Date")
		CCCD(cccd_gps_date)
	CHARACTERISTIC_UUID128(char_gps_time, uuid_gps_time, (gGattCharPropNotify_c |gGattCharPropRead_c))
		VALUE_UUID128_VARLEN(value_gps_time, uuid_gps_time, (gPermissionFlagReadable_c), 23, 0, "")
		DESCRIPTOR(descriptor_gps_time, 0x2901, (gPermissionFlagReadable_c), 8, "GPS Time")
		CCCD(cccd_gps_time)


PRIMARY_SERVICE_UUID128(service_smartcanton_devbox_bme680, uuid_service_smartcanton_devbox_bme680)
	CHARACTERISTIC_UUID128(char_bme680_iaq, uuid_bme680_iaq, (gGattCharPropNotify_c))
		VALUE_UUID128(value_bme680_iaq, uuid_bme680_iaq, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bme680_iaq)
		DESCRIPTOR(descriptor_bme680_iaq, 0x2901, (gPermissionFlagReadable_c), 11, "BME680 IAQ")
	CHARACTERISTIC_UUID128(char_bme680_iaq_accuracy, uuid_bme680_iaq_accuracy, (gGattCharPropNotify_c))
		VALUE_UUID128(value_bme680_iaq_accuracy, uuid_bme680_iaq_accuracy, (gPermissionNone_c), 1, 0x00)
		CCCD(cccd_bme680_iaq_accuracy)
		DESCRIPTOR(descriptor_bme680_iaq_accuracy, 0x2901, (gPermissionFlagReadable_c), 15, "BME680 Accuracy")
	CHARACTERISTIC_UUID128(char_bme680_temperature, uuid_bme680_temperature, (gGattCharPropNotify_c))
		VALUE_UUID128(value_bme680_temperature, uuid_bme680_temperature, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bme680_temperature)
		DESCRIPTOR(descriptor_bme680_temperature, 0x2901, (gPermissionFlagReadable_c), 18, "BME680 Temperature")
	CHARACTERISTIC_UUID128(char_bme680_humidity, uuid_bme680_humidity, (gGattCharPropNotify_c))
		VALUE_UUID128(value_bme680_humidity, uuid_bme680_humidity, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bme680_humidity)
		DESCRIPTOR(descriptor_bme680_humidity, 0x2901, (gPermissionFlagReadable_c), 15, "BME680 Humidity")
	CHARACTERISTIC_UUID128(char_bme680_pressure, uuid_bme680_pressure, (gGattCharPropNotify_c))
		VALUE_UUID128(value_bme680_pressure, uuid_bme680_pressure, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bme680_pressure)
		DESCRIPTOR(descriptor_bme680_pressure, 0x2901, (gPermissionFlagReadable_c), 15, "BME680 Pressure")
	CHARACTERISTIC_UUID128(char_bme680_raw_temperature, uuid_bme680_raw_temperature, (gGattCharPropRead_c))
		VALUE_UUID128(value_bme680_raw_temperature, uuid_bme680_raw_temperature, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_bme680_raw_temperature, 0x2901, (gPermissionFlagReadable_c), 22, "BME680 Raw Temperature")
	CHARACTERISTIC_UUID128(char_bme680_raw_humidity, uuid_bme680_raw_humidity, (gGattCharPropRead_c))
		VALUE_UUID128(value_bme680_raw_humidity, uuid_bme680_raw_humidity, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bme680_raw_humidity)
		DESCRIPTOR(descriptor_bme680_raw_humidity, 0x2901, (gPermissionFlagReadable_c), 19, "BME680 Raw Humidity")
	CHARACTERISTIC_UUID128(char_bme680_raw_gas, uuid_bme680_raw_gas, (gGattCharPropNotify_c))
		VALUE_UUID128(value_bme680_raw_gas, uuid_bme680_raw_gas, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bme680_raw_gas)
		DESCRIPTOR(descriptor_bme680_raw_gas, 0x2901, (gPermissionFlagReadable_c), 14, "BME680 Raw Gas")

PRIMARY_SERVICE_UUID128(service_smartcanton_devbox_bno055, uuid_service_smartcanton_devbox_bno055)
	CHARACTERISTIC_UUID128(char_bno055_accelerometer, uuid_bno055_accelerometer, (gGattCharPropNotify_c))
		VALUE_UUID128_VARLEN(value_bno055_accelerometer, uuid_bno055_accelerometer, (gPermissionNone_c), 23, 23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bno055_accelerometer)
		DESCRIPTOR(descriptor_bno055_accelerometer, 0x2901, (gPermissionFlagReadable_c), 24, "BNO055 Accelerometer xyz")
	CHARACTERISTIC_UUID128(char_bno055_gyroscope, uuid_bno055_gyroscope, (gGattCharPropNotify_c))
		VALUE_UUID128_VARLEN(value_bno055_gyroscope, uuid_bno055_gyroscope, (gPermissionNone_c), 23, 23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bno055_gyroscope)
		DESCRIPTOR(descriptor_bno055_gyroscope, 0x2901, (gPermissionFlagReadable_c), 20, "BNO055 Gyroscope xyz")
	CHARACTERISTIC_UUID128(char_bno055_magnetometer, uuid_bno055_magnetometer, (gGattCharPropNotify_c))
		VALUE_UUID128_VARLEN(value_bno055_magnetometer, uuid_bno055_magnetometer, (gPermissionNone_c), 23, 23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
		CCCD(cccd_bno055_magnetometer)
		DESCRIPTOR(descriptor_bno055_magnetometer, 0x2901, (gPermissionFlagReadable_c), 23, "BNO055 Magnetometer xyz")
	CHARACTERISTIC_UUID128(char_bno055_measure_delay, uuid_bno055_measure_delay, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_UUID128(value_bno055_measure_delay, uuid_bno055_measure_delay, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 4, 0x00, 0x00, 0x00, 0x00)
		DESCRIPTOR(descriptor_bno055_measure_delay, 0x2901, (gPermissionFlagReadable_c), 17, "BNO Measure Delay")

PRIMARY_SERVICE_UUID128(service_smartcanton_devbox_ble, uuid_service_smartcanton_devbox_ble)
	CHARACTERISTIC_UUID128(char_ble_devices_scanned, uuid_ble_devices_scanned, (gGattCharPropNotify_c | gGattCharPropRead_c))
		VALUE_UUID128(value_ble_devices_scanned, uuid_ble_devices_scanned, (gPermissionFlagReadable_c), 2, 0x00, 0x00)
		CCCD(cccd_ble_devices_scanned)
		DESCRIPTOR(descriptor_ble_devices_scanned, 0x2901, (gPermissionFlagReadable_c), 19, "BLE Devices Scanned")
	CHARACTERISTIC_UUID128(char_ble_scan_window, uuid_ble_scan_window, (gGattCharPropRead_c | gGattCharPropWrite_c))
		VALUE_UUID128(value_ble_scan_window, uuid_ble_scan_window, (gPermissionFlagReadable_c | gPermissionFlagWritable_c), 2, 0x00, 0x00)
		DESCRIPTOR(descriptor_ble_scan_window, 0x2901, (gPermissionFlagReadable_c), 15, "BLE Window Scan")

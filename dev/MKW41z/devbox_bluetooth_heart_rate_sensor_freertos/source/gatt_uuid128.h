/* 
* Declare all custom 128-bit UUIDs here using the format:
*
*  UUID128(name, bytes)
*
* where: 
*	-name : an unique tag for the newly defined UUID;
		will be used to reference this UUID when defining
		services and characteristics in <<gattDb.h>>
*	-bytes: 16 bytes representing the 128-bit value
*
* One definition per line. No semicolon required after each definition.
*
* example:
*  UUID128(uuid_service_robot_characteristics, 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF)
*  UUID128(uuid_char_robot_direction, 0x12, 0x34, 0x50, 0x00, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF)
*	
*/

/* Potentiometer Service */
UUID128(uuid_service_potentiometer, 0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x04, 0x56, 0xFF, 0x02)

/* Potentiometer Characteristic */
UUID128(uuid_characteristic_potentiometer_relative_value, 0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x04, 0x57, 0xFF, 0x02)

/* SmartCanton DevBox Service */
UUID128(uuid_service_smartcanton_devbox, 0x02, 0xFF, 0x56, 0x04, 0xBA, 0x5E, 0xF4, 0xEE, 0x5C, 0xA1, 0xEB, 0x1E, 0x5E, 0x4B, 0x1C, 0xE0)

/* LoRa Characteristics */
UUID128(uuid_lora_app_eui, 0x02, 0xFF, 0x57, 0x04, 0xBA, 0x5E, 0xF4, 0xEE, 0x5C, 0xA1, 0xEB, 0x1E, 0x5E, 0x4B, 0x1C, 0xE0)
UUID128(uuid_lora_app_key, 0x02, 0xFF, 0x58, 0x04, 0xBA, 0x5E, 0xF4, 0xEE, 0x5C, 0xA1, 0xEB, 0x1E, 0x5E, 0x4B, 0x1C, 0xE0)
UUID128(uuid_lora_device_eui, 0xDF, 0x90, 0x37, 0x93, 0x4A, 0x83, 0x4C, 0x1F, 0xBB, 0x33, 0xB6, 0x23, 0xB7, 0x34, 0x76, 0x3F)
UUID128(uuid_lora_confirm_mode, 0xDF, 0x90, 0xB4, 0x12, 0x4A, 0x83, 0x4C, 0x1F, 0xBB, 0x33, 0xB6, 0x23, 0xB7, 0x34, 0x76, 0x3F)
UUID128(uuid_lora_network_join_status, 0xDF, 0x90, 0x03, 0x3E, 0x4A, 0x83, 0x4C, 0x1F, 0xBB, 0x33, 0xB6, 0x23, 0xB7, 0x34, 0x76, 0x3F)
UUID128(uuid_lora_device_address, 0xDF, 0x90, 0xC3, 0x8E, 0x4A, 0x83, 0x4C, 0x1F, 0xBB, 0x33, 0xB6, 0x23, 0xB7, 0x34, 0x76, 0x3F)
UUID128(uuid_lora_network_session_key, 0xDF, 0x90, 0x67, 0x3A, 0x4A, 0x83, 0x4C, 0x1F, 0xBB, 0x33, 0xB6, 0x23, 0xB7, 0x34, 0x76, 0x3F)
UUID128(uuid_lora_app_session_key, 0xDF, 0x90, 0x15, 0x62, 0x4A, 0x83, 0x4C, 0x1F, 0xBB, 0x33, 0xB6, 0x23, 0xB7, 0x34, 0x76, 0x3F)
UUID128(uuid_lora_validate_new_configuration, 0xDF, 0x90, 0x5D, 0x0A, 0x4A, 0x83, 0x4C, 0x1F, 0xBB, 0x33, 0xB6, 0x23, 0xB7, 0x34, 0x76, 0x3F)

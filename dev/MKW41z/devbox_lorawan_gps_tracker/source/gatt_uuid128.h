/*
 * Declare all custom 128-bit UUIDs here using the format:
 *
 *  UUID128(name, bytes)
 *
 * where:
 *   -name : an unique tag for the newly defined UUID;
 will be used to reference this UUID when defining
 services and characteristics in <<gattDb.h>>
 *   -bytes: 16 bytes representing the 128-bit value
 *
 * One definition per line. No semicolon required after each definition.
 *
 * example:
 *  UUID128(uuid_service_robot_characteristics, 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF)
 *  UUID128(uuid_char_robot_direction, 0x12, 0x34, 0x50, 0x00, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF)
 *
 */
UUID128(uuid_service_smartcanton_devbox_lora, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_app_eui, 0x00, 0x00, 0x00, 0x01, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_app_key, 0x00, 0x00, 0x00, 0x02, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_device_eui, 0x00, 0x00, 0x00, 0x03, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_confirm_mode, 0x00, 0x00, 0x00, 0x04, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_network_join_status, 0x00, 0x00, 0x00, 0x05, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_device_address, 0x00, 0x00, 0x00, 0x06, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_network_session_key, 0x00, 0x00, 0x00, 0x07, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_app_session_key, 0x00, 0x00, 0x00, 0x08, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_lora_validate_new_configuration, 0x00, 0x00, 0x00, 0x09, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)

UUID128(uuid_service_smartcanton_devbox_gps, 0x10, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_gps_position, 0x10, 0x00, 0x00, 0x01, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_gps_speed, 0x10, 0x00, 0x00, 0x02, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_gps_course, 0x10, 0x00, 0x00, 0x03, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_gps_date, 0x10, 0x00, 0x00, 0x04, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)
UUID128(uuid_gps_time, 0x10, 0x00, 0x00, 0x05, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD)


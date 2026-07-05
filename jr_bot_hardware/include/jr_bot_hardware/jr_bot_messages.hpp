/**
 * @file jr_bot_messages.h
 * @brief Contains definitions for various message IDs used in the educational AMR platform.
 */

#ifndef JR_BOT_MESSAGES_H_
#define JR_BOT_MESSAGES_H_

enum ConfigFrameIDs {
	PROTOCOL_SWITCH = 0x00,
	IMU_USAGE = 0x01,
	IMU_FREQUENCY = 0x02,
	MOTOR_REVERSE_DIRECTIONS = 0x03,
	TOF_SWEEP_USAGE = 0x04,
};

enum RequestFrameIDs {
	REQUEST_IMU_GYRO_RAW = 0x21,
	REQUEST_IMU_ACCEL_RAW = 0x22,
	REQUEST_TOF_STATE = 0x23,
	REQUEST_IR_STATES = 0x24,
};

enum ResponseFrameIDs {
	RESPONSE_IMU_GYRO_RAW = 0x41,
	RESPONSE_IMU_ACCEL_RAW = 0x42,
	RESPONSE_TOF_STATE = 0x43,
	RESPONSE_IR_STATES = 0x44,
};

enum CommandsFrameIDs {
	MOTOR_DESIRED_PWM = 0x60,
	USER_DEFINED_LED = 0x61
};

enum EmergencyFrameIDs {
	EMERGENCY_STOP = 0x81,
};

enum SerializedDataSizes {
	SERIALIZED_IMU_GYRO_RAW_BYTES = 12,    	// 3 * 4 bytes (float32)
	SERIALIZED_IMU_ACCEL_RAW_BYTES = 12,   	// 3 * 4 bytes (float32)
	SERIALIZED_TOF_STATE_BYTES = 4,        	// 2 * 2 bytes (uint16_t dist, uint16_t angle)
	SERIALIZED_IR_STATES_BYTES = 2,        	// 2 * 1 byte (bool)
	SERIALIZED_MOTOR_DESIRED_PWM_BYTES = 4,	// 2 * 2 bytes (int16_t left, int16_t right)
	SERIALIZED_USER_DEFINED_LED_BYTES = 1, 	// 1 * 1 byte (bool)
};

enum UnserializedDataSizes {
	UNSERIALIZED_IMU_GYRO_RAW_SIZE = 3,
	UNSERIALIZED_IMU_ACCEL_RAW_SIZE = 3,
	UNSERIALIZED_TOF_STATE_SIZE = 2,
	UNSERIALIZED_IR_STATES_SIZE = 2,
	UNSERIALIZED_MOTOR_DESIRED_PWM_SIZE = 2,
	UNSERIALIZED_USER_DEFINED_LED_SIZE = 1,
};


#endif // JR_BOT_MESSAGES_H_

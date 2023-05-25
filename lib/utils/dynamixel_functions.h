#ifndef DYNAMIXEL_FUNCTIONS_H
#define DYNAMIXEL_FUNCTIONS_H

#include <stdio.h>
#include "../lib/dynamixel/dynamixel2pico/Dynamixel2Pico.h"

//Please see eManual Control Table section of your DYNAMIXEL.
//This example is written for DYNAMIXEL X series(excluding XL-320)
#define OPERATING_MODE_ADDR         11
#define OPERATING_MODE_ADDR_LEN     1
#define TORQUE_ENABLE_ADDR          64
#define TORQUE_ENABLE_ADDR_LEN      1
#define LED_ADDR                    65
#define LED_ADDR_LEN                1
#define GOAL_POSITION_ADDR          116
#define GOAL_POSITION_ADDR_LEN      4
#define PRESENT_POSITION_ADDR       132
#define PRESENT_POSITION_ADDR_LEN   4
#define POSITION_CONTROL_MODE       3

#define DEFAULT_TIMEOUT             50
#define DEFAULT_VELOCITY 100
const float DXL_PROTOCOL_VERSION = 2.0;

// User defined functions definitions -----------------------------
void change_protocol_type(Dynamixel2Pico dxl_num, uint8_t id, uint8_t protocol);

void moveTo(Dynamixel2Pico dxl_num, uint8_t ID, uint32_t position);

void moveToAngle(Dynamixel2Pico dxl_num, uint8_t ID, uint32_t angle);

void moveMotorToAngle(Dynamixel2Pico dxl_num, const uint8_t ID, uint32_t angle);

float getPosition(Dynamixel2Pico dxl_num, uint8_t id, uint8_t unit = (uint8_t)0U);

float getCurrentAngle(Dynamixel2Pico dxl_num, uint8_t id, uint8_t unit = (uint8_t)0U);

void dynamixelSetup(Dynamixel2Pico dxl_num, const uint8_t totalMotors);

float getCurrentVelocity(Dynamixel2Pico dxl_num, uint8_t id);

void setVelocity(Dynamixel2Pico dxl_num, uint8_t id, int32_t velocity);
void setAcceleration(Dynamixel2Pico dxl_num, uint8_t id, int32_t acceleration);
void setGoalCurrent(Dynamixel2Pico dxl_num, uint8_t id, int32_t current);

#endif
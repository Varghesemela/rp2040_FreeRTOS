#include <cstdio>
#include <math.h>
#include "dynamixel_functions.h"
#include "../lib/dynamixel/dynamixel2pico/actuator.h"

using namespace ControlTableItem;
#define BAUDRATE_ADDR           8
#define BAUDRATE_ADDR_LEN       1
#define PROTOCOL_TYPE_ADDR      13
#define PROTOCOL_TYPE_ADDR_LEN  1

void change_protocol_type(Dynamixel2Pico dxl_num, uint8_t id, uint8_t protocol){
    dxl_num.torqueOff(id);

    if(dxl_num.write(id, PROTOCOL_TYPE_ADDR, (uint8_t*)&protocol , PROTOCOL_TYPE_ADDR_LEN, DEFAULT_TIMEOUT)) {
        printf("Change Protocol type %" PRIu8 " successfull\n", protocol);
    }
    else{ printf("__Protocol type__ %" PRIu8 " failed\n", protocol); }
    dxl_num.torqueOff(id);

}
//----------------------------------------------------------------
void moveTo(Dynamixel2Pico dxl_num, uint8_t ID, uint32_t position) {
  bool ret = dxl_num.write(ID, GOAL_POSITION_ADDR, (uint8_t*)&position, GOAL_POSITION_ADDR_LEN, DEFAULT_TIMEOUT);
  if (!ret){
    printf("Error in moveto\n");
  }
}

void moveToAngle(Dynamixel2Pico dxl_num, uint8_t ID, uint32_t angle){
  dxl_num.setGoalPosition(ID, angle, UNIT_DEGREE);
}

void moveMotorToAngle(Dynamixel2Pico dxl_num, uint8_t ID, uint32_t angle){
  dxl_num.setGoalPosition(ID, angle, UNIT_DEGREE);
  printf("Motor ID: %d \t Position(angle): %d\n", ID, angle);
}

float getCurrentAngle(Dynamixel2Pico dxl_num, uint8_t id, uint8_t unit){
    float position = -1;
    position = dxl_num.getPresentPosition(id,unit);
    sleep_ms(100);
    printf("Present position(angle): %f\n",position);
    return position;
}

float getPosition(Dynamixel2Pico dxl_num, uint8_t id, uint8_t unit){
    float position = -1;
    position = dxl_num.getPresentPosition(id, unit);
    sleep_ms(100);
    printf("Present position: %f\n", position);
    return position;
}

float getCurrentVelocity(Dynamixel2Pico dxl_num, uint8_t id){
    float velocity = -1;
    velocity = dxl_num.getPresentVelocity(id, UNIT_RAW);
    sleep_ms(100);
    printf("Present velocity(RPM): %d\n", (int32_t)velocity);
    return velocity;
}

void setVelocity(Dynamixel2Pico dxl_num, uint8_t id, int32_t velocity){
    bool ret = dxl_num.writeControlTableItem(PROFILE_VELOCITY, id, velocity);
    sleep_ms(100);
    if(ret){
     printf("Successfully set goal velocity(RPM): %d\n", velocity);
    }
    else{
    printf("Error: Set goal velocity failed\n");
    }
}

void setAcceleration(Dynamixel2Pico dxl_num, uint8_t id, int32_t acceleration){
    bool ret = dxl_num.writeControlTableItem(PROFILE_ACCELERATION, id, acceleration);
    sleep_ms(100);
    if(ret){
     printf("Successfully set acceleration: %d\n", acceleration);
    }
    else{
    printf("Error: Set goal acceleration failed\n");
    }
}

void setGoalCurrent(Dynamixel2Pico dxl_num, uint8_t id, int32_t current){
    bool ret = dxl_num.writeControlTableItem(GOAL_CURRENT, id, current);
    sleep_ms(100);
    if(ret){
        printf("Successfully set current: %d\n", current);
    }
    else{
        printf("Error: Set goal current failed\n");
    }
}

void dynamixelSetup(Dynamixel2Pico dxl_num, uint8_t id){
  // Get DYNAMIXEL information 
    dxl_num.torqueOff(id);
    dxl_num.setOperatingMode(id, POSITION_CONTROL_MODE);
    dxl_num.setProtocol(id, DXL_PROTOCOL_VERSION);
    setVelocity(dxl_num, id, DEFAULT_VELOCITY);

    dxl_num.writeControlTableItem(POSITION_P_GAIN, id, 400);
    dxl_num.writeControlTableItem(POSITION_I_GAIN, id, 5);
    dxl_num.writeControlTableItem(POSITION_D_GAIN, id, 0);
    dxl_num.torqueOn(id);
} 
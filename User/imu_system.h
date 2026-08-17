#ifndef _IMU_SYSTEM_H_
#define _IMU_SYSTEM_H_

#include <stdint.h>

int  imu_system_init(void);
int  imu_system_run(void);
void imu_system_get_euler(float *roll, float *pitch, float *yaw);
void imu_system_get_gyro_z(float *gz);   /**< 获取最新陀螺仪Z轴角速度 (deg/s) */

#endif

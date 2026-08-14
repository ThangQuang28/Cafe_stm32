#ifndef __PID_H
#define __PID_H

#include <stdint.h>

/**
 * @brief  Khởi tạo các thông số cho PID Controller.
 * 
 *         Công thức: out = (Kp * Ek) + (Kp * (t/ti) * Sum(Ek)) + (Kp * (td/t) * (Ek - Ek-1)) + out0
 * @param pid: Pointer tới struct của PID
 * @param Kp: Khâu tỉ lệ
 * @param ti: Thời gian khâu tích phân
 * @param td: Thời gian khâu vi phân
 * @param t: Chu kỳ lấy mẫu
 * @param out0: Giá trị output ( VD: out0 khác 0 đến khi đạt giá trị của mục tiêu)
 * @param out_max: Giới hạn ouput cận trên (VD: 100.0 for 100% PWM)
 * @param out_min: Giới hạn output cận dưới (VD: 0.0)
 * @param sum_E: Tổng sai số ( thành phần tích phân )
 * @param last_E: Sai số cuối ( thành phân vi phân )
 * @param sum_E_max: Giới hạn tổng sai số cận trên
 * @param sum_E_min: Giới hạn tổng sai số cận dưới
 */
typedef struct {
    float Kp;       
    float ti;       
    float td;       
    float t;       
    float out0;     

    // Limits
    float out_max;  
    float out_min;  

    float sum_E;   
    float last_E;  

    float sum_E_max;
    float sum_E_min;
} PID_TypeDef;

void PID_Init(PID_TypeDef *pid, float Kp, float ti, float td, float t, float out0, float out_max, float out_min);

/**
 * @brief Tính PID dựa theo công thức trên
 * @param pid: Pointer tới struct PID
 * @param setpoint: Nhiệt độ mong muốn (T_0)
 * @param measured_value: Nhiệt độ hiện tại (T_1)
 * @retval Trả về tín hiệu output đã tính toán 
 */
float PID_Computer(PID_TypeDef *pid, float setpoint, float measured_value);

#endif /* __PID_H */

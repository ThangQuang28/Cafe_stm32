#include "pid.h"

void PID_Init(PID_TypeDef *pid, float Kp, float ti, float td, float t, float out0, float out_max, float out_min) {
    pid->Kp = Kp;
    pid->ti = ti;
    pid->td = td;
    pid->t = t;
    pid->out0 = out0;
    pid->out_max = out_max;
    pid->out_min = out_min;
    
    pid->sum_E = 0.0f;
    pid->last_E = 0.0f;

    pid->sum_E_max = out_max;
    pid->sum_E_min = -out_max;
}


float PID_Computer(PID_TypeDef *pid, float setpoint, float measured_value) {
    float Ek;        
    float P_term, I_term, D_term;
    float output;

    Ek = setpoint - measured_value; // Tính sai số hiện tại

    pid->sum_E += Ek; // Sai số tích lũy cho thành phần tích phân

    
    if (pid->sum_E > pid->sum_E_max) {
        pid->sum_E = pid->sum_E_max;
    } else if (pid->sum_E < pid->sum_E_min) {
        pid->sum_E = pid->sum_E_min;
    }

    // Khâu tỉ lệ: Kp * Ek 
    P_term = pid->Kp * Ek;

    // Khâu tích phân: Kp * (t / ti) * Sum(Ek)
    if (pid->ti != 0.0f) { 
        I_term = pid->Kp * (pid->t / pid->ti) * pid->sum_E;
    } else {
        I_term = 0.0f;
    }

    // Khâu vi phân: Kp * (td / t) * (Ek - Ek-1)
    if (pid->t != 0.0f) {
        D_term = pid->Kp * (pid->td / pid->t) * (Ek - pid->last_E);
    } else {
        D_term = 0.0f;
    }

    // PID
    output = P_term + I_term + D_term + pid->out0;

    // Lưu lại sai số hiện tại
    pid->last_E = Ek;

    // ngưỡng output của min/max limits
    if (output > pid->out_max) {
        output = pid->out_max;
    } else if (output < pid->out_min) {
        output = pid->out_min;
    }

    return output;
}

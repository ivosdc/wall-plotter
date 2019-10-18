#define SERVO_PIN D9
#define PEN_UP 50
#define PEN_DOWN 70
#define SPOOL_CIRC 94.2
#define STEPS_PER_ROTATION 4075.7728395
#define STEPS_PER_TICK 10
#define STEPS_PER_MM  (STEPS_PER_ROTATION / SPOOL_CIRC) / STEPS_PER_TICK
#define UPLOAD_PLOT_FILENAME "/wall-plotter.data"
#define WIFI_INIT_RETRY 20

#define MOTOR_LEFT_1 D5 // IN1
#define MOTOR_LEFT_2 D6 // IN2
#define MOTOR_LEFT_3 D7 // IN3
#define MOTOR_LEFT_4 D8 // IN4
#define MOTOR_RIGHT_1 D1
#define MOTOR_RIGHT_2 D2
#define MOTOR_RIGHT_3 D3
#define MOTOR_RIGHT_4 D4

/*
  =========================================================
   SELF-BALANCING ROBOT - ESP32 + MPU6050 + Dabble Gamepad
   (Ramped-lean drive: works WITH the balance PID, not against it)
  =========================================================
*/

#include <Wire.h>
#include <MPU6050_light.h>
#include <PID_v1.h>

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

MPU6050 mpu(Wire);

// ----------------- MOTOR DRIVER PINS -----------------
// Names match your confirmed physical wiring. Same GPIOs, same channel
// assignments as before - only the macro names changed, nothing electrical.
#define ENA 25   
#define IN1 26   
#define IN2 27   

#define ENB 33   
#define IN3 14   
#define IN4 32   

// ----------------- PWM SETUP -----------------
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8   
#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1

// ----------------- PID SETUP -----------------
double Kp = 9.5;
double Ki = 0.0;
double Kd = 0.0;

double pidInput;     
double pidOutput;    
double pidSetpoint;  

PID balancePID(&pidInput, &pidOutput, &pidSetpoint, Kp, Ki, Kd, DIRECT);

// ----------------- MOVEMENT TUNING -----------------
// Direct, held PWM push for forward/back - simple mental model: push the
// wheels, the body reacts by leaning, the balance PID fights to recenter
// it. Expect visible lean/recover cycles rather than a perfectly smooth
// glide - that's inherent to this approach, not a bug.
const int   MOVE_SPEED  = 90;    
const int   TURN_SPEED  = 100;   
const int   SPIN_SPEED  = 160;   
const float FALL_ANGLE  = 70.0;  

bool spinning = false;
bool manualDrive = false;
int moveBias = 0;

// ----------------- STICTION COMPENSATION -----------------
const int LEFT_MIN_PWM  = 60;   
const int RIGHT_MIN_PWM = 65;  

// =========================================================
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(LEFT_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENB, LEFT_CHANNEL);   // GPIO33 - same as before, correct name now

  ledcSetup(RIGHT_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, RIGHT_CHANNEL);  // GPIO25 - same as before, correct name now

  Wire.begin(21, 22);
  byte status = mpu.begin();
  while (status != 0) {
    Serial.println("Could not find MPU6050 - check wiring!");
    delay(1000);
  }

  Serial.println("Hold the robot upright... calibrating in 2 seconds.");
  delay(2000); 

  Serial.println("Calibrating now - keep it still and upright!");
  mpu.calcOffsets(true, true);
  Serial.println("Calibration done. Balancing started!");

  balancePID.SetMode(AUTOMATIC);        
  balancePID.SetOutputLimits(-255, 255); 
  balancePID.SetSampleTime(5);           

  Dabble.begin("BalanceBot");
}

// =========================================================
void loop() {
  Dabble.processInput();  

  mpu.update();            
  pidInput = mpu.getAngleX();
  
  handleControls();       

  if (fabs(pidInput) > FALL_ANGLE) {
    stopMotors();
    return;
  }

  if (manualDrive) {
    // Balance PID fully bypassed - wheels just get the raw push, robot
    // leans/rolls naturally like normal driving. Nothing fighting it.
    driveMotorsManual();
  } else {
    balancePID.Compute();    
    driveMotors((int)pidOutput);
  }
}

// =========================================================
void handleControls() {
  // If no phone is actually connected, GamePad reads can be stale/garbage -
  // ignore them completely and force everything to neutral.
  if (!Dabble.isAppConnected()) {
    manualDrive = false;
    moveBias = 0;
    spinning = false;
    pidSetpoint = 0;
    return;
  }

  spinning = false;
  pidSetpoint = 0; // Balance target - only used when NOT manually driving

  if (GamePad.isUpPressed()) {
    manualDrive = true;
    moveBias = MOVE_SPEED;
  } else if (GamePad.isDownPressed()) {
    manualDrive = true;
    moveBias = -MOVE_SPEED;
  } else {
    manualDrive = false;
    moveBias = 0;
  }

  if (GamePad.isCrossPressed()) {  
    spinning = true;
  }
}

// =========================================================
// Used ONLY while manually driving forward/back - no PID, no balance
// component, just a flat push on both wheels. The robot will lean
// naturally as a side effect; that's expected, not a bug.
void driveMotorsManual() {
  int leftSpeed  = constrain(moveBias, -255, 255);
  int rightSpeed = constrain(moveBias, -255, 255);

  setMotor(leftSpeed,  LEFT_CHANNEL,  IN4, IN3, LEFT_MIN_PWM);   // GPIO32/14 - same as before
  setMotor(rightSpeed, RIGHT_CHANNEL, IN1, IN2, RIGHT_MIN_PWM);  // GPIO26/27 - same as before
}

// =========================================================
void driveMotors(int balanceSpeed) {
  int leftSpeed  = balanceSpeed;
  int rightSpeed = balanceSpeed;

  if (spinning) {
    leftSpeed  += SPIN_SPEED;
    rightSpeed -= SPIN_SPEED;
  } else {
    // Direct button checks inside motor calculation eliminate release delay
    if (GamePad.isLeftPressed()) {
      rightSpeed += TURN_SPEED; // Turn left: push right wheel forward
    } 
    if (GamePad.isRightPressed()) {
      leftSpeed  += TURN_SPEED; // Turn right: push left wheel forward
    }
  }

  leftSpeed  = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  setMotor(leftSpeed,  LEFT_CHANNEL,  IN4, IN3, LEFT_MIN_PWM);   // GPIO32/14 - same as before
  setMotor(rightSpeed, RIGHT_CHANNEL, IN1, IN2, RIGHT_MIN_PWM);  // GPIO26/27 - same as before
}

// =========================================================
void setMotor(int speed, int channel, int in1, int in2, int minPwm) {
  int mag = abs(speed);

  if (mag == 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    ledcWrite(channel, 0);
    return;
  }

  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }

  // Additive floor: minPwm guarantees the wheel actually turns, then we
  // stack the real magnitude straight on top - no rescaling, so both
  // wheels behave predictably for the same input.
  int pwmVal = constrain(minPwm + mag, 0, 255);
  ledcWrite(channel, pwmVal);
}

void stopMotors() {
  ledcWrite(LEFT_CHANNEL, 0);
  ledcWrite(RIGHT_CHANNEL, 0);
}

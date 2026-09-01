<img width="600" height="1066" alt="gifrobot-ezgif com-optimize" src="https://github.com/user-attachments/assets/5aa2eb98-a5b1-418e-b028-2cf06f359c1f" />

This is a self-balancing robot project that is also driven by Bluetooth. IT uses an mpu6050 to measure tilt, an ESP32 as its brain to perform all the math and calculations for the PID, a L298 motor driver to control the wheels with the ESP32's commands, 2 DC motors with their gears and wheels and a 3s lipo battery 12v 2200mah (Which is a bit overkill for this project, i think u can use a smaller battery and it would work). I made the frame using popsicle sticks and a glue gun to stick them together. 

Now the mpu6050, on start, takes 5 seconds of calibration to detect where its 0 point is because if there are any curves or its placement isn't the best so it can measure correctly, meaning in the first 5 seconds you need to hold the robot straight in the point it should be at when balanced. Then the system works on its own by measuring any tilt and combating it. Make sure that both wheels spin in the same direction, and the direction that the robot leans, both wheels should drive towards it to catch the center of gravity and balance back. So if they spin against each other, it won't work, but if you find it that way, just switch the 2 wires feeding the motor power. In addition,n an important thing is the minimum PWM for each motor. Because cheap DC motors need different minimum PWM signals to start because of small changes in gears, starting points, or coils, it makes the min PWM different for each motor, so u should hold the robot in the air and tilt it a bit and see if one spins before the other. If so, then the one faster you should decrease its min PWM, and the one slower u should increase its min PWM. Keep tuning and refining it and tilt very slowly to actually find which starts first because if you lean too hard, both will work either way. On top of that, an important thing to tune and test a lot is PID values because mine will definitely differ from yours. 
There are 3 values: Proportional (P) measures the current error to make immediate, broad corrections based on how far away you are from your target, using the code name kp.
Integral (I) adds up past errors over time to eliminate any remaining, permanent gap that the Proportional term misses, using the code name ki.
Derivative (D) looks at the speed of the error change to predict future overshoot and smooth out the system's movement, using the code name kd.

Now for the driving part. I used the app called dabble, its a bit old and limited to LEDCc functions, but it's not a problem. The right and left controls on the gamepad just add PWM for a certain wheel. The system uses differential steering, meaningif Ii want to turn right, the left wheel spins while the right one stops fully, and vise versa. However, for the front and back moevemnt i wanted it to maintain its balance, like right and left,t because while they work, the PID is still balancing the robot. However, in the front and back, the way to do it was to make it a false angle it needs to follow that makes it lean as it will fall, and so it moves. However, the problem was that it would reach that point without falling or sliding, so it wouldn't move, meaning it stayed in place, and that is because my frame is short, but you can make yours tall, and that system would work. SO i resorted to just adding P, meaning giving it power, and I disabled the PID balancing system while driving forward or back; then it kicks back in when you leave the button to balance it. Also, just for fun, I added that the X button would make the robot spin in place, one while forward and the other backward, and it just keeps spinning. 

Now, to do this project, the frame is custom-built as you want, but then the code is ready for you. Just copy and paste it, and then everything is wired together, and it is detailed in the wiring diagram.

<img width="1600" height="1200" alt="WhatsApp Image 2026-09-01 at 2 33 06 AM (1)" src="https://github.com/user-attachments/assets/df9c1ef3-5b5e-432f-9b8d-febd7d79deb1" />
<img width="1200" height="1600" alt="WhatsApp Image 2026-09-01 at 2 33 06 AM (2)" src="https://github.com/user-attachments/assets/74fb6660-73bf-4010-af5b-a348d7b982c4" />
<img width="1600" height="1200" alt="WhatsApp Image 2026-09-01 at 2 33 07 AM" src="https://github.com/user-attachments/assets/87b383f5-960f-45a1-ae68-af0eaffd1678" />
<img width="1600" height="1200" alt="WhatsApp Image 2026-09-01 at 2 33 07 AM (1)" src="https://github.com/user-attachments/assets/331e7ac6-8e2e-406d-a39c-c9edfa7c896e" />
<img width="1600" height="1200" alt="WhatsApp Image 2026-09-01 at 2 33 05 AM" src="https://github.com/user-attachments/assets/af2baae7-0ea9-428f-baad-528beda41e74" />
<img width="1600" height="1200" alt="WhatsApp Image 2026-09-01 at 2 33 06 AM" src="https://github.com/user-attachments/assets/be29dfd8-6b95-4120-a69f-410bdbc4e3a6" />




This is the full YouTube video link for the project: https://youtube.com/shorts/5AvpLy7rWrU?feature=share


### Bill of Materials (BOM)

| Item # | Component Name / Description | Qty | Unit Price (EGP) | Total Price (EGP) | Reference / Source Notes |
|---|---|---|---|---|---|
| 1 | Hot Glue Gun & Glue Sticks | By consumption | 0 EGP | 0 EGP | Frame assembly |
| 2 | Popsicle Sticks (Pack for Frame) | 1 | 40 EGP | 40 EGP | Structural chassis / frame material |
| 3 | TT Mini DC Gearbox Motors (Pair - 2 Motors) | 1 | 50 EGP | 50 EGP | https://www.ram-e-shop.com/shop/ro-motor-tt-mini-dc-gearbox-motors-pair-2-motors-with-back-shaft-9455 |
| 4 | Robot Wheels 65x26mm for Mini DC Gearbox (2 Tires) | 1 | 50 EGP | 50 EGP | https://www.ram-e-shop.com/shop/ro-wheel-rw002-robot-wheels-65x26mm-for-mini-dc-gearbox-yellow-motors-2-tires-rw002-8808 |
| 5 | L298N Dual H-Bridge Motor Driver Module (Red Board) | 1 | 75 EGP | 75 EGP | https://www.ram-e-shop.com/shop/kit-l298-red-l298-module-red-board-dual-h-bridge-motor-driver-using-l298n-7084 |
| 6 | ESP32 ESP32S 30Pin Development Board (CP2102) | 1 | 280 EGP | 280 EGP | https://www.ram-e-shop.com/shop/kit-esp32-esp32s-30pin-esp-32s-30pin-development-board-wifi-bluetooth-with-new-cp2102-chip-7433 |
| 7 | MPU6050 (GY-521) 6-DOF IMU Gyroscope & Accelerometer Module | 1 | 175 EGP | 175 EGP | https://www.ram-e-shop.com/shop/kit-imu-mpu6050-gy521-imu-6-dof-mpu-6050-gyroscope-accelerometer-sensor-module-7076 |
| 8 | Solder Wire Roll | By consumption | 0 EGP | 0 EGP | Electrical connections |
| 9 | Jumper Wires (Female to Male) | By consumption | 0 EGP | 0 EGP | |
| 10 | 3S LiPo Battery (11.1V / 12V, 2200mAh) | 1 | 900 EGP | 900 EGP | Main power supply (Note: Overpowered for this setup; a smaller 2S LiPo or 18650 battery setup is recommended) |
| - | Total Project Cost (EGP) | | | 1,570 EGP | |
| - | Total Project Cost (USD) | | | ~$32.37 USD | Baseline exchange conversion (1 USD ≈ 48.5 EGP) |

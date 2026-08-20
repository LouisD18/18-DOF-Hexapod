# 18-DOF-Hexapod
<img width="1208" height="825" alt="image" src="https://github.com/user-attachments/assets/8a4fc03e-fb20-4cc7-a9fa-e8d4984a1922" />

This project documents the development of an 18-DOF hexapod robot I began designing and building in April 2026. The mechanical system, electronics, controller, and locomotion software were developed from scratch as an independent robotics project.

Features:
- Tripod gait genereation
- 3 DOF inverse kinematics
- IMU-based body orientation sensing
- Ground detection per foot
- Ground adaptation (a bit iffy right now)

Hardware:
- Arduino UNO
- Custom PCB
- 18 35kg DS3235 servos
- 3s LIPO battery
- OMRON Lever foot switches
- 6.8/5V UBECs
- 3D-printed chassis/legs

The hexapod moves by using inverse kinematics to trace out a bezier path. If the ground is detected with the foot switches, a modified bezier path is followed. The following image shows this.  
<img width="638" height="501" alt="image" src="https://github.com/user-attachments/assets/b39bf4aa-c605-48f2-8248-d0a4294faf25" />

#pragma config(Sensor, dgtl1,  encLiftRight,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  encLiftLeft,       sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  encClawRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  encClawLeft,    sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  encDriveRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl11, encDriveLeft,    sensorQuadEncoder)
#pragma config(Motor,  port1,           clawLeft,      tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           lift3,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           lift2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           driveLeft,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           driveRight,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           lift4,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           lift1,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          clawRight,     tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int leftAdjustment = 0;
int rightAdjustment = 0;

int driveThreshold = 25;
bool driveThresholdOn = true;

void drive(int rot1, int trans1)
{
	motor[driveRight] = 0.75*(trans1 - rot1) + rightAdjustment;
	motor[driveLeft] = 0.75*(trans1 + rot1) + leftAdjustment;
}

void driveExactly(int distance, int angle)
{

	//Clear sensors
	SensorValue[encDriveLeft] = 0;
	SensorValue[encDriveRight] = 0;

	//DISTANCE ONLY
	//drives the distance in inches
	//forwards is positive
	//27.7 ticks per inch
	if(angle == 0 && distance != 0){
		float kp = 0.75;
		driveThresholdOn = true;

		int direction = 1;
		if(distance < 0)
			direction = -1;

		int target = distance * 27.7;

		while(abs(target - SensorValue[encDriveLeft]) > 10){
			int driveMotorSpeed = abs(target - SensorValue[encDriveLeft]) * kp;
			if(driveMotorSpeed > 127)//delete this if we remove 0.75 from drive function
				driveMotorSpeed = 127;
			if(driveMotorSpeed < driveThreshold && driveThresholdOn){
				driveMotorSpeed = driveThreshold;
			}
			drive(0,direction * driveMotorSpeed);
		}

		direction = direction * -1;

		drive(0,127 * direction);
		wait1Msec(100);

		drive(0,0);
	}

	//ANGLE ONLY
	//turns the robot based on angle
	//clockwise is positive
	//3.66 ticks per angle
	if(angle != 0 && distance == 0){
		float kp = 2.0;
		driveThresholdOn = true;

		int direction = 1;
		if(angle < 0)
			direction = -1;

		int target = angle * 3.66;

		while(abs(target - SensorValue[encDriveLeft]) > 10){
			int driveMotorSpeed = abs(target - SensorValue[encDriveLeft]) * kp;
			if(driveMotorSpeed > 127)//delete this if we remove the 0.75 from drive function
				driveMotorSpeed = 127;
			if(driveMotorSpeed < driveThreshold && driveThresholdOn){
				driveMotorSpeed = driveThreshold;
			}
			drive(direction * driveMotorSpeed, 0);
		}

		direction = direction * -1;

		drive(127 * direction, 0);
		wait1Msec(100);

		drive(0,0);
	}
}

task autoAdjustMotors()
{
	leftAdjustment = 0;
	rightAdjustment = 0;

	int error = 0;
	int left = 0;
	int right = 0;

	int kp = 2;

	while(true){
		left = SensorValue[encDriveLeft]; //gets initial value
		right = -1*SensorValue[encDriveRight];
		wait1Msec(100);
		left = abs(SensorValue[encDriveLeft] - left);
		right = abs(-1*SensorValue[encDriveRight] - right);

		error = abs(left - right);

		if(left > right){
			leftAdjustment = 0;
			rightAdjustment = error/kp;
		}
		else if(left < right){
			leftAdjustment = error/kp;
			rightAdjustment = 0;
		}
		else{
			leftAdjustment = 0;
			rightAdjustment = 0;
		}
	}
}

task main()
{
	//THINGS TO DO:
	//RECORD AUTONOMOUS
	//GO BACK TO START FUNCTION
	//PID FOR DRIVE CHAIN/PREADJUST MOTOR FUNCTION, WHICH MOTOR IS FASTER
	//LCD
	//SKILLS/AUTON
	//IF ONE DRIVE MOTOR BREAKS, CUT OTHER SIDE IN HALF????
	//IMPLEMENT MORE CONCISE PID IN DRIVE EXACTLY??

	startTask(autoAdjustMotors);

	wait1Msec(1000);
	driveExactly(36,0);
}

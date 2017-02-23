#pragma config(Sensor, dgtl1,  encLiftRight,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  encLiftLeft,       sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  encClawRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  encClawLeft,    sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  encDriveRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl11, encDriveLeft,    sensorQuadEncoder)
#pragma config(Motor,  port1,           clawLeft,      tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           lift3,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           lift2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           drive3,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           drive2,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           drive1,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           drive4,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           lift4,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           lift1,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          clawRight,     tmotorVex393_HBridge, openLoop)
//#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//for Margaret Thatcher, the robot

#pragma platform(VEX)

#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)
//Variable that will carry between modes
//int Program;

#include "Vex_Competition_Includes.c"

#define getQuadVal() ( ( abs(SensorValue[encLiftRight]) + abs(SensorValue[encLiftLeft]) ) / 2 )

#define CR clawRight //reconcile differences in the port in #pragma with #define
#define CL clawLeft
#define CLenc encClawLeft
#define CRenc encClawRight

const float INTEGRAL_ACTIVE_ZONE = 50.0;
const int PIDTHRESHOLD = 30;
float clawSetPoint = 0;

bool thresholdOn = true;
bool clawPIDRunning = true;
bool pressedLastCycle = false;
bool lock = false;
//float clawTarget;

/*task getClawAvg()
{
	while (true) {
		float encLiftRightVal = abs(SensorValue[encLiftRight]);
		float encLiftLeftVal = abs(SensorValue[encLiftLeft]);
		clawTarget = (encLiftRightVal + encLiftLeftVal)/2;
	}
}*/

void drive(int rot1, int trans1)
{
	motor[drive1] = 0.75*(trans1 - rot1);
	motor[drive2] = 0.75*(trans1 + rot1);
	motor[drive3] = 0.75*(trans1 + rot1);
	motor[drive4] = 0.75*(trans1 - rot1);
}

void lift(int liftPower)
{
	motor[lift2] = liftPower;
	motor[lift1] = liftPower;
	motor[lift3] = liftPower;
	motor[lift4] = liftPower;
}

float pid(float kp, float ki, float kd, float target, float &error, float &errorTotal, float &prevError, float sensVal)
{
	error = target - sensVal;
	errorTotal += error;
	float deltaError = error - prevError;

	//set components of pid
	float proportional = kp * error;
	float integral = ki * errorTotal;
	float derivative = kd * deltaError;

	// Prevent Integral from Getting Too High or Too Low
	if (integral > INTEGRAL_ACTIVE_ZONE) {
		integral = INTEGRAL_ACTIVE_ZONE;
	}
	if (integral < -INTEGRAL_ACTIVE_ZONE) {
		integral = -INTEGRAL_ACTIVE_ZONE;
	}

	prevError = error;

	if((0.75*(abs(proportional + integral + derivative)) < PIDTHRESHOLD) && thresholdOn)
	{
		return 0;
	}
	else
	{
		return 0.75*(proportional + integral + derivative);
	}

}

void encClear()
{
	SensorValue[encLiftRight] = 0;
	SensorValue[encLiftLeft] = 0;
	SensorValue[encClawRight] = 0;
	SensorValue[encClawLeft] = 0;
	SensorValue[encDriveRight] = 0;
	SensorValue[encDriveLeft] = 0;

}

void pre_auton()
{
	//nothing
}

/*void pre_auton()
{
	encClear();
  // Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
  // Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
  bStopTasksBetweenModes = true;
	//Leave this value alone
	int lcdScreenMin = 1;
	//This keeps track of which program you want to run
	int lcdScreen = 1;
	//Change this value to be the maximum number of programs you want on the robot
	int lcdScreenMax = 4;
	//Turns on the Backlight
	bLCDBacklight = true;

	//Copied from someone's sample code because the documentation for RobotC won't tell me anything useful
	//These should logically work, but I'm not 100% sure
	const short leftButton = 1;
	const short centerButton = 2;
	const short rightButton = 4;

	while (bIfiRobotDisabled == 1) { //Ensures this code will run ONLY when the robot is disabled
		if (nLCDButtons == leftButton) { //Scrolls to the left
			if (lcdScreenMin == lcdScreen) {
				lcdScreen = lcdScreenMax;
				wait1Msec(250);
			} else {
				lcdScreen --;
				wait1Msec(250);
			}
		}
		if (nLCDButtons == rightButton) { //Scrolls to the right
			if (lcdScreenMax == lcdScreen) {
				lcdScreen = lcdScreenMin;
				wait1Msec(250);
			} else {
				lcdScreen++;
				wait1Msec(250);
			}
		}
		if (lcdScreen == 1 && Program != 1) {
			displayLCDCenteredString (0, "Program"); //Name the first program here
			displayLCDCenteredString (1, "1"); //Name the first program here
			if (nLCDButtons == centerButton) {
				Program = lcdScreen; //Sets the Program to the one on-screen
				displayLCDCenteredString (0, "Autonomous Has");
				displayLCDCenteredString (1, "Been Selected!");
				wait1Msec(1500);
			}
		} else if (lcdScreen == 1 && Program == 1) {
			displayLCDCenteredString (0, "Program"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "[1]"); //So that while we're scrolling, we can have one marked
		} else if (lcdScreen == 2 && Program != 2) {
			displayLCDCenteredString (0, "Program"); //Name the second program here
			displayLCDCenteredString (1, "2"); //Name the second program here
			if (nLCDButtons == centerButton) {
				Program = lcdScreen; //Sets the Program to the one on-screen
				displayLCDCenteredString (0, "Autonomous Has");
				displayLCDCenteredString (1, "Been Selected!");
				wait1Msec(1500);
			}
		} else if (lcdScreen == 2 && Program == 2) {
			displayLCDCenteredString (0, "Program"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "[2]"); //So that while we're scrolling, we can have one marked
		} else if (lcdScreen == 3 && Program != 3) {
			displayLCDCenteredString (0, "Program"); //Name the third program here
			displayLCDCenteredString (1, "3"); //Name the third program here
			if (nLCDButtons == centerButton) {
				Program = lcdScreen; //Sets the Program to the one on-screen
				displayLCDCenteredString (0, "Autonomous Has");
				displayLCDCenteredString (1, "Been Selected!");
				wait1Msec(1500);
			}
		} else if (lcdScreen == 3 && Program == 3) {
			displayLCDCenteredString (0, "Program"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "[3]"); //So that while we're scrolling, we can have one marked
		} else if (lcdScreen == 4 && Program != 4) {
			displayLCDCenteredString (0, "Program"); //Name the fourth program here
			displayLCDCenteredString (1, "4"); //Name the fourth program here
			if (nLCDButtons == centerButton) {
				Program = lcdScreen; //Sets the Program to the one on-screen
				displayLCDCenteredString (0, "Autonomous Has");
				displayLCDCenteredString (1, "Been Selected!");
				wait1Msec(1500);
			}
		} else if (lcdScreen == 4 && Program == 4) {
			displayLCDCenteredString (0, "Program"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "[4]"); //So that while we're scrolling, we can have one marked
		}
	}
}
*/

task pincerPID()
{
	const float L_claw_kp = 9.0;
	const float L_claw_ki = 0.01;
	const float L_claw_kd = 0.5;

	const float R_claw_kp = 9.0;
	const float R_claw_ki = 0.01;
	const float R_claw_kd = 0.5;

	float L_claw_error = 0;
	float L_claw_prevError = 0;
	float L_claw_errorTotal = 0;

	float R_claw_error = 0;
	float R_claw_prevError = 0;
	float R_claw_errorTotal = 0;

	float L_claw_sensVal, R_claw_sensVal;

	while (true)
	{
		if(clawPIDRunning)
		{
			L_claw_sensVal = SensorValue(encClawLeft);
			R_claw_sensVal = SensorValue(encClawRight);

			motor[CL] = pid(L_claw_kp, L_claw_ki, L_claw_kd, clawSetPoint, L_claw_error, L_claw_errorTotal, L_claw_prevError, L_claw_sensVal);
			motor[CR] = pid(R_claw_kp, R_claw_ki, R_claw_kd, clawSetPoint, R_claw_error, R_claw_errorTotal, R_claw_prevError, R_claw_sensVal);
		}
		sleep(20);
	}
}


//task autonomous()
//{
//	encClear();
//	startTask(pincerPID);
//	startTask(clawAuwait1wait1Msec;
//	startTask(driveAuton);
//	startTask(liftAuton);
//	sleep(1500);
//}

/*
void runSequenceStep(int driveRot, int driveTrans, int clawValue, int liftValue)
{
	drive(driveRot, driveTrans);
	motor[clawLeft] = -1*clawValue;
	lift(liftValue);
}

void runAutonomousSequenceRight()
{
	int sequence[21][5] = {
		//{rotation,translation,claw,lift,time in milliseconds}
		{0,127,0,0,150},//run to fence
		{0,127,127,127,800},//run up to fence, with claws opening and lift going up
		{0,127,0,127,950},//keep going but claws stop
		{0,-127,0,0,400},//go back
		{0,-127,0,-127,900},//go back and lower lift
		{0,0,0,-127,200},//lower lift
		{-127,0,0,0,500},//turn towards cube
		{0,127,0,0,800},//drive towards cube
		{0,0,127,0,1000},//grab cube
		{0,127,30,127,300},//drive forwards and lift up
		{127,0,30,127,600},//lift cube while turning to fence
		{0,0,30,127,600},//lift cube
		{0,127,30,127,1050},//lift cube to fence height and drive to fence
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,500},//drive backwards
		{0,-127,0,-127,600},//drive backwards and lift down
		{0,127,0,0,600},//drive fowards
		{0,127,0,127,650},//drive fowards and lift up
		{0,-127,0,0,700},//drive backwards
		{0,-127,0,-127,500},//drive backwards and lift down
		{0,0,0,0,0}//stop
	};

	for(int x = 0; x < 21; x++){
		runSequenceStep(sequence[x][0],sequence[x][1],sequence[x][2],sequence[x][3]);
		wait1Msec(sequence[x][4]);
	}
}

void runAutonomousSequenceLeft()
{
	int sequence[21][5] = {
		//{rotation,translation,claw,lift,time in milliseconds}
		{0,127,0,0,150},//run to fence
		{0,127,127,127,800},//run up to fence, with claws opening and lift going up
		{0,127,0,127,950},//keep going but claws stop
		{0,-127,0,0,400},//go back
		{0,-127,0,-127,900},//go back and lower lift
		{0,0,0,-127,200},//lower lift
		{127,0,0,0,500},//turn towards cube
		{0,127,0,0,800},//drive towards cube
		{0,0,127,0,1000},//grab cube
		{0,127,30,127,300},//drive forwards and lift up
		{-127,0,30,127,600},//lift cube while turning to fence
		{0,0,30,127,600},//lift cube
		{0,127,30,127,1050},//lift cube to fence height and drive to fence
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,500},//drive backwards
		{0,-127,0,-127,600},//drive backwards and lift down
		{0,127,0,0,700},//drive fowards
		{0,127,0,127,550},//drive fowards and lift up
		{0,-127,0,0,700},//drive backwards
		{0,-127,0,-127,500},//drive backwards and lift down
		{0,0,0,0,0}//stop
	};

	for(int x = 0; x < 21; x++){
		runSequenceStep(sequence[x][0],sequence[x][1],sequence[x][2],sequence[x][3]);
		wait1Msec(sequence[x][4]);
	}
}

void runAutonomousSequenceSkills()
{
	int sequence[34][5] = {
		//{rotation,translation,claw,lift,time in milliseconds}
		{0,127,0,0,150},//run to fence
		{0,127,127,127,800},//run up to fence, with claws opening outwards(closing inwards) and lift going up
		{0,127,0,127,1050},//keep going but claws stop
		{0,-127,0,0,400},//go back
		{0,-127,0,-127,900},//go back and lower lift
		{0,0,0,-127,200},//lower lift
		{127,0,0,0,500},//turn towards cube
		{0,127,0,0,800},//drive towards cube
		{0,0,127,0,1000},//grab cube
		{-127,0,30,127,700},//lift cube while turning to fence
		{0,0,30,127,600},//lift cube
		{0,127,30,127,1050},//lift cube to fence height and drive to fence
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,450},//drive backwards
		{0,-127,0,-127,1000},//drive backwards and lift down
		{0,127,0,127,400},//drive fowards and lift up
		{0,127,0,0,800},//drive fowards
		{0,127,0,127,300},//drive fowards and lift up
		{0,-127,0,0,400},//drive backwards
		{0,-127,0,-127,800},//drive backwards and lift down
		{0,127,0,0,1000},//drive forwards
		{0,0,127,0,1000},//close claws
		{0,-127,0,127,1000},//drive backwards and lift up
		{0,127,0,127,1000},//drive forwards and lift up
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,400},//drive backwards
		{-127,-127,0,-127,800},//drive backwards and lift down and turn towards back wall
		{0,127,0,0,70},//drive fowards
		{-127,127,0,0,700},//turn towards back wall and move fowards
		{-127,0,0,0,700},//turn towards back wall
		{0,127,0,0,650},//drive towards back wall
		{0,0,127,0,1000},//close claw on stars
		{0,-127,0,0,1500},//drive backwards
		{0,0,0,0,0}//stop
	};

	for(int x = 0; x < 34; x++){
		runSequenceStep(sequence[x][0],sequence[x][1],sequence[x][2],sequence[x][3]);
		wait1Msec(sequence[x][4]);
	}
}

task timeAuton()
{
	wait1Msec(15000);
	stopTask(autonomous);
	//startTask(usercontrol);
}
*/

//task usercontrol()
//{
//	encClear();
//	startTask(pincerPID);
//}

// Controls the Claw of the Robot During the User Control Portion of the Competition
task updatePincerUserControl()
{
	int CLsens, CRsens;
	int lastTime = 0;
	int CLAW_MIN_ROTATION = 100000000;
	int CLAW_MAX_ROTATION = -100000000;
	while(true)
	{
		CLsens = SensorValue(CLenc);
		CRsens = SensorValue(CRenc);

		if (vexRT[Btn8R] == 1 || vexRT[Btn8DXmtr2] == 1) {
			clawPIDRunning = false;
			// to give other thread time,  stop pid
			sleep(30);
			motor[CL] = 0;
			motor[CR] = 0;
			clawSetPoint = (CLsens + CRsens) / 2.0;
			lastTime = time1[T1];
		}
		else {
			if (vexRT[Btn5U] == 1 || vexRT[Btn6UXmtr2] == 1)
			{
				clawPIDRunning = false;
				// open claw fast
				if(CLsens < CLAW_MIN_ROTATION) motor[CL] = 127;
				if(CRsens < CLAW_MIN_ROTATION) motor[CR] = 127;
				clawSetPoint = (CLsens + CRsens) / 2.0;
				lastTime = time1[T1];
				lock = false;
			}
			else if (vexRT[Btn6U] == 1 || vexRT[Btn6DXmtr2] == 1)
			{
				clawPIDRunning = false;
				// close claw fast
				if(CLsens > CLAW_MAX_ROTATION) motor[CL] = -127;
				if(CRsens > CLAW_MAX_ROTATION) motor[CR] = -127;
				clawSetPoint = (CLsens + CRsens) / 2.0;
				lastTime = time1[T1];
				lock = true;
			}
			/*else if (vexRT[Btn6D] == 1 || vexRT[Btn5DXmtr2] == 1)
			{
				clawPIDRunning = false;
				// close claw slow
				if(CLsens > CLAW_MAX_ROTATION) motor[CL] = 30;
				if(CRsens > CLAW_MAX_ROTATION) motor[CR] = 30;
				clawSetPoint = (CLsens + CRsens) / 2.0;
				lastTime = time1[T1];
			}
			else if (vexRT[Btn5D] == 1 || vexRT[Btn5UXmtr2] == 1)
			{
				clawPIDRunning = false;
				// open claw slow
				if(CLsens < CLAW_MIN_ROTATION) motor[CL] = -30;
				if(CRsens < CLAW_MIN_ROTATION) motor[CR] = -30;
				clawSetPoint = (CLsens + CRsens) / 2.0;
				lastTime = time1[T1];
			}*/
			else
			{
				if(time1[T1] - lastTime < 300) {
					clawSetPoint = (CLsens + CRsens) / 2.0;
				}
				if(!lock){
					clawPIDRunning = true;
				}
				else
				{
				motor[CL] = -30;
				motor[CR] = -30;
				}
			}
		}

		sleep(20);
	}
}

void driveExactly(int distance)
{
	//drives forward the distance in inches
	//encoder resolution = 360
	//wheel diameter = 4
	//28.6479 counts per inch of linear translation

	SensorValue[encDriveLeft] = 0;
	SensorValue[encDriveRight] = 0;

	int multiplier = 1;
	if(distance < 0)
		multiplier = -1;

	int target = distance * 28.6479;

	while(abs(target - SensorValue[encDriveLeft]) > 720){
		drive(0,multiplier * 127);
	}


	while(abs(target - SensorValue[encDriveLeft]) > 360){
		drive(0,multiplier * 127);
	}

	while(abs(target - SensorValue[encDriveLeft]) > 50){
		drive(0,multiplier*50);
	}
	while(abs(target - SensorValue[encDriveLeft]) < 3){
		drive(0,0);
	}
	startTask(usercontrol);
}

//task autoAdjustMotors()
//{
//	int leftAdjustment = 0;
//	int rightAdjustment = 0;

//	int error = 0;
//	int left = 0;
//	int right = 0;

//	int kp = 5;
//	int left1 = 0;
//	int right1 = 0;

//	while(true){
//		left1 = SensorValue[encClawLeft]; //gets initial value
//		right1 = -1 * SensorValue[encClawRight];
//		wait1Msec(100);
//		left = abs(SensorValue[encClawLeft] - left1);
//		right = abs(SensorValue[encClawRight] - right1);

//		error = abs(left - right);

//		if(left > right){
//			leftAdjustment = 0;
//			rightAdjustment = error/kp;
//		}
//		else if(left < right){
//			leftAdjustment = error/kp;
//			rightAdjustment = 0;
//		}
//		else{
//			leftAdjustment = 0;
//			rightAdjustment = 0;
//		}
//	}
//}

//STUFF TO WORK ON IN CODE:
//SOMEHOW GET THIRD STAR ON FIRST FENCE
//GET SENSORS TO WORK
//GET OLD CLAW BACK??
//learn how to find length of array to put in for loop!!!!!!!!!!!
//sweep back two stars with cube
//collect encoder data with test and debug with writeDebugStreamLine
//find out whats overloading robot?? repeatedly starting tasks in user control?

task autonomous()
{
	//startTask(timeAuton);
	encClear();
	startTask(pincerPID);
	//startTask(autoAdjustMotors);
	driveExactly(72);
	clawSetPoint = -160;
	wait1Msec(2000);
	lift(127);
	wait1Msec(2000);
	lift(0);
	driveExactly(-72);
	clawSetPoint = 0;
	wait10Msec(20);
	stopAllTasks();
	/*if(SensorValue[skills] == 1)
		runAutonomousSequenceSkills();
	else
		if(SensorValue[autonomousSide] == 0)
			runAutonomousSequenceRight();
		else
			runAutonomousSequenceLeft();*/
}

task usercontrol()
{
	encClear();
	int t1 = 0, r1 = 0, motorThreshold = 25;
	startTask(pincerPID);
	startTask(updatePincerUserControl);

	while(true){
		if(vexRT[Btn7R] == 1)
			startTask(autonomous);

		if(vexRT[Btn7L] == 1)
			stopTask(autonomous);


		if(abs(vexRT[Ch1]) > motorThreshold)
			r1 = vexRT[Ch1];
		else
			r1 = 0;

		if(abs(vexRT[Ch2]) > motorThreshold)
			t1 = vexRT[Ch2];
		else
			t1 = 0;

		if (abs(vexRT[Ch3]) > motorThreshold)
		{
			lift(vexRT[Ch3]);
		}
		else
		{
			lift(0);
		}
		drive(r1,t1);
	}
}
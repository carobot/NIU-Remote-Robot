#include <Arduino.h>

/*

----------------------------
UNO Pin Layout
----------------------------

					| D13 - Joystick SCK
					| D12 - Joystick MISO
					| D11 - Joystick MOSI
					| D10 - Joystick SS
					| D9* - Right Motor ENA
					| D8  - Right Motor IN1
					| D7  - Right Motor IN2
					| D6* - Left Motor ENB
				A0 || D5  - Left Motor IN3
				A1 || D4  - Left Motor IN4
				A2 || D3
	Trigger -	A3 || D2
				A4 || D1
				A5 || D0

Keep D2 and D3 open for interrupt
Keep A4 and A5 open for I2C

----------------------------
Joystick Receiver Connection
----------------------------

Reference:
https://store.curiousinventor.com/guides/PS2
https://github.com/SukkoPera/PsxNewLib

MADCATZ 2.4Ghz (Purple)
Receiver Arduino
-------- -------
VCC      3.3V
D/T/R    D10 (SS)    NOTE: PS Attention
D/S/R    N/C
TXD      D12 (MISO)  NOTE: Data
RXD      D11 (MOSI)  NOTE: Command
SCK      D13 (SCK)   NOTE: Clock
GND      GND

*/

/*

Linux terminal command:
cu -l /dev/device -s baud-rate-speed
Look for device from "Auto-detected" during upload:
cu -l /dev/ttyACM0 -s 115200

*/


/////////////////////////////////////////////////////////////
// *** DO NOT USE MULTI-LINE COMMENT BEYOND THIS POINT *** //
/////////////////////////////////////////////////////////////

#include <DigitalIO.h>
#include <PsxControllerHwSpi.h>

#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

// Are we still debugging?
// Joystick connection message will always show right now
boolean debug_mode = false;

/// Joystick Stuff

// This can be changed freely but please see above
const byte PIN_PS2_ATT = 10;

//const byte PIN_BUTTONPRESS = A0;
//const byte PIN_HAVECONTROLLER = A1;

const char buttonSelectName[] PROGMEM = "Select";
const char buttonL3Name[] PROGMEM = "L3";
const char buttonR3Name[] PROGMEM = "R3";
const char buttonStartName[] PROGMEM = "Start";
const char buttonUpName[] PROGMEM = "Up";
const char buttonRightName[] PROGMEM = "Right";
const char buttonDownName[] PROGMEM = "Down";
const char buttonLeftName[] PROGMEM = "Left";
const char buttonL2Name[] PROGMEM = "L2";
const char buttonR2Name[] PROGMEM = "R2";
const char buttonL1Name[] PROGMEM = "L1";
const char buttonR1Name[] PROGMEM = "R1";
const char buttonTriangleName[] PROGMEM = "Triangle";
const char buttonCircleName[] PROGMEM = "Circle";
const char buttonCrossName[] PROGMEM = "Cross";
const char buttonSquareName[] PROGMEM = "Square";

const char* const psxButtonNames[PSX_BUTTONS_NO] PROGMEM = {
	buttonSelectName,
	buttonL3Name,
	buttonR3Name,
	buttonStartName,
	buttonUpName,
	buttonRightName,
	buttonDownName,
	buttonLeftName,
	buttonL2Name,
	buttonR2Name,
	buttonL1Name,
	buttonR1Name,
	buttonTriangleName,
	buttonCircleName,
	buttonCrossName,
	buttonSquareName
};

byte psxButtonToIndex (PsxButtons psxButtons) {
	byte i;

	for (i = 0; i < PSX_BUTTONS_NO; ++i) {
		if (psxButtons & 0x01) {
			break;
		}

		psxButtons >>= 1U;
	}

	return i;
}

FlashStr getButtonName (PsxButtons psxButton) {
	FlashStr ret = F("");
	
	byte b = psxButtonToIndex (psxButton);
	if (b < PSX_BUTTONS_NO) {
		PGM_BYTES_P bName = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(psxButtonNames[b])));
		ret = PSTR_TO_F (bName);
	}

	return ret;
}

void dumpButtons (PsxButtons psxButtons) {
	static PsxButtons lastB = 0;

	if (psxButtons != lastB) {
		lastB = psxButtons;     // Save it before we alter it
		
		Serial.print (F("Pressed: "));

		for (byte i = 0; i < PSX_BUTTONS_NO; ++i) {
			byte b = psxButtonToIndex (psxButtons);
			if (b < PSX_BUTTONS_NO) {
				PGM_BYTES_P bName = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(psxButtonNames[b])));
				Serial.print (PSTR_TO_F (bName));
			}

			psxButtons &= ~(1 << b);

			if (psxButtons != 0) {
				Serial.print (F(", "));
			}
		}

		Serial.println ();
	}
}

void dumpAnalog (const char *str, const byte x, const byte y) {
	Serial.print (str);
	Serial.print (F(" analog: x = "));
	Serial.print (x);
	Serial.print (F(", y = "));
	Serial.println (y);
}

const char ctrlTypeUnknown[] PROGMEM = "Unknown";
const char ctrlTypeDualShock[] PROGMEM = "Dual Shock";
const char ctrlTypeDsWireless[] PROGMEM = "Dual Shock Wireless";
const char ctrlTypeGuitHero[] PROGMEM = "Guitar Hero";
const char ctrlTypeOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerTypeStrings[PSCTRL_MAX + 1] PROGMEM = {
	ctrlTypeUnknown,
	ctrlTypeDualShock,
	ctrlTypeDsWireless,
	ctrlTypeGuitHero,
	ctrlTypeOutOfBounds
};

/// Motor Stuff

int left_speed = 0, right_speed = 0;

// Motor A - Right Motor
const byte PIN_ENA = 9;
const byte PIN_IN1 = 8;
const byte PIN_IN2 = 7;

// Motor B - Left Motor
const byte PIN_ENB = 6;
const byte PIN_IN3 = 5;
const byte PIN_IN4 = 4;

const byte PIN_TRIGGER = A3;
const byte PIN_BUBBLE_TRIGGER = A0;

void setLeftMotor(int speed) {

	// TODO: Add in some kind of ramp
	// same dir or zero, higher speed - ramp up
	// same dir or zero, lower speed - drop
	// diff dir, reduce to zero

	// if ((speed * left_speed) > 0 || left_speed == 0) {
	// 	// same direction or zero
	// } else {
	// 	// diff dir, reduce to zero
	// 	if (left_speed > 0) left_speed--;
	// 	else left_speed++;
	// }

	speed = constrain(speed, -255, 255);

	if (abs(speed) > 30) {
		left_speed = speed;
	} else {
		left_speed = 0;
	}

	if (debug_mode) Serial.print (F("Left: "));

	if (left_speed > 0) {
		// Move forward
		digitalWrite(PIN_IN3, HIGH);
    	digitalWrite(PIN_IN4, LOW);
		if (debug_mode) Serial.print (F("Forward: "));
	} else if (left_speed < 0) {
		// Move backward
		digitalWrite(PIN_IN3, LOW);
    	digitalWrite(PIN_IN4, HIGH);
		if (debug_mode) Serial.print (F("Backward: "));
	} else {
		// Stop
		digitalWrite(PIN_IN3, LOW);
    	digitalWrite(PIN_IN4, LOW);
		if (debug_mode) Serial.print (F("Stop: "));
	}

	analogWrite(PIN_ENB, abs(left_speed));
	if (debug_mode) Serial.print (abs(left_speed));

}

void setRightMotor(int speed) {
	//See above

	speed = constrain(speed, -255, 255);

	if (abs(speed) > 30) {
		right_speed = speed;
	} else {
		right_speed = 0;
	}

	if (debug_mode) Serial.print (F(" Right: "));

	if (right_speed > 0) {
		// Move forward
		digitalWrite(PIN_IN1, HIGH);
    	digitalWrite(PIN_IN2, LOW);
		if (debug_mode) Serial.print (F("Forward: "));
	} else if (right_speed < 0) {
		// Move backward
		digitalWrite(PIN_IN1, LOW);
    	digitalWrite(PIN_IN2, HIGH);
		if (debug_mode) Serial.print (F("Backward: "));
	} else {
		// Stop
		digitalWrite(PIN_IN1, LOW);
    	digitalWrite(PIN_IN2, LOW);
		if (debug_mode) Serial.print (F("Stop: "));
	}

	analogWrite(PIN_ENA, abs(right_speed));
	if (debug_mode) Serial.println (abs(right_speed));
}

/////////////////////////////////////////////////////////////
//                      Main Code                          //
/////////////////////////////////////////////////////////////

PsxControllerHwSpi<PIN_PS2_ATT> psx;

boolean haveController = false;
int maxSpeed = 100;
int maxTurnSpeed = 75;

void setup () {
	//fastPinMode (PIN_BUTTONPRESS, OUTPUT);
	//fastPinMode (PIN_HAVECONTROLLER, OUTPUT);

	fastPinMode (PIN_ENA, OUTPUT);
	fastPinMode (PIN_IN1, OUTPUT);
	fastPinMode (PIN_IN2, OUTPUT);
	fastPinMode (PIN_ENB, OUTPUT);
	fastPinMode (PIN_IN3, OUTPUT);
	fastPinMode (PIN_IN4, OUTPUT);

	fastPinMode (PIN_TRIGGER, OUTPUT);
	fastPinMode (PIN_BUBBLE_TRIGGER, OUTPUT);
	
	delay (300);

	Serial.begin (115200);
	Serial.println (F("Ready!"));
}
 
void loop () {
	static byte slx, sly, srx, sry;
	static int left_speed_setpoint, right_speed_setpoint;
	byte trigger_set = 0, bubble_trigger_set = 0, tank_mode = 0;
	PsxButtons buttons_pressed = 0;

	// Reading from Joystick start here VVV ////
	
	//fastDigitalWrite (PIN_HAVECONTROLLER, haveController);
	
	// DO NOT MODIFY FROM HERE to ... //
	
	if (!haveController) {
		if (psx.begin ()) {
			Serial.println (F("Controller found!"));
			delay (300);
			if (!psx.enterConfigMode ()) {
				Serial.println (F("Cannot enter config mode"));
			} else {
				PsxControllerType ctype = psx.getControllerType ();
				PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
				Serial.print (F("Controller Type is: "));
				Serial.println (PSTR_TO_F (cname));

				if (!psx.enableAnalogSticks ()) {
					Serial.println (F("Cannot enable analog sticks"));
				}
				
				//~ if (!psx.setAnalogMode (false)) {
					//~ Serial.println (F("Cannot disable analog mode"));
				//~ }
				//~ delay (10);
				
				if (!psx.enableAnalogButtons ()) {
					Serial.println (F("Cannot enable analog buttons"));
				}
				
				if (!psx.exitConfigMode ()) {
					Serial.println (F("Cannot exit config mode"));
				}
			}
			
			haveController = true;
		}
	} else {
		if (!psx.read ()) {
			Serial.println (F("Controller lost :("));
			haveController = false;
	// ... DO NOT MODIFY END //

		} else {
      
      	// HAVE Controller AND Input

			//fastDigitalWrite (PIN_BUTTONPRESS, !!psx.getButtonWord ());
			buttons_pressed = psx.getButtonWord ();
			if (debug_mode) dumpButtons (buttons_pressed);

			// MSB tp LSB
			// Square, Cross, Circle, Triangle, R1, L1, R2, L2, Left, Down, Right, Up, Start, R3, L3, Select

			trigger_set = (buttons_pressed & 0b0010000000000000) ? true : false; // circle button
			bubble_trigger_set = (buttons_pressed & 0b0100000000000000) ? true : false; // cross button
			tank_mode =   (buttons_pressed & 0b0000010000000000 && buttons_pressed & 0b0000100000000000) ? true : false; // L1 + R1

			byte lx, ly;
			psx.getLeftAnalog (lx, ly);
			if (lx != slx || ly != sly) {
				if (debug_mode) dumpAnalog ("Left", lx, ly);
				slx = lx;
				sly = ly;
			}

			byte rx, ry;
			psx.getRightAnalog (rx, ry);
			if (rx != srx || ry != sry) {
				if (debug_mode) dumpAnalog ("Right", rx, ry);
				srx = rx;
				sry = ry;
			}
		}
	}

	if (slx > 138 && slx < 118) slx = 128;
    if (sly > 138 && sly < 118) sly = 128;
    if (srx > 138 && srx < 118) srx = 128;
    if (sry > 138 && sry < 118) sry = 128;

	int left_joystic_y_mapped = map(sly, 0, 255, 255, -255);
	int left_joystic_x_mapped = map(slx, 0, 255, maxTurnSpeed, -maxTurnSpeed);
	int right_joystic_y_mapped = map(sry, 0, 255, 255, -255);
	int right_joystic_x_mapped = map(srx, 0, 255, maxTurnSpeed, -maxTurnSpeed);

	if (debug_mode) {
		Serial.print (F("Mapped: Left Y: "));
		Serial.print (left_joystic_y_mapped);
		Serial.print (F(" Left X: "));
		Serial.print (left_joystic_x_mapped);
	}

	// Reading from Joystick end here ^^^ ////

	// Motor Control starts here VVV ////

	if (tank_mode) {
		// map to 255 raw
		left_speed_setpoint = left_joystic_y_mapped;
		right_speed_setpoint = right_joystic_y_mapped;
	} else {
		// // use left joystick only
		// int left_speed_raw = left_joystic_y_mapped - left_joystic_x_mapped;
		// int right_speed_raw = left_joystic_y_mapped + left_joystic_x_mapped;
		// if (debug_mode) {
		// 	Serial.print (F(" Speed RAW: Left: "));
		// 	Serial.print (left_speed_raw);
		// 	Serial.print (F(" Right: "));
		// 	Serial.print (right_speed_raw);
		// }
		
		// // determine max value from calculation above
		// int speed_scale = max(abs(left_speed_raw), abs(right_speed_raw));
		// speed_scale = max(255, speed_scale);
		// if (debug_mode) {
		// 	Serial.print (F(" Scale: "));
		// 	Serial.println (speed_scale);
		// }
		
		// left_speed_setpoint = (int) (float) left_speed_raw / (float) speed_scale * 255.0;
		// right_speed_setpoint = (int) (float) right_speed_raw / (float) speed_scale * 255.0;

		// left_speed_setpoint = map(left_speed_setpoint, -255, 255, -maxSpeed, maxSpeed);
		// right_speed_setpoint = map(right_speed_setpoint, -255, 255, -maxSpeed, maxSpeed);

		left_speed_setpoint = map(left_joystic_y_mapped, -255, 255, -100, 100);
		right_speed_setpoint = map(right_joystic_y_mapped, -255, 255, -100, 100);
	}

	setLeftMotor(left_speed_setpoint);
	setRightMotor(right_speed_setpoint);

	// Motor Control end here ^^^ ////

	// Trigger Control starts here VVV ///

	if (trigger_set) {
		digitalWrite(PIN_TRIGGER, HIGH);
		Serial.println (F("Trigger!"));
	} else {
		digitalWrite(PIN_TRIGGER, LOW);
	}

	if (bubble_trigger_set) {
		digitalWrite(PIN_BUBBLE_TRIGGER, HIGH);
		Serial.println (F("Bubble Trigger!"));
	} else {
		digitalWrite(PIN_BUBBLE_TRIGGER, LOW);
	}

	// Trigger Control end here ^^^ ////

	// the delay might not be necessary if we have a lot of process
	delay (1000 / 60);
}
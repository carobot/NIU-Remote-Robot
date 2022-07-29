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
				A0 || D5  - Left Motor IN4
				A1 || D4  - Left Motor IN5
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

/// Joystick Stuff

// This can be changed freely but please see above
const byte PIN_PS2_ATT = 10;

const byte PIN_BUTTONPRESS = A0;
const byte PIN_HAVECONTROLLER = A1;

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

void setLeftMotor(int speed) {

	// TODO: Add in some kind of ramp
	// if (speed <= left_speed) {
	// 	// set motor speed to new speed if speed is lower, safety
	// 	left_speed = speed;
	// } else {
	// 	// ramp up motor speed
	// 	left_speed++;
	// }
	if (abs(speed) > 30) {
		left_speed = speed;
	} else {
		left_speed = 0;
	}

	Serial.print (F("Left: "));

	if (left_speed > 0) {
		// Move forward
		digitalWrite(PIN_IN3, LOW);
    	digitalWrite(PIN_IN4, HIGH);
		Serial.print (F("Forward: "));
	} else if (left_speed < 0) {
		// Move backward
		digitalWrite(PIN_IN3, HIGH);
    	digitalWrite(PIN_IN4, LOW);
		Serial.print (F("Backward: "));
	} else {
		// Stop
		digitalWrite(PIN_IN3, LOW);
    	digitalWrite(PIN_IN4, LOW);
		Serial.print (F("Stop: "));
	}

	analogWrite(PIN_ENB, abs(left_speed));
	Serial.print (abs(left_speed));

}

void setRightMotor(int speed) {
	//See above
	if (abs(speed) > 30) {
		right_speed = speed;
	} else {
		right_speed = 0;
	}

	Serial.print (F(" Right: "));

	if (right_speed > 0) {
		// Move forward
		digitalWrite(PIN_IN1, LOW);
    	digitalWrite(PIN_IN2, HIGH);
		Serial.print (F("Forward: "));
	} else if (right_speed < 0) {
		// Move backward
		digitalWrite(PIN_IN1, HIGH);
    	digitalWrite(PIN_IN2, LOW);
		Serial.print (F("Backward: "));
	} else {
		// Stop
		digitalWrite(PIN_IN1, LOW);
    	digitalWrite(PIN_IN2, LOW);
		Serial.print (F("Stop: "));
	}

	analogWrite(PIN_ENA, abs(right_speed));
	Serial.println (abs(right_speed));
}

/////////////////////////////////////////////////////////////
//                      Main Code                          //
/////////////////////////////////////////////////////////////

PsxControllerHwSpi<PIN_PS2_ATT> psx;

boolean haveController = false;

boolean debug_mode = false;

void setup () {
	fastPinMode (PIN_BUTTONPRESS, OUTPUT);
	fastPinMode (PIN_HAVECONTROLLER, OUTPUT);

	fastPinMode (PIN_ENA, OUTPUT);
	fastPinMode (PIN_IN1, OUTPUT);
	fastPinMode (PIN_IN2, OUTPUT);
	fastPinMode (PIN_ENB, OUTPUT);
	fastPinMode (PIN_IN3, OUTPUT);
	fastPinMode (PIN_IN4, OUTPUT);

	fastPinMode (PIN_TRIGGER, OUTPUT);
	
	delay (300);

	Serial.begin (115200);
	Serial.println (F("Ready!"));
}
 
void loop () {
	static byte slx, sly, srx, sry;
	static int left_speed_setpoint, right_speed_setpoint;
	byte trigger_set = 0;

	// Reading from Joystick start here VVV ////
	
	// DO NOT MODIFY FROM HERE to ... //
	fastDigitalWrite (PIN_HAVECONTROLLER, haveController);
	
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

			fastDigitalWrite (PIN_BUTTONPRESS, !!psx.getButtonWord ());
			if (debug_mode) dumpButtons (psx.getButtonWord ());

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

	// Reading from Joystick end here ^^^ ////

	// Motor Control starts here VVV ////

	left_speed_setpoint = map(sly, 0, 255, 255, -255);
	right_speed_setpoint = map(sry, 0, 255, 255, -255);

	setLeftMotor(left_speed_setpoint);
	setRightMotor(right_speed_setpoint);

	trigger_set = (psx.getButtonWord () & 0x2000) ? true : false;
	//Serial.println (psx.getButtonWord ());

	if (trigger_set) {
		digitalWrite(PIN_TRIGGER, HIGH);
		Serial.println (F("Trigger!"));
	} else {
		digitalWrite(PIN_TRIGGER, LOW);
	}

	// Motor Control end here ^^^ ////

	// the delay might not be necessary if we have a lot of process
	delay (1000 / 60);
}
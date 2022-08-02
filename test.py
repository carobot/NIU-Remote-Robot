import curses
from nanpy import ArduinoApi
from nanpy import SerialManager

screen = curses.initscr()
curses.noecho()
curses.cbreak()
screen.keypad(True)

enA = 9
in1 = 8
in2 = 7
enB = 6
in3 = 5
in4 = 4
trig = 10

try:
    connection = SerialManager(device='COM7')
    a = ArduinoApi(connection=connection)
except:
    print("Failed to connect to Arduino")

# Setup the pinModes

a.pinMode(enA, a.OUTPUT)
a.pinMode(enB, a.OUTPUT)
a.pinMode(in1, a.OUTPUT)
a.pinMode(in2, a.OUTPUT)
a.pinMode(in3, a.OUTPUT)
a.pinMode(in4, a.OUTPUT)
a.pinMode(trig, a.OUTPUT)



try:
    while True:
        char = screen.getch()
        if char == ord('q'):
            break
        elif char == curses.KEY_UP:
            print ("up")
            a.digitalWrite(in1,a.HIGH)
            a.digitalWrite(in2, a.LOW)
            a.analogWrite(enA,50)
            a.digitalWrite(in3,a.HIGH)
            a.digitalWrite(in4, a.LOW)
            a.analogWrite(enB,50)

        elif char == curses.KEY_DOWN:
            print ("down")
            a.digitalWrite(in1,a.LOW)
            a.digitalWrite(in2, a.HIGH)
            a.analogWrite(enA,50)
            a.digitalWrite(in3,a.LOW)
            a.digitalWrite(in4, a.HIGH)
            a.analogWrite(enB,50)
        elif char == curses.KEY_RIGHT:
            print ("right")
            a.digitalWrite(in1,a.HIGH)
            a.digitalWrite(in2, a.LOW)
            a.analogWrite(enA,50)
        elif char == curses.KEY_LEFT:
            print ("left")
            a.digitalWrite(in3,a.HIGH)
            a.digitalWrite(in4, a.LOW)
            a.analogWrite(enB,50)
        elif char == ord('s'):
            print ("shoot")
            a.digitalWrite(trig,a.HIGH)
        else:
            print("robot inactive")
            a.digitalWrite(in1, a.LOW)
            a.digitalWrite(in2, a.LOW)
            a.analogWrite(enA, 0)
            a.digitalWrite(in3, a.LOW)
            a.digitalWrite(in4, a.LOW)
            a.analogWrite(enB, 0)
            a.digitalWrite(trig,a.LOW)

finally:
    curses.nocbreak(); screen.keypad(0); curses.echo()
    curses.endwin()

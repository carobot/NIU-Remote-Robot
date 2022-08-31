This is a short description of the creation of the physical robot used for the CAROBOT Night It Up 2022 Remote Robot. 

The frame of the robot, as well as 2 wheels and motors was taken from a ride-on children’s car. A caster wheel was screwed into a piece of plywood. Then the plywood was screwed to the plastic frame such that the caster replaced the two front wheels as seen below. 

![car](https://github.com//carobot/NIU-Remote-Robot/PXL_20220630_161905990(2).jpg?raw=true)

Additional plywood was then added to the interior of the robot to add structural integrity and provide locations to mount the electronics. 

Additional frames from the original car were added as supports which zip tied the frame of the doll to such that it stood up straight as seen below.

The electronics portion of the robot was all held within the main cavity of the car. The Robot is powered by a 12V lead acid car battery. For power distribution, we used a power and ground rail to connect between components. For the devices that require lower voltages, such as the raspberry pi and arduino, we used a voltage regulator. The 15A fuses were added in series with each of the motors to prevent damage in case of a short. 

The electrical components used in this project are:
1 Raspberry Pi 4
1 Arduino Uno
1 pc case fan
1 voltage regulator
1 radio receiver
1 Motor Controller (30 Amp)
2 120W DC motors 
2 15 Amp fuses
3 USB cameras
1 wireless game controller
1 power switch
1 Phone with data plan


The phone provides a mobile hotspot which connects the raspberry pi to the internet. This allows the raspberry pi to interface with a database to receive controls, and stream the USB cameras. The Arduino is connected to the raspberry pi through serial, this allows the raspberry pi to relay the controls to the Arduino. 

The Arduino has two inputs: the radio receiver and the raspberry pi. The program on the Arduino determines how the robot should move for a set of inputs. The Arduino then outputs PWM and digital signals to the Motor Controller which controls the speed and direction of the robot.

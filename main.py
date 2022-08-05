from nanpy import (ArduinoApi, SerialManager)

try:
    connection = SerialManager(device='COM7')
    a = ArduinoApi(connection)
except:
    print("Failed to connect to Arduino")
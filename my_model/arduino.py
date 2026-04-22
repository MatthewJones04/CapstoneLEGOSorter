# Simple script to test serial communication with Arduino
import serial
import time

ser = serial.Serial('COM5', 115200, timeout=1)  # change COM3 if needed
time.sleep(2)  # wait for Arduino to reset

# ser.write(b'ON\n')     # send string
# time.sleep(5)
ser.write(b'OFF\n')

line = ser.readline().decode('utf-8').strip()
print(line)


ser.close()

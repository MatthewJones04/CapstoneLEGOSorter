from time import time
import serial

# Initialize serial communication with Arduino (ENSURE COM PORT/BAUD RATE IS SET CORRECTLY)
ser = serial.Serial('COM5', 115200, timeout=1)  # change COM3 if needed
time.sleep(2)  # wait for Arduino to reset

# Get response from user
while True:
    response = input("Enter code to be received by Arduino (or 'q' to quit): ")
    if response == 'q':
        break
    else:
        # Send response to Arduino
        # ser.write(response.encode())
        print(f"Sent to Arduino: {response}")
        ser.write(response.encode())
        
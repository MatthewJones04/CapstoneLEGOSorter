# Simple camera test using OpenCV

import cv2

cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)  # DirectShow = better on Windows

if not cap.isOpened():
    print("Camera not detected")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Frame failed")
        break

    cv2.imshow("Camera Test", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
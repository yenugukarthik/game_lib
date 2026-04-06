import cv2
import mediapipe as mp
import os

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=1)
mp_draw = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)

tip_ids = [8, 12, 16, 20]  # ignore thumb

stop_file = "stop_signal"

while True:
    # allow external program to stop this loop by creating the stop file
    if os.path.exists(stop_file):
        break

    success, img = cap.read()
    if not success:
        continue
    img = cv2.flip(img, 1)
    rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb)

    count = 0

    if results.multi_hand_landmarks:
        for handLms in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(img, handLms, mp_hands.HAND_CONNECTIONS)

            h, w, c = img.shape

            landmarks = []
            for lm in handLms.landmark:
                landmarks.append((int(lm.x * w), int(lm.y * h)))

            for tip in tip_ids:
                if landmarks[tip][1] < landmarks[tip - 2][1]:
                    count += 1

    if count not in [1, 2, 3, 4]:
        count = 0

    # print with flush so external readers get real-time values
    print(count, flush=True)

    cv2.putText(img, str(count), (50, 150),
                cv2.FONT_HERSHEY_SIMPLEX,
                4, (0, 255, 0), 8)

    cv2.imshow("Finger Counter", img)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()

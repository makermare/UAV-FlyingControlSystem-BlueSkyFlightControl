import cv2

def imshow_ssh(frame):
    height, width = frame.shape[:2]
    size = 320/width
    cv2.imshow("image", cv2.resize(frame, None, fx = size, fy = size))
    cv2.waitKey(10)

def get_camera_para(capture):
    print(
        'width:', capture.get(cv2.CAP_PROP_FRAME_WIDTH),
        'height:', capture.get(cv2.CAP_PROP_FRAME_HEIGHT),
        'fps:', capture.get(cv2.CAP_PROP_FPS),
        'brightness:', capture.get(cv2.CAP_PROP_BRIGHTNESS),
        'contrast:', capture.get(cv2.CAP_PROP_CONTRAST),
        'saturation:', capture.get(cv2.CAP_PROP_SATURATION),
        'hue:', capture.get(cv2.CAP_PROP_HUE),
        'exposure:', capture.get(cv2.CAP_PROP_EXPOSURE),
        '', capture.get(cv2.CAP_PROP_FRAME_COUNT),
    )

if __name__ == "__main__":
    cam = cv2.VideoCapture(1)

    if not cam.isOpened():
        print("Error: Can't open camera.")
    else:
        cam.set(cv2.CAP_PROP_FPS, 30)
        get_camera_para(cam)
        while True:
            ret, frame = cam.read()
            imshow_ssh(frame)
            print(frame.shape)

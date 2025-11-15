#
# Testing the datalink using separated process
#
import sys
sys.path.append("..")
from python.app.Datalink.src.datalink import Datalink
import time
import numpy as np
import cv2

def main ():
    link = Datalink(port=21001, timeout=2)

    print ("waiting for client to connect")
    while not link.is_connected():
        time.sleep(0.01)

    image = cv2.imread("./img_1.png")
    image = np.asarray(image)

    pos = 1
    while True:
        if not link.is_connected():
            time.sleep(0.01)
            continue
        
        print (f"sending {pos}")
        link.write_uint8_np(image)
        res, _ = link.read()
        if (res == "ack"):
            exit(0)


if __name__ == "__main__":
    main()
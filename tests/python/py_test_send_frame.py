#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time
import numpy as np
import cv2
import argparse

def main (host: str, port: int):
    link = Datalink(host=host, port=port, timeout=2)

    # print ("waiting for client to connect")
    # while not link.is_connected():
    #     time.sleep(0.01)

    image = cv2.imread("./img_1.png")
    image = np.asarray(image)

    pos = 1
    while not link.is_connected():
        time.sleep(0.01)
        continue
        
    print (f"sending {pos}")
    link.write_uint8_np(image)
    res, _ = link.read()
    if (res == "ack"):
        print ("acknowledge received")
        exit(0)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", "-H", default="127.0.0.1", help="datalink host")
    parser.add_argument("--port", "-p", type=int, default=21001, help="datalink port")
    args = parser.parse_args()
    main(args.host, args.port)
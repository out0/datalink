#
# Testing the datalink using separated process
#
import sys
from pydatalink import Datalink 
import time
import cv2
import argparse

def main (port: int):
    link = Datalink(port=port, timeout=2)

    while not link.is_connected():
        time.sleep(0.01)
        
    print ("A client just connected. We will receive the frame")

    arr = link.read_uint8_np((400, 400, 3))
    print (f"Data received with a shape: {arr.shape}")

    print ("Sending the acknowledge")
    link.write("ack")
    cv2.imwrite("received_img.png", arr)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", "-p", type=int, default=21001, help="datalink port")
    args = parser.parse_args()
    main(args.port)
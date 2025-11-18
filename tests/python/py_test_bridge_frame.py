#
# Testing the datalink using separated process
#
import sys
from pydatalink import Databridge
import time
import cv2
import argparse


if __name__ == "__main__":
    link = Databridge(in_port=21001, out_port=21002, timeout=2)
    input()
    del link

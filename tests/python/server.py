#! /usr/bin/python3 
#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time
import numpy as np

def main ():
    link = Datalink(port=20000, timeout=1000)
    payload = np.full((1024, 1024), fill_value=10, dtype=np.int8)
    #payload = "A" * 1024 * 1024

    while True:
        if not link.is_ready():
            print ("waiting for client to connect")
            while not link.is_ready():
                time.sleep(0.01)
        
        print("sending...")
        link.write(payload)
        #time.sleep(0.5)


if __name__ == "__main__":
    main()
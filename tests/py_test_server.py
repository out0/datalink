#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time
import numpy as np

def main ():
    link = Datalink(port=21001, timeout=2)

    print ("waiting for client to connect")
    while not link.is_connected():
        time.sleep(0.01)

    pos = 1
    while True:
        if not link.is_connected():
            time.sleep(0.01)
            continue
        
        print (f"sending {pos}")
        link.write(f"data #{pos}")
        pos += 1
        time.sleep(0.5)


if __name__ == "__main__":
    main()
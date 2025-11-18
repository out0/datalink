#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time
import numpy as np

def main ():
    link = Datalink(port=20000, timeout=2)

    pos = 1
    while True:
        if not link.is_connected():
            print ("waiting for client to connect")
            while not link.is_connected():
                time.sleep(0.01)
        
        print (f"sending {pos}")
        link.write(f"data #{pos}")
        pos += 1
        _, sz = link.read()
        print (f"received: {sz} bytes")
        #time.sleep(0.5)


if __name__ == "__main__":
    main()
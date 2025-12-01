#! /usr/bin/python3 
from pydatalink import Datalink
import time
import numpy as np
import faulthandler
faulthandler.enable()

def main ():
    link = Datalink(host="127.0.0.1", port=20000, timeout=1000)

    while True:
        if not link.is_ready():
            print ("waiting for the server to be ready")
            while not link.is_ready():
                time.sleep(0.01)
        
        if link.has_data():
            rcv, sz = link.read_np((100, 100, 3), dtype=np.int8)
            print (f"received {sz} bytes")
            #rcv, sz = link.read_bytes()
            
            # print("[")
            # for i in range(0, 10):
            #     print (f" {rcv[i]}", end="")
            # print(" ]")
            #rcv, sz = link.read_np((1000, 1000, 3), dtype=np.float32)
            # if sz == 0:
            #     continue
            # print (f"received: {sz} bytes")
            # print (f"array shape: {rcv.shape}")

if __name__ == "__main__":
    main()
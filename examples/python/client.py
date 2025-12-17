#! /usr/bin/python3 
from pydatalink import Datalink
import time
import numpy as np
import faulthandler
faulthandler.enable()

def main ():
    link = Datalink(host="127.0.0.1", port=21000, timeout=1000, max_incommming_messages_in_queue=-1)

    i = 0
    while True:
        if not link.is_ready():
            print ("waiting for the server to be ready")
            while not link.is_ready():
                time.sleep(0.01)
        
        if link.has_data():
            #rcv, sz = link.read_bytes()
            rcv, sz, timestap = link.read_np((1024, 1024), dtype=np.int8)
            if sz == 0:
                continue
            print (f"received {sz} bytes")
            #rcv, sz = link.read_bytes()

            link.write_keep_alive()
            
            # print("[")
            # for i in range(0, 10):
            #     print (f" {rcv[i]}", end="")
            # print(" ]")
            #rcv, sz = link.read_np((1000, 1000, 3), dtype=np.float32)
            # if sz == 0:
            #     continue
            # print (f"received: {sz} bytes")
            # print (f"array shape: {rcv.shape}")
            # if i > 10:
            #     break
            # i += 1

if __name__ == "__main__":
    main()
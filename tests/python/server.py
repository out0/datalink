#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time
import numpy as np

def main ():
    link = Datalink(port=20000, timeout_ms=2000)

    SRV_MSG = "B" * ((1024 ** 2) * 2)
    SRV_SZ = len(SRV_MSG)

    while True:
        if not link.is_connected():
            print ("waiting for client to connect")
            while not link.is_connected():
                time.sleep(0.01)
        
        print ("client connected")
        while link.is_connected():
            _, sz = link.read_str()
            print (f"message received with {sz} bytes")

            print (f"sending response with {SRV_SZ} bytes")
            link.write(SRV_MSG)
            pass

        print ("client disconnected")

    # pos = 1

    # send_data = np.full((300, 300), fill_value=7.0, dtype=np.float32)
    

    # while True:
    #     if not link.is_connected():
    #         print ("waiting for client to connect")
    #         while not link.is_connected():
    #             time.sleep(0.01)
        
    #     print (f"sending the array data")
    #     link.write_float_np(send_data)
    #     pos += 1

    #     print (f"waiting data from the client")
    #     _, sz = link.read()
    #     print (f"received: {sz} bytes")
    #     #time.sleep(0.5)


if __name__ == "__main__":
    main()
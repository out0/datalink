#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time

SRV_MSG = "A" * ((1024 ** 2) * 2)
SRV_SZ = len(SRV_MSG)


def main ():
    link = Datalink(host="127.0.0.1", port=20000, timeout_ms=3000)

    while True:
        print ("connecting to the server")
        while not link.is_connected():
            time.sleep(0.01)
        
        print ("connected to the server")
        while link.is_connected():
            print (f"sending message back with {SRV_SZ} bytes")
            link.write(SRV_MSG)

            _, sz = link.read_str()
            print (f"response received with {sz} bytes")


        print ("the server is off")

    # generate a 10 MB ASCII string
    # len_payload = (10 * 1024 * 1024)
    # payload = "A" * (len_payload)  # 10 * 1024 * 1024 bytes
    # print(f"generated payload of {len(payload)} bytes")

    # pos = 1
    # while True:
    #     if not link.is_connected():
    #         print ("waiting for the server to be ready")
    #         while not link.is_connected():
    #             time.sleep(0.01)
        
    #     print ("receiving data from the server...")
    #     rcv = link.read_float_np(shape=(300, 300))
    #     print (f"received: array with shape: {rcv.shape}")

    #     print (f"sending {len_payload} bytes")
    #     link.write(payload)
    #     pos += 1
    #     #time.sleep(0.5)



if __name__ == "__main__":
    main()
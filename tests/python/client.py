#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time

def main ():
    link = Datalink(host="127.0.0.1", port=20000, timeout=2)

    # generate a 10 MB ASCII string
    len_payload = (10 * 1024 * 1024)
    payload = "A" * (len_payload)  # 10 * 1024 * 1024 bytes
    print(f"generated payload of {len(payload)} bytes")

    pos = 1
    while True:
        if not link.is_connected():
            print ("waiting for the server to be ready")
            while not link.is_connected():
                time.sleep(0.01)
        
        rcv, sz = link.read()
        print (f"received: {sz} bytes")

        print (f"sending {len_payload} bytes")
        link.write(payload)
        pos += 1
        #time.sleep(0.5)



if __name__ == "__main__":
    main()
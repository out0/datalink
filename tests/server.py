#
# Testing the datalink using separated process
#
from pydatalink import Datalink
import time

def main ():
    link = Datalink(port=20000, timeout=-1)

    pos = 1
    while True:
        if not link.is_ready():
            print ("waiting for client to connect")
            while not link.is_ready():
                time.sleep(0.01)
        
        # print (f"sending {pos}")
        # link.write(f"data #{pos}")
        # pos += 1
        _, sz = link.read()
        print (f"received: {sz} bytes")
        time.sleep(0.5)


if __name__ == "__main__":
    main()
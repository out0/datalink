from pydatalink import Datalink
import time

def main ():
    link = Datalink(host="127.0.0.1", port=20000, timeout=5000)
    len_payload = 10 * 1024 * 1024
    payload = "A" * len_payload  # 10 * 1024 * 1024 bytes
    print(f"generated payload of {len(payload)} bytes")

    while True:
        if not link.is_ready():
            print ("waiting for the server to be ready")
            while not link.is_ready():
                time.sleep(0.01)
        
        # rcv, sz = link.read()
        # print (f"received: {sz} bytes")

        print (f"sending {len_payload} bytes")
        link.write(payload)
        time.sleep(0.5)

if __name__ == "__main__":
    main()
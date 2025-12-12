from pydatalink import Datalink, Databridge

def main ():
    left = Datalink(port=20000, timeout=100)
    right = Datalink(host="127.0.0.1", port=21000, timeout=100)

    bridge = Databridge(left, right)

    print ("press enter to terminate...")
    input()


if __name__ == "__main__":
    main()
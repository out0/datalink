from pydatalink import Datalink
import unittest, time


class TestDataLinkReadWrite(unittest.TestCase):
    
    _exec_start: float

    def execute_start(self) -> None:
        self._exec_start = time.time()
    
    def execute_end(self, msg: str) -> None:
        t = 1000*(time.time() - self._exec_start)
        print(f"{msg} executed in {t:.2f} ms")


    def test_read_write_with_timeout(self):
        client = Datalink(host="127.0.0.1", port=20101, timeout=50)
        server = Datalink(port=20101, timeout=50)

        start = time.time()
        max_ms = 1000
        total_exec_time = -1

        while True:
            if client.is_ready() and server.is_ready():
                break
            total_exec_time = 1000*(time.time() - start)
            if total_exec_time > max_ms:
                self.fail("timeout connecting server and client")
                return
        
        start = time.time()
        size = 1024 * 1024
        payload = "A" * size
        self.assertTrue(client.write(payload))

        while not server.has_data():
            time.sleep(0.01)

        data, sz = server.read()
        total_exec_time = 1000*(time.time() - start)
        print (f"data transfer executed in {total_exec_time:.2f} ms")
        self.assertEqual(size, sz)
        for i in range(size):
            if data[i] != payload[i]:
                self.fail("data mismatch")

        print("END 2")


if __name__ == "__main__":
    unittest.main()
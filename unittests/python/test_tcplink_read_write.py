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
        self.assertTrue(client.write(payload, 123.45))

        while not server.has_data():
            time.sleep(0.01)

        data, sz, timestamp = server.read()
        total_exec_time = 1000*(time.time() - start)
        print (f"data transfer executed in {total_exec_time:.2f} ms")
        self.assertEqual(size, sz)
        self.assertAlmostEqual(timestamp, 123.45, places=2)
        for i in range(size):
            if data[i] != payload[i]:
                self.fail("data mismatch")

        print("END 2")

    def test_double_read_without_data_repeat(self):
        server = Datalink(port=20011, timeout=50)
        client = Datalink(host="127.0.0.1", port=20011, timeout=50)

        max_loops = 1000
        while max_loops >= 0:
            if server.is_ready() and client.is_ready():
                break
            time.sleep(0.001)
            max_loops -= 1

        if max_loops <= 0:
            self.fail("Timeout while trying to connect")

        size = 1024 * 1024  # 1 MB
        payload = bytes([i % 100 for i in range(size)])

        self.execute_start()
        self.assertTrue(client.write(payload, 123.45))
        self.execute_end("1MB data send client->server")

        while not server.has_data():
            time.sleep(0.001)
        
        self.execute_start()
        data, timestamp = server.recv_object()
        self.execute_end("1MB data recv client->server")

        self.assertEqual(sz, size)
        self.assertAlmostEqual(timestamp, 123.45, places=2)
        self.assertEqual(data, payload)

        self.assertFalse(server.has_data())
        data2 = server.recv_object()
        self.assertEqual(timestamp2, 0.0)
        self.assertEqual(sz2, 0)


if __name__ == "__main__":
    unittest.main()
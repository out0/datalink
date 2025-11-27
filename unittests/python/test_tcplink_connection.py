from pydatalink import Datalink
import unittest, time


class TestDataLink(unittest.TestCase):
    
    _exec_start: float

    def execute_start(self) -> None:
        self._exec_start = time.time()
    
    def execute_end(self, msg: str) -> None:
        t = 1000*(time.time() - self._exec_start)
        print(f"{msg} executed in {t:.2f} ms")


    def test_tcp_connect_no_timeout(self):
        client = Datalink(host="127.0.0.1", port=20000, timeout=-1)
        server = Datalink(port=20000, timeout=-1)

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
        
        total_exec_time = 1000*(time.time() - start)

        print (f"connection executed in {total_exec_time:.2f} ms")

    def test_tcp_connect_with_timeout(self):
        client = Datalink(host="127.0.0.1", port=20001, timeout=10)
        server = Datalink(port=20001, timeout=10)

        start = time.time()
        max_ms = 10
        total_exec_time = -1

        while True:
            if client.is_ready() and server.is_ready():
                break
            total_exec_time = 1000*(time.time() - start)
            if total_exec_time > max_ms:
                self.fail("timeout connecting server and client")
                return
        
        total_exec_time = 1000*(time.time() - start)

        print (f"connection executed in {total_exec_time:.2f} ms")

    def test_tcp_connect_no_timeout_fail(self):
        client = Datalink(host="127.0.0.1", port=20002, timeout=-1)
        server = Datalink(port=20000, timeout=-1)

        start = time.time()
        max_ms = 100
        total_exec_time = -1

        while True:
            if client.is_ready() and server.is_ready():
                self.fail("Server and client connected, but they shouldn't.")
                return
            
            total_exec_time = 1000*(time.time() - start)
            if total_exec_time > max_ms:
                return
            
    def test_tcp_connect_with_timeout_fail(self):
        client = Datalink(host="127.0.0.1", port=20002, timeout=50)
        server = Datalink(port=20000, timeout=50)

        start = time.time()
        max_ms = 100
        total_exec_time = -1

        while True:
            if client.is_ready() and server.is_ready():
                self.fail("Server and client connected, but they shouldn't.")
                return
            
            total_exec_time = 1000*(time.time() - start)
            if total_exec_time > max_ms:
                return
        

if __name__ == "__main__":
    unittest.main()
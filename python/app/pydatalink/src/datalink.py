import ctypes
import os
import numpy as np
import time


FLOAT_SIZE_BYTES = 4  # sizeof_float


FLOAT_32 = 1
FLOAT_64 = 2
INT_32 = 3
INT_64 = 4
INT_8 = 5
UINT_8 = 6
UNSUPPORTED = 7


class Datalink:
    ''' Creates a TCP link between two endpoints
    '''
    link: ctypes.c_void_p

    def __init__(self, host: str = None, port: int = -1, timeout: float = -1) -> None:
        if port <= 0:
            raise Exception("port should be defined")

        Datalink.setup_cpp_lib()

        if host is None:
            self.link = Datalink.lib.init_tcp_server(
                ctypes.c_int(port), ctypes.c_float(timeout))
        else:
            self.link = Datalink.lib.init_tcp_client(ctypes.c_char_p(
                host.encode('utf-8')), ctypes.c_int(port), ctypes.c_float(timeout))

    @classmethod
    def setup_cpp_lib(cls) -> None:
        if hasattr(Datalink, "lib"):
            return

        lib_path = os.path.join(os.path.dirname(
            __file__), "../cpp", "libdatalink.so")

        Datalink.lib = ctypes.CDLL(lib_path)

        Datalink.lib.init_tcp_server.restype = ctypes.c_void_p
        Datalink.lib.init_tcp_server.argtypes = [ctypes.c_int]

        Datalink.lib.init_tcp_client.restype = ctypes.c_void_p
        Datalink.lib.init_tcp_client.argtypes = [ctypes.c_char_p, ctypes.c_int]

        Datalink.lib.destroy_tcp_link.restype = None
        Datalink.lib.destroy_tcp_link.argtypes = [ctypes.c_void_p]

        Datalink.lib.write_raw_data.restype = ctypes.c_bool
        Datalink.lib.write_raw_data.argtypes = [ctypes.c_void_p,
                                                ctypes.c_char_p, ctypes.c_long, ctypes.c_double]

        Datalink.lib.is_ready.restype = ctypes.c_bool
        Datalink.lib.is_ready.argtypes = [ctypes.c_void_p]

        Datalink.lib.next_message_size.restype = ctypes.c_long
        Datalink.lib.next_message_size.argtypes = [ctypes.c_void_p]


        Datalink.lib.read_next_message.restype = ctypes.c_long
        Datalink.lib.read_next_message.argtypes = [
            ctypes.c_void_p, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_long, ctypes.POINTER(ctypes.c_double)]

        Datalink.lib.has_data.restype = ctypes.c_bool
        Datalink.lib.has_data.argtypes = [ctypes.c_void_p]

    def __del__(self):
        Datalink.lib.destroy_tcp_link(self.link)

    def is_ready(self) -> bool:
        return Datalink.lib.is_ready(self.link)

    def has_data(self) -> bool:
        return Datalink.lib.has_data(self.link)

    def read(self) -> tuple[str, int, float]:
        raw_data, size, timestamp = self.read_bytes()
        data = raw_data.decode('utf-8')

        # Datalink.lib.free_memory(result_ptr)
        return data, size, timestamp

    def read_bytes(self) -> tuple[bytes, int, float]:

        raw_size = Datalink.lib.next_message_size(self.link)

        pointer_type = ctypes.POINTER(ctypes.c_ubyte)        
        data_block = (ctypes.c_ubyte * raw_size)()
        data_pointer = ctypes.cast(ctypes.addressof(data_block), pointer_type)
        timestamp = ctypes.c_double()
        read_size = Datalink.lib.read_next_message(self.link, data_pointer, raw_size, ctypes.byref(timestamp))

        if read_size == 0:
            return bytes(), 0, 0

        if read_size != raw_size:
            raise "Read size does not match expected size"

        return bytes(data_block), read_size, timestamp.value


    def __write_np(self, data: np.array, timestamp: float = -1) -> bool:
        raw_data = data.tobytes()
        size = len(raw_data)
        if timestamp < 0:
            timestamp = time.time()
        return Datalink.lib.write_raw_data(
                self.link,
                ctypes.c_char_p(raw_data),
                ctypes.c_long(size),
                timestamp)

    def write(self, data: any, timestamp: float = -1) -> bool:
        if isinstance(data, np.ndarray):
            return self.__write_np(data)

        else:
            if isinstance(data, str):
                msg = data
            else:
                msg = str(data)

            if timestamp < 0:
                timestamp = time.time()

            return Datalink.lib.write_raw_data(
                self.link,
                ctypes.c_char_p(msg.encode('utf-8')),
                ctypes.c_long(len(data)),
                timestamp)
    def _shape_size(self, shape) -> int:
        size = 1
        for dim in shape:
            size *= dim
        return size

    def read_np(self, shape, dtype) -> tuple[np.ndarray, int, float]:
        raw_data, size, timestamp = self.read_bytes()
        if size == 0:
            return np.array([]), 0, 0 
        expected_size = self._shape_size(shape) * np.dtype(dtype).itemsize
        if size !=expected_size:
            print(f"Data size does not match expected size: {size} != {expected_size}")
            return np.array([]), 0, 0 
        
        new_arr = np.frombuffer(raw_data, dtype=dtype)
        return new_arr.reshape(shape), size, timestamp

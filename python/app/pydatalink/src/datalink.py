import ctypes, os
import numpy as np


FLOAT_SIZE_BYTES = 4  # sizeof_float

class Datalink:
    ''' Creates a TCP link between two endpoints
    '''
    link: ctypes.c_void_p
    _timeout: float

    def __init__(self, host: str = None, port: int = -1, timeout: float = -1) -> None:
        if port <= 0:
            raise Exception("port should be defined")
           
        Datalink.setup_cpp_lib()

        if host is None:
            self.link = Datalink.lib.init_server(
                ctypes.c_int(port), ctypes.c_float(timeout))
        else:
            self.link = Datalink.lib.init_client(ctypes.c_char_p(
                host.encode('utf-8')), ctypes.c_int(port), ctypes.c_float(timeout))
            
        self._timeout = timeout


    @classmethod
    def setup_cpp_lib(cls) -> None:
        if hasattr(Datalink, "lib"):
            return
        
        lib_path = os.path.join(os.path.dirname(__file__), "../cpp", "libdatalink.so")

        Datalink.lib = ctypes.CDLL(lib_path)

        Datalink.lib.init_server.restype = ctypes.c_void_p
        Datalink.lib.init_server.argtypes = [ctypes.c_int]

        Datalink.lib.init_client.restype = ctypes.c_void_p
        Datalink.lib.init_client.argtypes = [ctypes.c_char_p, ctypes.c_int]

        Datalink.lib.destroy_link.restype = None
        Datalink.lib.destroy_link.argtypes = [ctypes.c_void_p]

        Datalink.lib.write_str_link.restype = ctypes.c_bool
        Datalink.lib.write_str_link.argtypes = [ctypes.c_void_p,
                                    ctypes.c_char_p, ctypes.c_long]

        Datalink.lib.write_np_float_link.restype = ctypes.c_bool
        Datalink.lib.write_np_float_link.argtypes = [ctypes.c_void_p, np.ctypeslib.ndpointer(
            dtype=ctypes.c_float, ndim=1), ctypes.c_size_t]

        Datalink.lib.write_np_uint8_link.restype = ctypes.c_bool
        Datalink.lib.write_np_uint8_link.argtypes = [ctypes.c_void_p, np.ctypeslib.ndpointer(
            dtype=ctypes.c_uint8, ndim=1), ctypes.c_size_t]


        Datalink.lib.has_data.restype = ctypes.c_bool
        Datalink.lib.has_data.argtypes = [ctypes.c_void_p]

        Datalink.lib.is_connected.restype = ctypes.c_bool
        Datalink.lib.is_connected.argtypes = [ctypes.c_void_p]

        Datalink.lib.is_listening.restype = ctypes.c_bool
        Datalink.lib.is_listening.argtypes = [ctypes.c_void_p]

        # ctypes.POINTER(ctypes.c_char_p)
        Datalink.lib.read_str_link.restype = ctypes.POINTER(ctypes.c_char_p)
        Datalink.lib.read_str_link.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long), ctypes.c_float]

        Datalink.lib.read_np_float_link.restype = ctypes.POINTER(ctypes.c_float)
        Datalink.lib.read_np_float_link.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long), ctypes.c_float]

        Datalink.lib.free_memory.argtypes = [ctypes.c_void_p]
        Datalink.lib.free_memory.restype = None

    def __del__(self):
        Datalink.lib.destroy_link(self.link)


    def write(self, data: any) -> bool:

        if data is None: return

        if isinstance(data, str):
            return Datalink.lib.write_str_link(
                self.link,
                ctypes.c_char_p(data.encode('utf-8')),
                ctypes.c_long(len(data)))
        elif isinstance(data, np.ndarray):
            size = 1
            for i in range(len(data.shape)):
                size = size * data.shape[i]
            
            if data.dtype == np.uint8:
                Datalink.lib.write_np_uint8_link(
                    self.link,
                    np.ascontiguousarray(data.reshape((size))),
                    size)
            elif data.dtype == np.float32:
                Datalink.lib.write_np_float_link(
                    self.link,
                    np.ascontiguousarray(data.reshape((size))),
                    size)
            else:
                raise TypeError("we only support data dtype np.float32 or np.uint8")

        else:
            self.write(str(data))

    def has_data(self) -> bool:
        return Datalink.lib.has_data(self.link)

    def is_connected(self) -> bool:
        return Datalink.lib.is_connected(self.link)

    def is_listening(self) -> bool:
        return Datalink.lib.is_listening(self.link)

    def read_str(self) -> tuple[str, int]:
        size = ctypes.c_long(0)
        result_ptr = Datalink.lib.read_str_link(self.link, ctypes.byref(size), self._timeout)
        data: str = None
        try:
            data = ctypes.cast(
                result_ptr, ctypes.c_char_p).value.decode('utf-8')
        except:
            data = ""

        Datalink.lib.free(result_ptr)
        return data, size.value

    def read_float_np(self, shape: tuple) -> np.ndarray:
        size = ctypes.c_long(0)
        result_ptr = Datalink.lib.read_np_float_link(self.link, ctypes.byref(size), self._timeout)
        data = np.zeros(size, dtype=np.float32)
        for i in range(size):
            data[i] = float(result_ptr[i])
        r = data.reshape(shape)
        Datalink.lib.free(result_ptr)
        return r

    def read_uint8_np(self, shape: tuple) -> np.ndarray:
        size = ctypes.c_long(0)
        result_ptr = Datalink.lib.read_str_link(self.link, ctypes.byref(size), self._timeout)
        data = np.zeros(size, dtype=np.uint8)
        for i in range(size):
            data[i] = int(result_ptr[i])
        r = data.reshape(shape)
        Datalink.lib.free(result_ptr)
        return r
       
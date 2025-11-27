import ctypes, os
import numpy as np


FLOAT_SIZE_BYTES = 4  # sizeof_float

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
        
        lib_path = os.path.join(os.path.dirname(__file__), "../cpp", "libdatalink.so")

        Datalink.lib = ctypes.CDLL(lib_path)

        Datalink.lib.init_tcp_server.restype = ctypes.c_void_p
        Datalink.lib.init_tcp_server.argtypes = [ctypes.c_int]

        Datalink.lib.init_tcp_client.restype = ctypes.c_void_p
        Datalink.lib.init_tcp_client.argtypes = [ctypes.c_char_p, ctypes.c_int]

        Datalink.lib.destroy_tcp_link.restype = None
        Datalink.lib.destroy_tcp_link.argtypes = [ctypes.c_void_p]

        Datalink.lib.write_str_link.restype = ctypes.c_bool
        Datalink.lib.write_str_link.argtypes = [ctypes.c_void_p,
                                    ctypes.c_char_p, ctypes.c_long]

        Datalink.lib.is_ready.restype = ctypes.c_bool
        Datalink.lib.is_ready.argtypes = [ctypes.c_void_p]


        Datalink.lib.next_message.restype = ctypes.POINTER(ctypes.c_char_p)
        Datalink.lib.next_message.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long)]

        Datalink.lib.free_memory.argtypes = [ctypes.c_void_p]
        Datalink.lib.free_memory.restype = None


        Datalink.lib.has_data.restype = ctypes.c_bool
        Datalink.lib.has_data.argtypes = [ctypes.c_void_p]

    def __del__(self):
        Datalink.lib.destroy_tcp_link(self.link)

    def write(self, data: str) -> bool:
        return Datalink.lib.write_str_link(
            self.link,
            ctypes.c_char_p(data.encode('utf-8')),
            ctypes.c_long(len(data)))

    def is_ready(self) -> bool:
        return Datalink.lib.is_ready(self.link)

    def read(self) -> tuple[str, int]:
        size = ctypes.c_long(0)
        result_ptr = Datalink.lib.next_message(self.link, ctypes.byref(size))
        if size == 0:
            return "", 0
        
        data: str = None
        try:
            data = ctypes.cast(
                result_ptr, ctypes.c_char_p).value.decode('utf-8')
        except:
            data = ""

        #Datalink.lib.free_memory(result_ptr)
        return data, size.value

    def has_data(self) -> bool:
        return  Datalink.lib.has_data(self.link)
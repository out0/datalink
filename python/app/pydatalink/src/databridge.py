import ctypes, os
import numpy as np


FLOAT_SIZE_BYTES = 4  # sizeof_float
DEFAULT_HOST = None
DEFAULT_PORT = 20000

class Databridge:
    ''' Creates a TCP link between two endpoints
    '''
    link: ctypes.c_void_p

    def __init__(self, in_host: str = None, in_port: int = 20000, out_host: str = None, out_port: int = 20001, timeout: float = -1) -> None:
        Databridge.setup_cpp_lib()

        if in_host is None and out_host is None: 
            print (f"server [{in_port}] / server [{out_port}]") 
            self.__link = Databridge.lib.init_bridge_server_server(ctypes.c_int(in_port), ctypes.c_int(out_port),  ctypes.c_float(timeout))
            return
        if in_host is None:
            print (f"server [{in_port}] / client ~") 
            self.__link = Databridge.lib.init_bridge_server_client(ctypes.c_int(in_port), out_host.encode('utf-8'), ctypes.c_int(out_port),  ctypes.c_float(timeout))
            return
        if out_host is None:
            print (f"client ~ / server [{out_port}]") 
            self.__link = Databridge.lib.init_bridge_client_server(in_host.encode('utf-8'), ctypes.c_int(in_port), ctypes.c_int(out_port),  ctypes.c_float(timeout))
            return

        print ("client ~ / client ~") 
        self.__link = Databridge.lib.init_bridge_client_server(in_host.encode('utf-8'), ctypes.c_int(in_port), out_host.encode('utf-8'), ctypes.c_int(out_port),  ctypes.c_float(timeout))

    @classmethod
    def setup_cpp_lib(cls) -> None:
        if hasattr(Databridge, "lib"):
            return
        
        lib_path = os.path.join(os.path.dirname(__file__), "../cpp", "libdatalink.so")

        Databridge.lib = ctypes.CDLL(lib_path)

        Databridge.lib.init_bridge_server_server.restype = ctypes.c_void_p
        Databridge.lib.init_bridge_server_server.argtypes = [
              ctypes.c_int,     # in_port
              ctypes.c_int,     # out_port
              ctypes.c_float      # timeout
        ]
        Databridge.lib.init_bridge_client_server.restype = ctypes.c_void_p
        Databridge.lib.init_bridge_client_server.argtypes = [
              ctypes.c_char_p,  # in_host
              ctypes.c_int,     # in_port
              ctypes.c_int,     # out_port
              ctypes.c_float      # timeout
        ]
        Databridge.lib.init_bridge_server_client.restype = ctypes.c_void_p
        Databridge.lib.init_bridge_server_client.argtypes = [
              ctypes.c_int,     # in_port
              ctypes.c_char_p,  # out_host
              ctypes.c_int,     # out_port
              ctypes.c_float     # timeout
        ]
        Databridge.lib.init_bridge_client_client.restype = ctypes.c_void_p
        Databridge.lib.init_bridge_client_client.argtypes = [
              ctypes.c_char_p,  # in_host
              ctypes.c_int,     # in_port
              ctypes.c_char_p,  # out_host
              ctypes.c_int,     # out_port
              ctypes.c_float    # timeout
        ]
       
        Databridge.lib.destroy_bridge.restype = None
        Databridge.lib.destroy_bridge.argtypes = [
              ctypes.c_void_p  # bridge
        ]


    def __del__(self):
        Databridge.lib.destroy_bridge(self.__link)

    
import ctypes, os
import numpy as np
from . datalink import Datalink

FLOAT_SIZE_BYTES = 4  # sizeof_float
DEFAULT_HOST = None
DEFAULT_PORT = 20000

class Databridge:
    ''' Creates a TCP link between two endpoints
    '''
    __link: ctypes.c_void_p

    def __init__(self, left: Datalink, right: Datalink) -> None:
        Databridge.setup_cpp_lib()
        self.__link = Databridge.lib.init_bridge(left.link, right.link)

    @classmethod
    def setup_cpp_lib(cls) -> None:
        if hasattr(Databridge, "lib"):
            return
        
        lib_path = os.path.join(os.path.dirname(__file__), "../cpp", "libdatalink.so")

        Databridge.lib = ctypes.CDLL(lib_path)

        Databridge.lib.init_bridge.restype = ctypes.c_void_p
        Databridge.lib.init_bridge.argtypes = [
              ctypes.c_void_p,     # left link
              ctypes.c_void_p,     # right link
        ]

        Databridge.lib.destroy_bridge.restype = None
        Databridge.lib.destroy_bridge.argtypes = [
              ctypes.c_void_p  # bridge
        ]

    

    def __del__(self):
        Databridge.lib.destroy_bridge(self.__link)

    
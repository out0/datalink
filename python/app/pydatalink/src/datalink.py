import ctypes, os
import numpy as np
import json


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
        
        Datalink.lib.write_int_array.restype = ctypes.c_bool
        Datalink.lib.write_int_array.argtypes = [ctypes.c_void_p,
                                    np.ctypeslib.ndpointer(dtype=ctypes.c_int, ndim=1), ctypes.c_long]

        Datalink.lib.write_int8_array.restype = ctypes.c_bool
        Datalink.lib.write_int8_array.argtypes = [ctypes.c_void_p,
                                    np.ctypeslib.ndpointer(dtype=ctypes.c_int8, ndim=1), ctypes.c_long]

        Datalink.lib.write_long_array.restype = ctypes.c_bool
        Datalink.lib.write_long_array.argtypes = [ctypes.c_void_p,
                                    np.ctypeslib.ndpointer(dtype=ctypes.c_long, ndim=1), ctypes.c_long]

        Datalink.lib.write_float_array.restype = ctypes.c_bool
        Datalink.lib.write_float_array.argtypes = [ctypes.c_void_p,
                                    np.ctypeslib.ndpointer(dtype=ctypes.c_float, ndim=1), ctypes.c_long]

        Datalink.lib.write_double_array.restype = ctypes.c_bool
        Datalink.lib.write_double_array.argtypes = [ctypes.c_void_p,
                                    np.ctypeslib.ndpointer(dtype=ctypes.c_double, ndim=1), ctypes.c_long]



        Datalink.lib.is_ready.restype = ctypes.c_bool
        Datalink.lib.is_ready.argtypes = [ctypes.c_void_p]


        Datalink.lib.next_message.restype = ctypes.POINTER(ctypes.c_char_p)
        Datalink.lib.next_message.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long)]

        Datalink.lib.next_message_int_array.restype = ctypes.POINTER(ctypes.c_int)
        Datalink.lib.next_message_int_array.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long)]

        Datalink.lib.next_message_int8_array.restype = ctypes.POINTER(ctypes.c_int8)
        Datalink.lib.next_message_int8_array.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long)]

        Datalink.lib.next_message_long_array.restype = ctypes.POINTER(ctypes.c_long)
        Datalink.lib.next_message_long_array.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long)]
        
        Datalink.lib.next_message_float_array.restype = ctypes.POINTER(ctypes.c_float)
        Datalink.lib.next_message_float_array.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long)]

        Datalink.lib.next_message_double_array.restype = ctypes.POINTER(ctypes.c_double)
        Datalink.lib.next_message_double_array.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_long)]

        Datalink.lib.free_memory.argtypes = [ctypes.c_void_p]
        Datalink.lib.free_memory.restype = None

        Datalink.lib.free_memory_int.argtypes = [ ctypes.POINTER(ctypes.c_int)]
        Datalink.lib.free_memory_int.restype = None

        Datalink.lib.free_memory_int8.argtypes = [ ctypes.POINTER(ctypes.c_int8)]
        Datalink.lib.free_memory_int8.restype = None

        Datalink.lib.free_memory_long.argtypes = [ ctypes.POINTER(ctypes.c_long)]
        Datalink.lib.free_memory_long.restype = None

        Datalink.lib.free_memory_float.argtypes = [ ctypes.POINTER(ctypes.c_float)]
        Datalink.lib.free_memory_float.restype = None
        
        Datalink.lib.free_memory_double.argtypes = [ ctypes.POINTER(ctypes.c_double)]
        Datalink.lib.free_memory_double.restype = None

        Datalink.lib.has_data.restype = ctypes.c_bool
        Datalink.lib.has_data.argtypes = [ctypes.c_void_p]

    def __del__(self):
        Datalink.lib.destroy_tcp_link(self.link)

 

    def is_ready(self) -> bool:
        return Datalink.lib.is_ready(self.link)

    def has_data(self) -> bool:
        return  Datalink.lib.has_data(self.link)
    
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
    
    def read_bytes(self) -> tuple[bytes, int]:
        size = ctypes.c_long(0)
        result_ptr = Datalink.lib.next_message(self.link, ctypes.byref(size))
        if size == 0:
            return "", 0
        
        res = ctypes.string_at(result_ptr, size.value)

        #Datalink.lib.free_memory(result_ptr)
        return res, size.value
                  
    # def encode_type(self, dtype) -> int:
    #     dt = np.dtype(dtype)
    #     if dt == np.dtype(np.float32):
    #         return FLOAT_32
    #     if dt == np.dtype(np.float64):
    #         return FLOAT_64
    #     if dt == np.dtype(np.int32):
    #         return INT_32
    #     if dt == np.dtype(np.int64):
    #         return INT_64
    #     if dt == np.dtype(np.int8):
    #         return INT_8
    #     if dt == np.dtype(np.uint8):
    #         return UINT_8
    #     # unknown/unsupported
    #     return UNSUPPORTED

    # def decode_type(self, type_code: int) -> np.dtype:
    #     if type_code == FLOAT_32:
    #         return np.dtype(np.float32)
    #     if type_code == FLOAT_64:
    #         return np.dtype(np.float64)
    #     if type_code == INT_32:
    #         return np.dtype(np.int32)
    #     if type_code == INT_64:
    #         return np.dtype(np.int64)
    #     if type_code == INT_8:
    #         return np.dtype(np.int8)
    #     if type_code == UINT_8:
    #         return np.dtype(np.uint8)
    #     # unknown/unsupported
    #     return None

    def __write_np(self, data: np.array) -> bool:
        total_size = data.shape[0]
        if len(data.shape) > 1: 
            for i in range(1, len(data.shape)):
                total_size *= data.shape[i]

        sending_data = np.ascontiguousarray(data.reshape(total_size), dtype=data.dtype)

        print (f"sending_data shape: {sending_data.shape}")
        
        if data.dtype == np.dtype(np.int32):
            return Datalink.lib.write_int_array(
                    self.link,
                    sending_data,
                    ctypes.c_long(total_size))
        
        if data.dtype == np.dtype(np.int64):
            return Datalink.lib.write_long_array(
                    self.link,
                    sending_data,
                     ctypes.c_long(total_size))
        
        if data.dtype == np.dtype(np.float32):
            return Datalink.lib.write_float_array(
                    self.link,
                    sending_data,
                    ctypes.c_long(total_size))
        
        if data.dtype == np.dtype(np.float64):
            return Datalink.lib.write_double_array(
                    self.link,
                    sending_data,
                    ctypes.c_long(total_size))
        
        if data.dtype == np.dtype(np.int8):
            return Datalink.lib.write_int8_array(
                    self.link,
                    sending_data,
                    ctypes.c_long(total_size))

    def write(self, data: any) -> bool:
        if isinstance(data, np.ndarray):
            return self.__write_np(data)
    
        else:
            if isinstance(data, str):
                msg = data
            else:
                msg = str(data)
            
            return Datalink.lib.write_str_link(
                self.link,
                ctypes.c_char_p(msg.encode('utf-8')),
                ctypes.c_long(len(data))) 
    
    def read_np(self, shape, dtype) -> tuple[np.ndarray, int]:
        
        ptr = None
        size = ctypes.c_long(0)

        if dtype == np.dtype(np.int32):
            ptr = Datalink.lib.next_message_int_array(
                    self.link,
                    ctypes.byref(size))
            
            size = size.value           
            res = np.zeros(size, dtype=dtype)

            for i in range (size):
                res[i] = int(ptr[i])

            Datalink.lib.free_memory_int(ptr)

        elif dtype == np.dtype(np.int64):
            ptr = Datalink.lib.next_message_long_array(
                    self.link,
                    ctypes.byref(size))

            size = size.value           
            res = np.zeros(size, dtype=dtype)

            for i in range (size):
                res[i] = int(ptr[i])

            Datalink.lib.free_memory_long(ptr)

        elif dtype == np.dtype(np.float32):
            ptr = Datalink.lib.next_message_float_array(
                    self.link,
                    ctypes.byref(size))

            size = size.value           
            res = np.zeros(size, dtype=dtype)

            for i in range (size):
                res[i] = float(ptr[i])
            
            Datalink.lib.free_memory_float(ptr)

        elif dtype == np.dtype(np.float64):
            ptr = Datalink.lib.next_message_double_array(
                    self.link,
                    ctypes.byref(size))

            size = size.value           
            res = np.zeros(size, dtype=dtype)

            for i in range (size):
                res[i] = float(ptr[i])

            Datalink.lib.free_memory_double(ptr)       

        elif dtype == np.dtype(np.int8):
            ptr = Datalink.lib.next_message_int8_array(
                    self.link,
                    ctypes.byref(size))

            size = size.value           
            res = np.zeros(size, dtype=dtype)

            for i in range (size):
                res[i] = float(ptr[i])

            Datalink.lib.free_memory_int8(ptr)  

        else: return None, 0

        return res.reshape(shape), size

        

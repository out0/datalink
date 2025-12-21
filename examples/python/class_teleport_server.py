#
# This example shows how to "teleport" a class from a python script to another via the network
#
import time
from pydatalink import  recv_object

class MyData:
    str_val: str
    int_val: int
    float_val: float
    bool_val: bool
    array_val: list
    dict_val: dict
        
def main():
    data, _ = recv_object()
    print ("data:")
    print (f"\t str_val: {data.str_val}")
    print (f"\t int_val: {data.int_val}")
    print (f"\t float_val: {data.float_val}")
    print (f"\t bool_val: {data.bool_val}")
    print (f"\t array_val: {data.array_val}")
    print (f"\t dict_val: {data.dict_val}")
    
    time.sleep(0.1)

if __name__ == "__main__":
    main()
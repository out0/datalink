#
# This example shows how to "teleport" a class from a python script to another via the network
#
from pydatalink import send_object, recv_object

class MyData:
    str_val: str
    int_val: int
    float_val: float
    bool_val: bool
    array_val: list
    dict_val: dict

    def __init__(self):
        self.str_val = "abcABC"
        self.int_val = 123
        self.float_val = 1.23
        self.bool_val = True
        self.array_val = [1, 2, 3, 4, 5]
        self.dict_val = {"a": 1, "b": 2, "c": 3}
        
def main():
    data = MyData()
    send_object(data, "127.0.0.1")

   

if __name__ == "__main__":
    main()
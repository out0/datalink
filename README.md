## Datalink

#### Datalink provides fast dual easy client-server connection between two terminals A <------> B

The idea is simple: declare one side as server and another side as client. The link is going to keep the connection and re-connect if it fails, automatically.

All data is transmited using uint8_t format, equivalent to an unsigned byte, which means that any connection that can
handle a stream of bytes (all connections in the world?) may support the datalink payload.

Python to C++ binding is also performed on byte array level. Python np array, string or any type of data is converted into bytes to be sent through the link. To avoid memory leaking, the python interpreter is responsible for handling the buffer data. We're allocating a new buffer for every message, since we understand that this is much more reliable.

In a pure TCP local connection, we've achieved 5 Gb/s in C++ and ~4.3 Gb/s in Python which are close numbers.

Server example in C++ (sending as much data as possible):

```cpp

    uint8_t *payload = new uint8_t[SIZE];
    for (int i = 0; i < SIZE; i++)
        payload[i] = i % 1024;

    auto link = Datalink::TcpServer(20000, 100);

    while (true)
    {
        if (!link->isReady())
        {
            printf("Waiting for the client to connect\n");
            while (!link->isReady()) {}
            printf("client connected\n");
        }
        printf ("sending payload\n");
        link->write(payload, SIZE, 123.45);

        auto [k, t] = link->readMessage();        
        //printf ("hit enter\n");
        //std::cin.get();
        sleep(0.01);
    }

```

Client example in C++ (receiving as much data as possible):

```cpp

    auto link = Datalink::TcpClient("127.0.0.1", 20000, 100);

    while (true)
    {
        if (!link->isReady())
        {
            printf("Establishing a connection to the server...\n");
            while (!link->isReady()) {}
            printf("connected\n");
        }
            
        if (link->hasData()) 
        {
            auto [data, timestamp] = link->readMessage();
            printf("received %ld bytes [%f]\n", data.size(), timestamp);            
            link->writeKeepAlive();
        }
    }

```

Server example in Python (sending as much data as possible):



```python

    link = Datalink(port=20000, timeout=100)
    payload = "A" * (1024 * 1024)

    pos = 1
    while True:
        if not link.is_ready():
            print ("waiting for client to connect")
            while not link.is_ready():
                time.sleep(0.01)
        
        print (f"sending msg #{pos} - {len(payload)} bytes")
        link.write(payload)
        pos += 1
```

Client example in Python (receiving as much data as possible):
```python

    link = Datalink(host="127.0.0.1", port=20000, timeout=100)

    while True:
        if not link.is_ready():
            print ("waiting for the server to be ready")
            while not link.is_ready():
                time.sleep(0.01)
        
        rcv, sz, timestamp = link.read()
        print (f"received: {sz} bytes, timestamp: {timestamp}")

        link.write_keep_alive()
```

Server example in Python (sending a heavy 1000x1000x3 np.float32 array):


```python

    link = Datalink(port=20000, timeout=1000)
    payload = np.full((1000, 1000, 3), fill_value=21.7, dtype=np.float32)

    while True:
        if not link.is_ready():
            print ("waiting for client to connect")
            while not link.is_ready():
                time.sleep(0.01)
        
        print("sending...")
        link.write(payload)
        time.sleep(0.5)
```

Client example in Python (receiving a heavy 1000x1000x3 np.float32 array):
```python

    link = Datalink(host="127.0.0.1", port=20000, timeout=1000)

    while True:
        if not link.is_ready():
            print ("waiting for the server to be ready")
            while not link.is_ready():
                time.sleep(0.01)
        
        if link.has_data():
            rcv, sz, timestamp = link.read_np((1000, 1000, 3), dtype=np.float32)
            if sz == 0:
                continue
            
            link.write_keep_alive()
            
            print (f"received: {sz} bytes")
            print (f"array shape: {rcv.shape}")

```

### Databridge

Datalink can provide a bridge to connect two unreachable endpoints. For example, lets say that you have a datalink
A as a client, connecting to a server via port 20000 and the server is actually being server in port 21000. 
The brigde connects those two links using two more datalinks to forward information:
<br />
<br />
A <----  X  ----> B
<br />
<br />
A <----> D_left-D_right  <----> B
<br />
<br />
Bridge example in C++

```cpp

    // serves any client trying to reach port 20000
    auto left = Datalink::TcpServer(20000, 100);
    // connects to the client on port 21000
    auto right = Datalink::TcpClient("127.0.0.1", 21000, 100);
    auto bridge = DataBrigde(left.get(), right.get());
    printf ("hit enter to kill the bridge...\n");
    std::cin.get();

```

Bridge example in Python

```python

    left = Datalink(port=20000, timeout=100)
    right = Datalink(host="127.0.0.1", port=21000, timeout=100)
    Databridge(left, right)
    print ("hit enter to kill the bridge...")
    input()

```
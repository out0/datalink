## Datalink

### This is still an on-going project

#### Datalink provides fast dual easy client-server connection between two terminals A <------> B
#### The idea is simple: declare one side as server and another side as client. The link is going to keep the connection and re-connect if it fails, automatically.

Server example in C++ (sending as much data as possible):

```cpp

    char *payload = new char[SIZE];
    for (int i = 0; i < SIZE; i++)
        payload[i] = i % 1024;

    TCPLink link(20000, 100);

    while (true)
    {
        if (!link.isReady())
        {
            printf("Waiting for the client to connect\n");
            while (!link.isReady())
                ;
            printf("client connected\n");
        }
        printf ("sending payload\n");
        link.write(payload, SIZE);        
    }

```

Client example in C++ (receiving as much data as possible):

```cpp

    TCPLink link("127.0.0.1", 20000, 100);

    while (true)
    {
        if (!link.isReady())
        {
            printf("Establishing a connection to the server...\n");
            while (!link.isReady())
                ;
            printf("connected\n");
        }
        
        if (link.hasData()) {
            auto data = link.readMessage();
            printf("received %ld bytes\n", data.size());
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
        
        rcv, sz = link.read()
        print (f"received: {sz} bytes")
```


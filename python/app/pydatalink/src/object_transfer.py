from . datalink import Datalink
import time

DEFAULT_PORT=20000
DEFAULT_TIMEOUT_MS = -1

def send_object(object: any, host: str, port: int = DEFAULT_PORT, timeout = DEFAULT_TIMEOUT_MS) -> bool:

    link = Datalink(host=host, port=port, timeout=timeout)
    return link.send_object(object, timestamp=time.time())


def recv_object(port: int = DEFAULT_PORT, timeout = DEFAULT_TIMEOUT_MS) -> tuple[any, float]:

    link = Datalink(port=port, timeout=timeout)
    return link.recv_object()

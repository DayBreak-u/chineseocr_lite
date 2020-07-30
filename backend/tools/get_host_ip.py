

import socket


def host_ip():
    """
    查询本机ip地址
    :return: ip
    """
    ip = '0.0.0.0'
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except OSError as ex:
        hostname = socket.gethostname()
        ip = socket.gethostbyname(hostname)
    finally:
        s.close()

    return ip

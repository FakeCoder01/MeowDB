from meowdb import MeowDB


import socket
import threading
import time

def send_request(client_id):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('10.10.172.123', 6969))
        s.sendall(f"create key{client_id} value{client_id}".encode())
        response = s.recv(1024).decode()
        s.close()
        # print(f"Client {client_id}: {response}")
    except Exception as e:
        print(f"Client {client_id} failed: {e}")

def concurrency_test(num_clients):
    threads = []
    start_time = time.time()
    for i in range(num_clients):
        t = threading.Thread(target=send_request, args=(i,))
        threads.append(t)
        t.start()
    for t in threads:
        t.join()
    end_time = time.time()
    total_time = end_time - start_time
    print(f"Total time for {num_clients} clients: {total_time:.2f}s")


def test_commands():
    db = MeowDB(host='10.10.172.123', port=6969)
    c = {1 : "insert", 2 : "update", 3 : "delete", 4 : "select", 5 : "close"}

    while True:
        r = ""
        print(c)
        choosen = int(input(":: > "))
        if choosen == 5:
            db.close()
            break
        
        cmd = input(c[choosen] + " > ")
        match choosen:
            case 1:
                r = db.insert(cmd.split()[0], cmd.split()[1])
            case 2:
                r = db.update(cmd.split()[0], cmd.split()[1])
            case 3:
                r = db.delete(cmd)
            case 4:
                r = db.select(cmd)
            case 0:
                for num_clients in [10, 50, 100, 200, 500, 1000]:
                    print(f"Testing with {num_clients} concurrent clients...")
                    concurrency_test(num_clients)
            case _:
                db.close()
                break
        print("# > ", r)


if __name__ == "__main__":
    test_commands()


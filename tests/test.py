import socket
import threading
import time

def send_request(client_id):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('127.0.0.1', 6969))
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

# Test with increasing numbers of concurrent clients
# for num_clients in [10, 50, 100, 200, 500, 1000]:
#    print(f"Testing with {num_clients} concurrent clients...")
#    concurrency_test(num_clients)
concurrency_test(1000)

import socket
import threading
import time
import random

PORT = 8080
NUM_ORDERS = 1000

def send_order():
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect(("127.0.0.1", PORT))
        
        # Randomize ID (1000 to 9999) and Side (1 for Buy, 0 for Sell)
        order_id = random.randint(1000, 9999)
        is_buy = random.choice([0, 1])
        payload = f"{order_id} {is_buy} 10 150.0 0 -1 0 0 0 0.0 0 0"
        time.sleep(0.05)
        client.sendall(payload.encode('utf-8'))
        client.close()
    except Exception as e:
        pass

def main():
    print(f"--- STARTING LOAD TEST ---")
    print(f"Blasting {NUM_ORDERS} random orders to port {PORT}...")
    
    threads = []
    start_time = time.time()
    
    for _ in range(NUM_ORDERS):
        t = threading.Thread(target=send_order)
        threads.append(t)
        t.start()
        
    for t in threads:
        t.join()
        
    elapsed = time.time() - start_time
    print(f"Finished! {NUM_ORDERS} random orders sent in {elapsed:.4f} seconds.")

if __name__ == "__main__":
    main()

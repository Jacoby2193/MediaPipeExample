import cv2
import mediapipe as mp
import socket
import threading
import time

from numpy.ma.core import append

mp_drawing = mp.solutions.drawing_utils
mp_hands = mp.solutions.hands

# 서버 설정
TCP_IP = '0.0.0.0'  # 모든 인터페이스에서 수신
TCP_PORT = 23458  # 서버 포트 번호
BUFFER_SIZE = 1024

# 클라이언트 설정
CLIENT_TCP_IP = '192.168.0.53' # 서버 IP 주소
CLIENT_TCP_PORT = 23458  # 서버 포트 번호

clients = []

def handle_client(conn, addr):
    """클라이언트의 요청을 처리하는 함수"""
    global clients
    clients.append(conn)  # 클라이언트를 리스트에 추가
    print(f"클라이언트와 연결되었습니다: {addr}")

    try:
        while True:  # 클라이언트와 데이터 통신
            data = conn.recv(BUFFER_SIZE)
            if not data:
                break
            f1 = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7]
            f2 = data[8] << 24 | data[9] << 16 | data[10] << 8 | data[11]
            print(f"받은 데이터: {data}, f1 : {f1}, f2 : {f2}")

            # 모든 클라이언트에 데이터 전송
            for client in clients:
                if client != conn:  # 데이터를 보낸 클라이언트를 제외
                    client.send(data)
    finally:
        conn.close()  # 클라이언트와의 연결 종료
        clients.remove(conn)  # 클라이언트 리스트에서 제거
        print(f"클라이언트와의 연결이 종료되었습니다: {addr}")


def start_server():
    """서버를 시작하여 클라이언트로부터 데이터를 수신"""
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((TCP_IP, TCP_PORT))
    server_socket.listen(5)  # 최대 5개의 클라이언트를 대기
    print("서버가 시작되었습니다. 클라이언트 연결을 기다리는 중...")

    while True:
        conn, addr = server_socket.accept()  # 클라이언트 연결 수립
        # 각 클라이언트에 대해 새로운 스레드 생성
        threading.Thread(target=handle_client, args=(conn, addr)).start()

    server_socket.close()  # 서버 소켓 종료


cap = cv2.VideoCapture(0)

def client_process():

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((CLIENT_TCP_IP, CLIENT_TCP_PORT))

    send_time = time.time()

    with mp_hands.Hands(
            max_num_hands=1,
            min_detection_confidence=0.5,
            min_tracking_confidence=0.5) as hands:
        while cap.isOpened():
            success, image = cap.read()
            if not success:
                continue
            image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)

            results = hands.process(image)

            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

            # image.flags.writeable = False

            packet = bytearray(12)
            packet[0] = 0xff
            packet[1] = 0xff
            packet_body_length = 8
            packet_command_no = 1
            packet[2] = packet_command_no
            packet[3] = packet_body_length

            if results.multi_hand_landmarks:
                for hand_landmarks in results.multi_hand_landmarks:
                    finger1 = int(hand_landmarks.landmark[4].x * 100)
                    finger2 = int(hand_landmarks.landmark[8].x * 100)

                    packet[4] = (finger1 >> 24) & 0xff
                    packet[5] = (finger1 >> 16) & 0xff
                    packet[6] = (finger1 >> 8) & 0xff
                    packet[7] = finger1 & 0xff

                    packet[8] = (finger2 >> 24) & 0xff
                    packet[9] = (finger2 >> 16) & 0xff
                    packet[10] = (finger2 >> 8) & 0xff
                    packet[11] = finger2 & 0xff

                    dist = abs(finger1 - finger2)
                    cv2.putText(
                        image, text='f1=%d f2=%d dist=%d ' % (finger1, finger2, dist), org=(10, 30),
                        fontFace=cv2.FONT_HERSHEY_SIMPLEX, fontScale=1,
                        color=255, thickness=3)

                    mp_drawing.draw_landmarks(image, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            if time.time() - send_time > 1:
                print('send!')
                send_time = time.time()
                client_socket.send(bytearray(packet))

            cv2.imshow('image', image)
            if cv2.waitKey(1) == ord('q'):
                break

    cap.release()
    client_socket.close()
    cv2.destroyAllWindows()


# 서버를 스레드로 실행
server_thread = threading.Thread(target=start_server)
server_thread.start()

# 클라이언트 프로세스 실행
client_process()

# 서버 스레드가 종료될 때까지 대기
server_thread.join()

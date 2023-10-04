import cv2
from multiprocessing import Process,shared_memory
import time
shared=[1]
def subprocess(arg):
    captured = cv2.VideoCapture(0)
    while True:
        result=arg**2

        _, frame = captured.read()
        print(frame)

        shared.append(frame)
        print('in sub', shared[-1])




if __name__ == "__main__":

    num=6
    sub = Process(target=subprocess, args=(num,))
    sub.start()
    i = 0
    while True:
        i+=1
        read=shared[-1]
        print('read=', read)
        print('i=', i)
        time.sleep(0.5)



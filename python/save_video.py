
import cv2

if __name__ == "__main__":
    # find the webcam
    capture = cv2.VideoCapture(0)

    # record video
    i=0
    while (capture.isOpened()):
        ret, image = capture.read()
        cv2.imshow("captured img",image)
        k=cv2.waitKey(1)&0xFF
        if k==ord('s'):
            cv2.imwrite('image_{}.png'.format(i),image)
            print('image saved!')
            i+=1
        elif k==ord('q'):
            break
    capture.release()
    cv2.destroyAllWindows()



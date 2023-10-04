import numpy as np
import cv2
from matplotlib import pyplot as plt
import math
import time

class ObjectDetector:


    #Note: Here I didn't use a init func, because it will bring up an error related to multi-processing in OpenCV.


    def detect(self, video, conn):

        # Parameter for main body of the ball bearing
        params = cv2.SimpleBlobDetector_Params()
        params.filterByArea = True
        params.filterByInertia = False
        params.filterByConvexity = True
        params.filterByColor = True
        params.minArea = 400
        params.maxArea = 2000
        params.minConvexity = 0.4
        params.blobColor = 255
        # Parameter for the hot spot on the ball bearing
        params2 = cv2.SimpleBlobDetector_Params()
        params2.filterByArea = True
        params2.filterByInertia = False
        params2.filterByConvexity = False
        params2.filterByColor = True
        params2.minArea = 5
        params2.maxArea = 200
        params2.blobColor = 255
        # Actual Value of Ball bearing & camera parameters in cm
        ballSize=2.286
        focalLengthPixel=529.6167

        videoCapture = cv2.VideoCapture('output.mp4')  # For camera, video set to 0
        videoCapture.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        videoCapture.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        while True:


            success, frame = videoCapture.read()
            #print(success)
            #print(frame)
            # im = cv2.imread(image) #Used if 'image' is a path
            im = frame
            #im=cv2.imread('../ims2/im_0380.png')
            im_threshold = cv2.inRange(im, (65, 10, 70), (95, 50, 100))
            im_threshold_hot_spot = cv2.inRange(im, (180, 180, 180), (255, 255, 255))

            detector = cv2.SimpleBlobDetector_create(params)  # create a blob detector for target
            keypoints = detector.detect(im_threshold)

            #Now we only use detector2
            detector2 = cv2.SimpleBlobDetector_create(params2)  # create a blob detector for hot spot on target
            keypoints_hot_spot = detector2.detect(im_threshold_hot_spot)
            plt.imshow(im)

            if  len(keypoints_hot_spot) != 0:
                # If the distance between the spot and ball is close:
                im_with_keypoints = cv2.drawKeypoints(im, keypoints_hot_spot, np.array([]), (255, 255, 255),
                                                      cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
                plt.imshow(im_with_keypoints[:, :, [2, 1, 0]], extent=[0, im.shape[1], im.shape[0], 0])
                plt.plot([keypoints_hot_spot[0].pt[0]], [keypoints_hot_spot[0].pt[1]], 'r+')

                #plt.draw()
                #plt.pause(0.1)
                #plt.close()
                # There will be more than one potential keypoints be detected, where the first one is the most likely.

                ydistance, xdistance=self.calculate_distance_by_single_pixel(keypoints_hot_spot[0].pt[0], keypoints_hot_spot[0].pt[1], focalLengthPixel)

                keyPoint_info=[ydistance,xdistance]
                conn.send(keyPoint_info)

                #conn.close()
                #return keypoints[0].pt  # The x,y coordinate in image coordinate.

            else:
                conn.send(None)


    def calculate_distance_by_single_pixel(self,keypoint_x,keypoint_y, focalLength_pixel):
        y_offset=keypoint_y-240
        x_offset=keypoint_x-320
        y_offset_angle=np.arctan2(y_offset,focalLength_pixel)*(180/np.pi)
        if y_offset_angle>=0:
            y_angle=58-y_offset_angle
        else:
            y_angle=58-y_offset_angle

        y_distance=19*np.tan(y_angle*(np.pi/180))

        x_angle=np.arctan2(x_offset,focalLength_pixel)*(180/np.pi)

        x_offset_distance=np.tan(x_angle*(np.pi/180))*y_distance

        return y_distance,x_offset_distance









import numpy as np
import cv2
from matplotlib import pyplot as plt
import math


class ObjectDetector:
    def __init__(self):
        # Parameter for main body of the ball bearing
        self.params = cv2.SimpleBlobDetector_Params()
        self.params.filterByArea = True
        self.params.filterByInertia = False
        self.params.filterByConvexity = True
        self.params.filterByColor = True
        self.params.minArea = 400
        self.params.maxArea = 2000
        self.params.minConvexity = 0.4
        self.params.blobColor = 255
        # Parameter for the hot spot on the ball bearing
        self.params2 = cv2.SimpleBlobDetector_Params()
        self.params2.filterByArea = True
        self.params2.filterByInertia = False
        self.params2.filterByConvexity = False
        self.params2.filterByColor = True
        self.params2.minArea = 5
        self.params2.maxArea = 200
        self.params2.blobColor = 255

    def splitFrame(self, video, conn):
        videoCapture = cv2.VideoCapture(video)  # For camera, video set to 0
        sucess, frame = videoCapture.read()
        if sucess:
            conn.send(frame)
            conn.close()
            return frame

    def detect(self, image, conn):

        # im = cv2.imread(image) #Used if 'image' is a path
        im = image
        im_threshold = cv2.inRange(im, (65, 10, 70), (95, 50, 100))
        im_threshold_hot_spot = cv2.inRange(im, (250, 250, 250), (255, 255, 255))

        detector = cv2.SimpleBlobDetector_create(self.params)  # create a blob detector for target
        keypoints = detector.detect(im_threshold)

        detector2 = cv2.SimpleBlobDetector_create(self.params2)  # create a blob detector for hot spot on target
        keypoints_hot_spot = detector2.detect(im_threshold_hot_spot)
        plt.imshow(im)

        if (len(keypoints) and len(keypoints_hot_spot)) != 0:  # If there has both keypoints and a spot
            # If the distance between the spot and ball is close:
            if (math.sqrt((keypoints[0].pt[0] - keypoints_hot_spot[0].pt[0]) ** 2 + (
                    keypoints[0].pt[1] - keypoints_hot_spot[0].pt[1]) ** 2) <= 50):
                im_with_keypoints = cv2.drawKeypoints(im, keypoints, np.array([]), (255, 255, 255),
                                                      cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
                plt.imshow(im_with_keypoints[:, :, [2, 1, 0]], extent=[0, im.shape[1], im.shape[0], 0])
                plt.plot([keypoints[0].pt[0]], [keypoints[0].pt[1]], 'r+')

                # plt.draw()
                # plt.pause(0.1)
                # plt.close()
                # There will be more than one potential keypoints be detected, where the first one is the most likely.
                conn.send(keypoints[0].pt)
                conn.close()
                return keypoints[0].pt  #The x,y coordinate in image coordinate.
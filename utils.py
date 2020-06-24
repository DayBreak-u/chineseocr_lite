import numpy as np
import cv2
from PIL import Image


def rotate_cut_img(im, degree, x_center, y_center, w, h, leftAdjust=False, rightAdjust=False, alph=0.2):
    # degree_ = degree * 180.0 / np.pi
    # print(degree_)
    right = 0
    left = 0
    if rightAdjust:
        right = 1
    if leftAdjust:
        left = 1
    
    box = (max(1, x_center - w / 2 - left * alph * (w / 2))
           , y_center - h / 2,  # ymin
           min(x_center + w / 2 + right * alph * (w / 2), im.size[0] - 1)
           , y_center + h / 2)  # ymax
    
    newW = box[2] - box[0]
    newH = box[3] - box[1]
    tmpImg = im.rotate(degree, center=(x_center, y_center)).crop(box)
    
    return tmpImg, newW, newH


def crop_rect(img, rect, alph=0.15):
    img = np.asarray(img)
    # get the parameter of the small rectangle
    # print("rect!")
    # print(rect)
    center, size, angle = rect[0], rect[1], rect[2]
    min_size = min(size)
    
    if angle > -45:
        center, size = tuple(map(int, center)), tuple(map(int, size))
        # angle-=270
        size = (int(size[0] + min_size * alph), int(size[1] + min_size * alph))
        height, width = img.shape[0], img.shape[1]
        M = cv2.getRotationMatrix2D(center, angle, 1)
        # size = tuple([int(rect[1][1]), int(rect[1][0])])
        img_rot = cv2.warpAffine(img, M, (width, height))
        # cv2.imwrite("debug_im/img_rot.jpg", img_rot)
        img_crop = cv2.getRectSubPix(img_rot, size, center)
    else:
        center = tuple(map(int, center))
        size = tuple([int(rect[1][1]), int(rect[1][0])])
        size = (int(size[0] + min_size * alph), int(size[1] + min_size * alph))
        angle -= 270
        height, width = img.shape[0], img.shape[1]
        M = cv2.getRotationMatrix2D(center, angle, 1)
        img_rot = cv2.warpAffine(img, M, (width, height))
        # cv2.imwrite("debug_im/img_rot.jpg", img_rot)
        img_crop = cv2.getRectSubPix(img_rot, size, center)
    img_crop = Image.fromarray(img_crop)
    return img_crop


def draw_bbox(img_path, result, color=(255, 0, 0), thickness=2):
    if isinstance(img_path, str):
        img_path = cv2.imread(img_path)
        # img_path = cv2.cvtColor(img_path, cv2.COLOR_BGR2RGB)
    img_path = img_path.copy()
    for point in result:
        point = point.astype(int)
        cv2.line(img_path, tuple(point[0]), tuple(point[1]), color, thickness)
        cv2.line(img_path, tuple(point[1]), tuple(point[2]), color, thickness)
        cv2.line(img_path, tuple(point[2]), tuple(point[3]), color, thickness)
        cv2.line(img_path, tuple(point[3]), tuple(point[0]), color, thickness)
    return img_path


def sort_box(boxs):
    res = []
    for box in boxs:
        # box = [x if x>0 else 0 for x in box ]
        x1, y1, x2, y2, x3, y3, x4, y4 = box[:8]
        newBox = [[x1, y1], [x2, y2], [x3, y3], [x4, y4]]
        # sort x
        newBox = sorted(newBox, key=lambda x: x[0])
        x1, y1 = sorted(newBox[:2], key=lambda x: x[1])[0]
        index = newBox.index([x1, y1])
        newBox.pop(index)
        newBox = sorted(newBox, key=lambda x: -x[1])
        x4, y4 = sorted(newBox[:2], key=lambda x: x[0])[0]
        index = newBox.index([x4, y4])
        newBox.pop(index)
        newBox = sorted(newBox, key=lambda x: -x[0])
        x2, y2 = sorted(newBox[:2], key=lambda x: x[1])[0]
        index = newBox.index([x2, y2])
        newBox.pop(index)
        
        newBox = sorted(newBox, key=lambda x: -x[1])
        x3, y3 = sorted(newBox[:2], key=lambda x: x[0])[0]
        
        res.append([x1, y1, x2, y2, x3, y3, x4, y4])
    return res


def solve(box):
    """
    绕 cx,cy点 w,h 旋转 angle 的坐标
    x = cx-w/2
    y = cy-h/2
    x1-cx = -w/2*cos(angle) +h/2*sin(angle)
    y1 -cy= -w/2*sin(angle) -h/2*cos(angle)

    h(x1-cx) = -wh/2*cos(angle) +hh/2*sin(angle)
    w(y1 -cy)= -ww/2*sin(angle) -hw/2*cos(angle)
    (hh+ww)/2sin(angle) = h(x1-cx)-w(y1 -cy)

    """
    x1, y1, x2, y2, x3, y3, x4, y4 = box[:8]
    cx = (x1 + x3 + x2 + x4) / 4.0
    cy = (y1 + y3 + y4 + y2) / 4.0
    w = (np.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2) + np.sqrt((x3 - x4) ** 2 + (y3 - y4) ** 2)) / 2
    h = (np.sqrt((x2 - x3) ** 2 + (y2 - y3) ** 2) + np.sqrt((x1 - x4) ** 2 + (y1 - y4) ** 2)) / 2
    
    sinA = (h * (x1 - cx) - w * (y1 - cy)) * 1.0 / (h * h + w * w) * 2
    angle = np.arcsin(sinA)
    return angle, w, h, cx, cy


def app_url(version, name):
    url = '/{}/{}'.format(version, name)
    return url

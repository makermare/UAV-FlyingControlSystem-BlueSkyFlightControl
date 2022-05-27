import cv2
import numpy as np
import math
import time

def init_camera(device = 0, fps = 30):
    capture = cv2.VideoCapture(device)
    capture.set(cv2.CAP_PROP_FPS, 30)
    if not capture.isOpened():
        print('Error: Can\'t open camera.')
        ret = False
    else:
        ret = True
        print('- Init camera done')
    return ret, capture

def get_img_hw(img):
    return img.shape[:2]

def imshow_ssh(frame, name = 'image'):
    height, width = frame.shape[:2]
    size = 320/width
    cv2.imshow(name, cv2.resize(frame, None, fx = size, fy = size))
    cv2.waitKey(10)



def find_circle(img):
    # 原理：利用 Hough变换
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)  # 转换为灰度图
    blur = cv2.GaussianBlur(gray, (9, 9), 2, 2)  # 高斯模糊，消除噪点干扰
    circles = cv2.HoughCircles(blur, cv2.HOUGH_GRADIENT, 1, 50)  # Hough 变换找圆
    # cv2.HoughCircles(image,method,dp,minDist[, circles[,param1, param2[,minRadius[,maxRadius]]]]])
    # HoughCircles 参数参数解释：
    # image: 输入图片
    # method: cv2.HOUGH_GRADIENT 梯度法
    # dp: ???
    # minDist：圆心之间的最小距离（单位为像素）

    if circles is not None:
        ret = True
        circles = np.uint16(np.around(circles))
        for i in circles[0, :]:
            center = (i[0], i[1])
            radius = i[2]
            img = cv2.circle(img, center, 3, (0, 255, 0), 2)
            img = cv2.circle(img, center, radius, (255, 50, 0), 3)
    else:
        ret = False
        circles = []
    imshow_ssh(img, 'circle')
    return ret, circles

def find_ellipse(img):
    # 原理：找轮廓，然后计算轮廓面积，然后用椭圆去拟合轮廓，然后比较轮廓面积和椭圆面积
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    imgray = cv2.Canny(img, 600, 100, 3)  # Canny边缘检测，参数可更改
    ret, thresh = cv2.threshold(imgray, 127, 255, cv2.THRESH_BINARY)
    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    # contours为轮廓集，可以计算轮廓的长度、面积等
    centers = []
    for cnt in contours:
        if len(cnt) > 50:
            S1 = cv2.contourArea(cnt)
            ell = cv2.fitEllipse(cnt)
            S2 = math.pi * ell[1][0] * ell[1][1] + 0.00000001
            ratio = S1 / S2
            if 0.23 < ratio < 10:  # 面积比例，可以更改，根据数据集
                img = cv2.ellipse(img, ell, (0, 255, 0), 2)
                # print(str(S1 / S2) + "      " + str(S1) + "    " + str(S2) + "   " + str(ell[0][0]) + "   " + str(ell[0][1]))
                centers.append([ell[0][0], ell[0][1]])
    center = find_center(centers)
    if center[0]:
        cv2.circle(img, (int(center[0]), int(center[1])), 3, (0, 255, 0), 2)
    
    imshow_ssh(img)
    return centers

def find_center(centers):
    # 从一组圆心中间找到同心圆的圆心
    # 时间复杂度 O(n)

    # 没有点找个屁
    if len(centers) == 0:
        return False, False

    # 只有一个点时，默认该点就是圆心
    if len(centers) == 1:
        return centers[0]

    # 排除孤立点
    # 计算最大/最小点的距离
    # 距离大于阈值则认为两点都是孤立点
    centers_np = np.array(centers)
    length = len(centers)
    threshold = 20
    index = np.argsort(centers_np, axis=0)
    invalid_points = []
    for i in range(length // 2):
        max_x = index[length - 1 - i, 0]
        min_x = index[i, 0]
        max_y = index[length - 1 - i, 1]
        min_y = index[i, 1]
        if centers_np[max_x, 0] - centers_np[min_x, 0] > threshold:
            if max_x not in invalid_points:
                invalid_points.append(max_x)
            if min_x not in invalid_points:
                invalid_points.append(min_x)
        if centers_np[max_y, 1] - centers_np[min_y, 1] > threshold:
            if max_y not in invalid_points:
                invalid_points.append(max_y)
            if min_y not in invalid_points:
                invalid_points.append(min_y)

    # 计算有效点的平均x,y
    valid_points = [i for i in range(length) if i not in invalid_points]
    if len(valid_points) == 0:
        return False, False
    # print(valid_points)
    mean_x = np.mean(centers_np[valid_points, 0])
    mean_y = np.mean(centers_np[valid_points, 1])
    # for i in valid_points:
    #     mean_x += centers_np[i, 0]
    #     mean_y += centers_np[i, 1]
    # mean_x /= len(valid_points)
    # mean_y /= len(valid_points)

    # 计算无效点与平均x,y的距离
    # 避免误判无效点
    for i in invalid_points:
        if abs(centers_np[i, 0] - mean_x) < threshold and abs(centers_np[i, 1] - mean_x) < threshold:
            valid_points.append(i)

    # 重新计算有效点
    mean_x = 0
    mean_y = 0
    for i in valid_points:
        mean_x += centers_np[i, 0]
        mean_y += centers_np[i, 1]
    mean_x /= len(valid_points)
    mean_y /= len(valid_points)

    return mean_x, mean_y


def find_pillar(img):
    # 寻找柱子
    # 返回柱子左边的x坐标和宽度
    # 若没有找到柱子，则返回 False, False
    rows, cols = img.shape[:2]

    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    mask_color = cv2.inRange(hsv, np.array((0, 0, 0)), np.array((180, 255, 46)))  # 过滤黑色
    # 关于 HSV 的颜色：https://blog.csdn.net/ColdWindHA/article/details/82080176

    contours, hierarchy = cv2.findContours(mask_color.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)  # 找连通区域
    for cnt in contours:

        if len(cnt) > 400:  # 连通区域面积要大于 400 像素
            y_max = max(cnt[:, :, 1])
            y_min = min(cnt[:, :, 1])
            if y_max - y_min > rows / 2:  # 判断是否贯穿图像的一半
                area = cv2.contourArea(cnt)  # 区域面积
                x, y, w, h = cv2.boundingRect(cnt)  # 竖直矩形拟合
                aspect_ratio = float(w) / h  # 矩形宽长比
                rect_area = w * h  # 矩形面积
                extent = float(area) / rect_area  # 区域面积/矩形面积
                if aspect_ratio < 10 and extent > 0.05:  # 柱子的宽<长 # 面积比不能太悬殊
                    # 计算质心
                    # M = cv2.moments(cnt)
                    # cx = int(M['m10'] / M['m00'])
                    # cy = int(M['m01'] / M['m00'])
                    # print(w / h, extent, cnt.shape)
                    try:
                        real_w, real_x = get_pillar_width(cnt, y_min, y_max)
                    except:
                        real_w, real_x = False, False
                    if real_w and real_x:
                        real_w = int(real_w)
                        real_x = int(real_x)
                    else:
                        real_w = w
                        real_x = x
                        # print(real_w, real_x)
                    cv2.rectangle(img, (real_x, y), (real_x + real_w, y + h), (0, 255, 0), 2)
                    imshow_ssh(img)
                    # cv2.imwrite(str(time.time())+'.jpg', img)
                    return real_x, real_w

                    # 利用 opencv 内置的直线函数拟合
                    # [vx, vy, x, y] = cv2.fitLine(cnt, cv2.DIST_L2, 0, 0.01, 0.01)
                    # lefty = int((-x * vy / vx) + y)
                    # righty = int(((cols - x) * vy / vx) + y)
                    # cv2.line(img, (cols - 1, righty), (0, lefty), (0, 255, 0), 2)
    
    # img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # imgray = cv2.Canny(img, 600, 100, 3)  # Canny边缘检测，参数可更改
    # ret, thresh = cv2.threshold(imgray, 127, 255, cv2.THRESH_BINARY)
    # contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    imshow_ssh(img)
    # cv2.imwrite(str(time.time())+'.jpg', img)
    return False, False

def get_pillar_width(cnt, y_min, y_max):
    points = []
    min_x = []

    for i in range(int(y_min), int(y_max) + 1, 3):  # 每 3 个点采样一次
        index = np.where(cnt[:, :, [1]] == i)
        width = max(cnt[index]) - min(cnt[index])
        points.append([width, 0])
    
    if points:
        mean_x, mean_y = find_center(points)
    else:
        mean_x = 0
    
    for i in range(int(y_min), int(y_max) + 1, 3):  # 每 3 个点采样一次
        index = np.where(cnt[:, :, [1]] == i)
        width = max(cnt[index]) - min(cnt[index])
        if width < mean_x + 10:
            min_x.append([min(cnt[index]), 0])
    
    if min_x:
        start_x, start_y = find_center(min_x)
    else:
        start_x = 0  # 可能需要改成图像中点
    
    return mean_x, start_x

def find_color(img, color):
    if len(color) != 6:
        contours = False
    else:
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv, np.array((color[0], color[1], color[2])), np.array((color[3], color[4], color[5])))
        mask = cv2.dilate(mask, kernel, iterations=3)
        contours, hierarchy = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    return contours


def get_pillar_distance(width):
    # TODO: 用实际的杆来标定
    
    distance = 1644.5*width**(-0.958)
    return distance

if __name__=='__main__':
    ret, capture = init_camera(device = 1)
    while(ret):
        try:
            ret, frame = capture.read()
            x, w = find_pillar(frame)
            if x and w:
                print('像素宽度：', w, '预测距离', get_pillar_distance(w))
        except KeyboardInterrupt:
            print('按键退出')
            break

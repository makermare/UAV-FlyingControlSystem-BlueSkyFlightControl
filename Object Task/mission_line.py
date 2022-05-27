from protocol import protocol
from vision import *
from pid import PID, clip
import time



def init():
    ptcl = protocol(device='/dev/ttyUSB0', baud=115200)
    ret, capture = init_camera(device = 0, fps = 30)
    return ret, capture, ptcl

class AverageFilter(object):
    # 平均滤波
    # 用法：
    #   1. 初始化时写入需要滤波长度
    #   2. 用 .append() 添加新数据
    #   3. 用 .calc() 计算平均值
    def __init__(self, length):
        self.length = length
        self.index = 0
        self.values = []

    def clear(self):
        self.index = 0
        self.values = []

    def append(self, value):
        if len(self.values) < self.length:
            self.values.append(value)
            self.index += 1
            self.index %= self.length
        else:
            self.values[self.index] = value
            self.index += 1
            self.index %= self.length
    
    def calc(self):
        if len(self.values) != 0:
            sum = 0
            for i in self.values:
                sum += i
            avg = sum/len(self.values)
        else:
            avg = 0
        return avg
    
    def has(self, value):
        return value in self.values

if __name__ == '__main__':

    # 初始化协议、摄像头
    print('=== Init Start ===')
    ret = False
    while(not ret):
        ret, capture, ptcl = init()
        ret, frame = capture.read()
    print('=== Init End ===')

    # 获取画面中央像素坐标
    img_height, img_width = get_img_hw()
    img_central_x = img_width/2
    
    # 解锁
    print('=== Arm ===')
    ptcl.clear()
    ptcl.set_motion('arm')
    ptcl.send()
    
    time.sleep(3)

    # 自动起飞
    print('=== Auto Take Off ===')
    ptcl.clear()
    ptcl.set_motion('autoTakeOff')
    ptcl.send()

    # 自动起飞需要一定时间到达指定高度并稳定
    time.sleep(8)

    try:

        # 时间戳，当检测杆超时时自动跳过
        timespan = time.time()

        # 巡航速度初始设置为15cm/s
        velocity = 15

        # 开始找第一根杆
        print('=== Finding First Pillar===')
        ret, frame = capture.read()
        first_pillar = AverageFilter(10)
        [first_pillar.append(False) for i in range(10) ]
        pillar_x, pillar_w = find_pillar(frame)
        first_pillar.append(bool(pillar_w))
        while(first_pillar.has(False)): # 若画面中没有杆
            print('Can\'t find pillar 1')
            
            # 当检测到杆但还没达到重复验证条件时，先降低速度
            if(bool(pillar_w)!=False):
                # 根据杆与画面中央距离降低速度，若杆越过了画面中心，可能是飞机已经飞越也可能是误判
                velocity = (-(pillar_x-img_central_x)/20+15) if (pillar_x>img_central_x) else 5
            else:
                velocity = 15

            ptcl.clear()
            ptcl.set_vel(y=velocity) # 向右移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            first_pillar.append(bool(pillar_w))

            # 当开始寻找开始5秒后仍然找不到杆则跳过第一根杆
            if((time.time()-timespan)>5):
                break

        # 微调使飞机近似对准第一根杆
        while(abs(pillar_x-img_central_x)<50):
            print('Tiny modify distance to pillar 1')
            velocity = (pillar_x-img_central_x)/20
            ptcl.clear()
            ptcl.set_vel(y=velocity) # 向右移动
            ptcl.send()

            if((time.time()-timespan)>5):
                break

        # 找到杆后停止
        ptcl.clear()
        ptcl.send()
        # 在杆前面暂停5秒，日后可以添加距离校正等功能
        time.sleep(5)
        
        # 向右绕过第一根杆
        print('=== Cross First Pillar ===')
        ret, frame = capture.read()
        pillar_x, pillar_w = find_pillar(frame)
        first_pillar.append(bool(pillar_w))
        print(first_pillar.values)
        while(first_pillar.has(True)):
            print('Still has pillar 1')
            ptcl.clear()
            ptcl.set_vel(y=15) # 向右移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            first_pillar.append(bool(pillar_w))
        # 找不到杆后停止
        ptcl.clear()
        ptcl.send()

        # 开始找第二根杆
        print('=== Finding Second Pillar===')
        ret, frame = capture.read()
        second_pillar = AverageFilter(10)
        [second_pillar.append(False) for i in range(10) ]
        pillar_x, pillar_w = find_pillar(frame)
        first_pillar.append(bool(pillar_w))
        while(second_pillar.has(False)): # 若画面中没有杆
            print('Can\'t find pillar 2')
            ptcl.clear()
            ptcl.set_vel(y=15) # 向右移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            second_pillar.append(bool(pillar_w))
        # 找到杆后停止
        ptcl.clear()
        ptcl.send()
        # 在杆前面暂停5秒，日后可以添加距离校正等功能
        time.sleep(5)
        
        # 向右绕过第二根杆
        print('=== Cross Second Pillar ===')
        ret, frame = capture.read()
        pillar_x, pillar_w = find_pillar(frame)
        second_pillar.append(bool(pillar_w))
        while(second_pillar.has(True)):
            print('Still has pillar 2')
            ptcl.clear()
            ptcl.set_vel(y=15) # 向右移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            second_pillar.append(bool(pillar_w))
        # 找不到杆后停止
        ptcl.clear()
        ptcl.send()

        # 预留舵机方向调整代码

        # 向前飞过第三根杆直至视野中看不到第三根杆，然后停止（第三和后面的第四第五都是虚拟的）
        print('=== Cross Third Pillar ===')
        ret, frame = capture.read()
        pillar_x, pillar_w = find_pillar(frame)
        second_pillar.append(bool(pillar_w))
        while(second_pillar.has(True)):
            print('Still has pillar 2')
            ptcl.clear()
            ptcl.set_vel(x=15) # 向前移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            second_pillar.append(bool(pillar_w))
        # 找不到杆后停止
        ptcl.clear()
        ptcl.send()

        # 预留舵机方向调整代码
        # 暂停3秒，直至舵机方向调整完成
        time.sleep(3)


        # 开始找第四根杆
        print('=== Finding Forth Pillar===')
        ret, frame = capture.read()
        first_pillar = AverageFilter(10)
        [first_pillar.append(False) for i in range(10) ]
        pillar_x, pillar_w = find_pillar(frame)
        first_pillar.append(bool(pillar_w))
        while(first_pillar.has(False)): # 若画面中没有杆
            print('Can\'t find pillar 1')
            ptcl.clear()
            ptcl.set_vel(y=-15) # 向左移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            first_pillar.append(bool(pillar_w))
        # 找到杆后停止
        ptcl.clear()
        ptcl.send()
        # 在杆前面暂停5秒，日后可以添加距离校正等功能
        time.sleep(5)
        
        # 向左绕过第四根杆
        print('=== Cross Forth Pillar ===')
        ret, frame = capture.read()
        pillar_x, pillar_w = find_pillar(frame)
        first_pillar.append(bool(pillar_w))
        print(first_pillar.values)
        while(first_pillar.has(True)):
            print('Still has pillar 1')
            ptcl.clear()
            ptcl.set_vel(y=-15) # 向左移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            first_pillar.append(bool(pillar_w))
        # 找不到杆后停止
        ptcl.clear()
        ptcl.send()

        # 开始找第五根杆
        print('=== Finding Fifth Pillar===')
        ret, frame = capture.read()
        second_pillar = AverageFilter(10)
        [second_pillar.append(False) for i in range(10) ]
        pillar_x, pillar_w = find_pillar(frame)
        first_pillar.append(bool(pillar_w))
        while(second_pillar.has(False)): # 若画面中没有杆
            print('Can\'t find pillar 2')
            ptcl.clear()
            ptcl.set_vel(y=-15) # 向左移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            second_pillar.append(bool(pillar_w))
        # 找到杆后停止
        ptcl.clear()
        ptcl.send()
        # 在杆前面暂停5秒，日后可以添加距离校正等功能
        time.sleep(5)
        
        # 向右绕过第二根杆
        print('=== Cross Fifth Pillar ===')
        ret, frame = capture.read()
        pillar_x, pillar_w = find_pillar(frame)
        second_pillar.append(bool(pillar_w))
        while(second_pillar.has(True)):
            print('Still has pillar 2')
            ptcl.clear()
            ptcl.set_vel(y=-15) # 向右移动
            ptcl.send()

            ret, frame = capture.read()
            pillar_x, pillar_w = find_pillar(frame)
            second_pillar.append(bool(pillar_w))
        # 找不到杆后停止
        ptcl.clear()
        ptcl.send()
        
    except KeyboardInterrupt:
        # 停止
        print('=== Stop ===')
        ptcl.clear()
        ptcl.send()

        # 自动降落
        print('=== Auto Land ===')
        ptcl.clear()
        ptcl.set_motion('autoLand')
        ptcl.send()

        print("=== Mission Complete ===")

    # 停止
    print('=== Stop ===')
    ptcl.clear()
    ptcl.send()

    # 自动降落
    print('=== Auto Land ===')
    ptcl.clear()
    ptcl.set_motion('autoLand')
    ptcl.send()

    print("=== Mission Complete ===")

# Note: 
# For VIM3:
#   PIN15(RX) <---> TX
#   PIN16(TX) <---> RX

# bytes.fromhex('aa')

import wiringpi # 用于发送数据
import re # 用于打印信息
import time # 用于延时
# from ctypes import *

def int2char(num, length = 2):
    # 将一个有符号数用 uint8 的数组表示
    # 参数:
    #   num: 有符号数
    # 返回:
    #   char_list: uint8列表

    char_list = []
    for i in range(length):
        char_list.append(num&0xff)
        num = num>>8
    return char_list

def char2int(char_list):
    # 将一个 uint8 的数组转化为有符号数
    num = 0
    if char_list[-1] > 0x80:
        pass
    else:
        for i in char_list:
            num += i
            num = num << 8
    return num

class protocol(object):
    data_len = 15      # 帧长度
    data_start = 0xAA  # 帧起始符
    data_end = 0xBB    # 帧终止符
    data = []          # 帧

    data_mode = {'control': 0x41, 'motion': 0x51}
    data_motion = {'autoTakeOff': 2, 'autoLand':3, 'arm':4, 'disarm':5,'flyAroundRod':6}
    confict = {'autoTakeOff': 3, 'autoLand':2, 'arm':5, 'disarm':4, 'flyAroundRod': 5}


    def __init__(self, device='/dev/ttyS3', baud=115200):
        # 初始化串口
        # 参数:
        #   device: 串口对应的 Linux 设备
        #   baud: 波特率
        # 返回值:
        #   无

        self.device = device
        self.baud = baud
        self.serial = wiringpi.serialOpen(self.device, self.baud)
        print('- Init protocol done.')
        # self.data = self.construct_empty_data()

    def show_raw(self, data = data):
        print(data)
    
    def show(self, data = data):
        if len(data) != self.data_len:
            print('Invalid Data: Length of Data should be', self.data_len, 'not', len(data))
            return False
        if data[0] != self.data_start or data[data_len-1] != self.data_end:
            print('Invalid Data: the start byte and end byte should be', self.data_start, self.data_end, 'not', data[0], data[self.data_len-1])
            return False
        
        if data[1] == self.data_mode['control']:
            print("Mode: control")
        elif data[1] == self.data_mode['motion']:
            print("Mode: motion")
        else:
            print('Invalid Data: Data[1] should be', self.data_mode['control'], self.data_mode['motion'], 'not', data[1])

    def send(self, data = data, clear = False, need_check_sum = False):
        # 发送一帧数据
        # 参数:
        #   data: 数据（默认为类内的帧），可以是数组或 ascii 字符串
        #   clear: 发送后是否清空数据，默认 False
        # 返回值:
        #   Bool，表示成功与否

        if type(data) == list or type(data) == tuple:
            print('Send: ', end='')
            for char in data:
                wiringpi.serialPutchar(self.serial, char)
                print('%02X' %char, end=' ')
            
            if need_check_sum:
                wiringpi.serialPutchar(self.serial, self.check_sum(data))
                print('%02X' %(self.check_sum(data)))
            else:
                wiringpi.serialPutchar(self.serial, 0x00)
                print('%02X' %(0x00))
        elif type(data) == str:
            command = data
            wiringpi.serialPuts(self.serial, command)
            hex_string = bytes(command, 'UTF-8').hex()
            print('Send: ', re.sub(r'(?<=\w)(?=(?:\w\w)+$)', ' ', hex_string))
        elif type(data) == bytes:
            command = data.decode()
            wiringpi.serialPuts(self.serial, command)
            hex_string = bytes(command, 'UTF-8').hex()
            print('Send: ', re.sub(r'(?<=\w)(?=(?:\w\w)+$)', ' ', hex_string))
        else:
            print("Error: send() only accept list/str/bytes as data")
            return False
        

        if clear:
            data = construct_empty_data()

        return True

    def clear(self, data = data):
        # 清空帧（起始与终止符除外）
        # 参数:
        #   data: 数据（默认为类内的帧），可以是数组或 ascii 字符串
        # 返回值:
        #   data
        if len(data) == protocol.data_len:
            for i in range(1, protocol.data_len-1):
                data[i] = 0x00
        elif len(data) == 0:
            data+=self.construct_empty_data()
        else:
            data = self.construct_empty_data()
        return data

    def construct_empty_data(self):
        # 构造空数据（除起始与终止字节外，其余为0）
        # 参数:
        #   空
        # 返回:
        #   一帧空数据（除起始和终止，其他位置为 0x00）

        data = [0x00 for i in range(protocol.data_len)]
        data[0] = protocol.data_start
        data[protocol.data_len-1] = protocol.data_end
        
        return data


    def set_mode(self, mode, data = data):
        # 设置帧类型
        # 参数:
        #   mode: 模式，'control' 或 'motion'
        #   data: 待设置的帧（默认为类内的帧）
        # 返回:
        #   data

        if mode in ("control", "motion"):
            data[1] = protocol.data_mode[mode]
        else:
            print("Warning: set_mode() only accept two mode: 'control' or 'motion'")
        
        return data


    def set_motion(self, motion, data = data):
        # 设置自动动作
        # 参数:
        #   mode: 动作，'autoTakeOff', 'autoLand', 'arm', 'disarm'
        #   data: 待设置的帧（默认为类内的帧）
        # 返回:
        #   data
        # 注:
        #   1. 函数会自动设置 mode 为 motion
        #   2. 函数会自动清除冲突的动作

        self.set_mode("motion", data)
        if motion in ('autoTakeOff', 'autoLand', 'arm', 'disarm','flyAroundRod'):
            data[self.data_motion[motion]] = 0x01
            # 将相应的冲突动作置零，比如起飞，则会将降落命令置零
            data[self.confict[motion]] = 0x00
        else:
            print("Warning: set_motion() only accept",'autoTakeOff', 'autoLand', 'arm', 'disarm', 'flyAroundRod')
        
        return data
        

    def set_pos(self, roll=None, pitch=None, yaw=None, data = data):
        # 设置roll、pitch角度，和 yaw 速度
        # 参数:
        #   roll: roll 角
        #   pitch: pitch 角
        #   yaw: yaw 角速度
        #   data: 待设置的帧（默认为类内的帧）
        # 返回:
        #   data
        # 注:
        #   1. 函数会自动设置 mode 为 control

        data = self.set_mode("control", data)
        if roll!=None:
            char_list = int2char(int(roll))
            data[2] = char_list[0]
            data[3] = char_list[1]

        if pitch!=None:
            char_list = int2char(int(pitch))
            data[4] = char_list[0]
            data[5] = char_list[1]

        if yaw!=None:
            char_list = int2char(int(yaw))
            data[6] = char_list[0]
            data[7] = char_list[1]


    def set_vel(self, x=None, y=None, z=None, data = data):
        # 设置 x、y、z 速度
        # 参数:
        #   x: x 速度
        #   y: y 速度
        #   z: z 速度
        #   data: 待设置的帧（默认为类内的帧）
        # 返回:
        #   data
        # 注:
        #   1. 函数会自动设置 mode 为 control

        data = self.set_mode("control", data)
        if x!=None:
            char_list = int2char(int(x))
            data[8] = char_list[0]
            data[9] = char_list[1]

        if y!=None:
            char_list = int2char(int(y))
            data[10] = char_list[0]
            data[11] = char_list[1]

        if z!=None:
            char_list = int2char(int(z))
            data[12] = char_list[0]
            data[13] = char_list[1]


    def check_sum(self, data = data):
        sum = 0
        for i in data:
            sum += i
        return sum&0xff

    def close(self):
        wiringpi.serialClose(self.serial)

if __name__ == "__main__":
    ptcl = protocol(device='/dev/ttyUSB0', baud=115200)
    ptcl.clear()
    ptcl.set_motion('arm')
    print('arm', ptcl.send())

    time.sleep(10)

    ptcl.clear()
    ptcl.set_motion('autoTakeOff')
    print('autoTakeOff',ptcl.send())

    time.sleep(10)

    ptcl.clear()
    ptcl.set_motion('autoLand')
    print('autoLand',ptcl.send())
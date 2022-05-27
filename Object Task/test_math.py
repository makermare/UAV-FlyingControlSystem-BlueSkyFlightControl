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

filter = AverageFilter(3)
while(True):
    value = bool(int(input('Get Input:')))
    filter.append(value)
    print(filter.has(False))
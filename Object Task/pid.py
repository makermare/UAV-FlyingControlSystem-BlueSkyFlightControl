import time

class PID(object):

    def __init__(self, P, I, D, SetPoint):
        self.Kp = P
        self.Ki = I
        self.Kd = D
        self.sample_time = 0.00
        self.current_time = time.time()
        self.last_time = self.current_time
        self.clear()
        self.SetPoint = SetPoint

    def clear(self):
        self.SetPoint = 0.0
        self.PTerm = 0.0
        self.ITerm = 0.0
        self.DTerm = 0.0
        self.last_error = 0.0
        self.int_error = 0.0
        self.output = 0.0

    def update(self, feedback_value):
        error = self.SetPoint - feedback_value
        self.current_time = time.time()
        delta_time = self.current_time - self.last_time
        delta_error = error - self.last_error
        if (delta_time >= self.sample_time):
            self.PTerm = self.Kp * error#比例
            self.ITerm += error * delta_time#积分
            self.DTerm = 0.0
            if delta_time > 0:
                self.DTerm = delta_error / delta_time#微分
            self.last_time = self.current_time
            self.last_error = error
            self.output = self.PTerm + (self.Ki * self.ITerm) + (self.Kd * self.DTerm)
            return self.output
        else:
            return False

    def setSampleTime(self, sample_time):
        self.sample_time = sample_time

    def setSetPoint(self, SetPoint):
        self.SetPoint = SetPoint

def clip(value, limit):
    if abs(value)>limit:
        if value>0:
            return limit
        else:
            return -limit
    else:
        return value
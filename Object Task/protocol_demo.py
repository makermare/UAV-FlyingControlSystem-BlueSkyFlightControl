from protocol import protocol


if __name__ == "__main__":
    ptcl = protocol()

    ptcl.clear()
    ptcl.set_motion('disarm')
    ptcl.send(need_check_sum = False)

    ptcl.clear()
    ptcl.set_motion('autoTakeOff')
    ptcl.send(need_check_sum = False)

    ptcl.clear()
    ptcl.set_vel(x=0x1111, y=0x2222, z=0x3333)
    ptcl.send(need_check_sum = False)

    ptcl.clear()
    ptcl.set_pos(roll=0x1111, pitch=0x2222, yaw=0x3333)
    ptcl.send(need_check_sum = False)

    ptcl.clear()
    ptcl.set_motion('autoLand')
    ptcl.send(need_check_sum = False)
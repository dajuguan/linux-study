import pty
import os

def calculate_expression():
    master, slave = pty.openpty()
    pid = os.fork()

    if pid == 0:  # 子进程
        os.close(master)
        os.dup2(0, slave)  # 将子进程的标准输入重定向到伪终端
        os.dup2(1, slave)  # 将子进程的标准输出重定向到伪终端
        os.execlp('/usr/bin/bc',"-q")  # 在子进程中启动 /bin/bc
    else:  # 父进程
        os.close(slave)
        os.dup2(master, 0)
        os.dup2(master, 1)

        os.waitpid(pid, 0)

# 测试计算表达式
result = calculate_expression()

import os
import subprocess
import tkinter as tk
import signal
import sys


def start_mapping():
    command = "ros2 launch my_robot_bringup main__mapping.launch.py"
    process = subprocess.Popen(command, shell=True)
    

def control_robot_manually():
    command = "ros2 run turtlebot3_teleop teleop_keyboard"
    process = subprocess.Popen(command, shell=True)
    

def save_map():
    command = "ros2 run nav2_map_server map_saver_cli -f ros2_ws/src/my_robot_bringup/maps/my_map"
    subprocess.Popen(command, shell=True)


def start_navigation():
    # Terminate all running processes
    
    # When these are terminated, navigation is not worked because some background processes are also killed.
    #subprocess.Popen("pkill -SIGINT -f 'ros2 launch my_robot_bringup main__mapping.launch.py'", shell=True)
    #subprocess.Popen("pkill -SIGINT -f 'ros2 run turtlebot3_teleop teleop_keyboard'", shell=True)

    command = "ros2 launch my_robot_bringup main__navigation.launch.py"
    process = subprocess.Popen(command, shell=True)
    


def exit_program():
    # Terminate all running processes
    subprocess.Popen("pkill -SIGINT -f 'ros2 launch my_robot_bringup main__mapping.launch.py'", shell=True)
    subprocess.Popen("pkill -SIGINT -f 'ros2 run turtlebot3_teleop teleop_keyboard'", shell=True)
    subprocess.Popen("pkill -SIGINT -f 'ros2 launch my_robot_bringup main__navigation.launch.py'", shell=True)
    
    # Exit the application
    root.destroy()

# Commands to execute when the program starts
startup_commands = [
    "cd ros2_ws && colcon build && source install/setup.bash"
]

for command in startup_commands:
    subprocess.run(command, shell=True)

# Create the main application window
root = tk.Tk()
root.title("ROS2 Control Panel")

# Create buttons for each action
mapping_button = tk.Button(root, text="Start Mapping", command=start_mapping)
mapping_button.pack(pady=5)

control_button = tk.Button(root, text="Control Robot Manually", command=control_robot_manually)
control_button.pack(pady=5)

save_map_button = tk.Button(root, text="Save Map", command=save_map)
save_map_button.pack(pady=5)

navigation_button = tk.Button(root, text="Start Navigation", command=start_navigation)
navigation_button.pack(pady=5)

# Create an Exit button
exit_button = tk.Button(root, text="Exit", command=exit_program)
exit_button.pack(pady=5)

# Run the application's event loop
root.mainloop()


# Simple Two-Dimensional Robot Control
### Lab 4 Wall Avoiding Robot

This robot avoids walls using a few different techniques.

First, it checks that there is a clear path directly in front of it. If there is, it allows the users input to pass directly to the robot. If there is no clear path ahead (it checks slightly to the left and slightly to the right) it continues to the next step.

If there is no path, it checks every lidar line for the longest path and gets the angle and direction. It then forces the robot to stop and rotates it to face the direction of the longest path.

# Launch Files

## Simple launch

Launch the lab 4 robot into the default robot0 namespace.
```
roslaunch lab_4_robot simple.launch
```

Launch the lab 4 robot into some other namespace.
```
roslaunch lab_4_robot simple.launch ns:=some_namespace
```

## Advanced launch

Launch the lab 4 robot into the default robot0 namespace and open the RQT interface and the STDR Simulator.

```
roslaunch lab_4_robot advanced.launch
```

Launch the lab 4 robot with the interfaces into some other namespace.
```
roslaunch lab_4_robot advanced.launch ns:=some_namespace
```

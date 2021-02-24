# UKMARSEY controllers

## Configuration

At the time of writing, the ukmarsey code provides speed control of the motors by means of a pair of speed controllers. Rather than having individual speed controllers for each wheel, the code takes a more integrated view. There is one speed controller for forward motion and one for rotation. These can be set independently and their outputs are combined to provide the individual wheel drive voltages.

This type of control is sometimes called 'vee-omega' control where v is forward velocity amd omega is angular or rotational velocity. It should be clear that the robot can be commanded to move in a straight line by setting forward velocity to some value while keeping the rotational velocity at zero. by the same token, if forward velocity is zero and rotational velocity is set to some fixed value, the robot will spin on the spot. A combination of forward and rotational speeds is used to make the robot move in a constant, circular arc. Conveniently, the relationship between the two speeds determines the radius of that arc, independetly of the configuration of the robot. Even better, the relationship is fixed. That means that you can run the same radius arc at twice the forward speed so long as you also double the rotational speed. That is where the real benefit of the technique is found. the robot code takes care of how fast each wheel must turn, you just have to provide two speeds and you will know the radius of the resulting curve.

For the convenience of humans, the units used are mm/second for forward velocities (negative values move the robot backwards) and rotational velocities are expressed in degrees/second. For positive rtational velocities the robot will rotate _anti-clockwise_.

## PID Control

Each of these speed controllers is implemented using the common PID control technique. In this case, it is sufficient to only use Proportional with Intergral (PI) control and so the Derivative (D) constant is left at zero.

the code used to implement the PID controllers is slightly adapted form the Arduino PID library. This excelent and widely used library is described in detail here:

http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/

Rather than install the library in the normal way, the files have been copied into the project and modified a little to make them more compatible with the rest of the software model. In particular, the compute() mthod is always called at the same fixed rate in ukmarsey whereas the library code has to be able to be called irregularly but frequently. We are able to make a simplification for our use case. There are some other minor changes for consistency and code style as well.

## Feed Forward

While PID controllers are often used for a whole range of control tasks, it would be unusual if the control engineer or designer did not have at least some idea of the drive signals needed to get a particular output response even without the controller. In the case of UKMARSBOT, the wheels are driven by DC motors and it is easy enough to do some tests and work out what voltage is needed to drive the robot at a given spee. In fact, because DC motors have well-known characteristic, you can easily write a simple equation that lets you calculate the correct voltage for any speed. Better yet, you can have the robot controller work it out for your. In that way, if the main control code wants to move at a particular speed, the motor control code can calculate and apply a suitable voltage. This kind of control is called _feedforward control_ and can be used to have the robot move quite reliabley and predictably. Without realising it, you have probably already used feedforward control whenever you just set a PWM value and expect the robot to move at a particular speed.

Unfortunately, feedforward control cannot deal with changes in load or other conditions and so the PID controller gives a helping hand to make sure that everything behaves as well as it possibly can. For the PID contyroller to do its best job, it must be tuned for each robot. If the robot configuration changes - new motors, extra weight, different wheels etc - then both the feedforward and PID controllers will need to be adjusted.

## Tuning

To work well, PID controllers must be 'tuned'. That is, specific values need to be set for the controller constants. The values found in the sample configuration file should provide a reasonable starting point but each robot is different and will need the values setting up for that specific robot. A separate document describes how you might go about tuning the controllers.
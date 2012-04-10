##Assignment 2: Out for a Drive



###Your assignment is to make a little 3D car that drives around a stage and is viewed from a eleveated camera position.

OpenGL components you are expected to use:
Keyboard listeners
Rendering primatives
Model Transformations
What your program should do:
You should draw a little car with a body and four wheels sitting on a stage. A person's head should be sticking out of the sunroof of the car and should have two eyes and what ever other facial features you want. Your car's wheels should have spokes or lugnuts or some other feature so that it is clear when your wheels are spinning and when they are stationary.

Your car's driver should be capable of turning their head left and right, the car's front wheels should be capable of steering left or right, and your car should be able to drive forward and backward (in the direction the front wheels are pointed).  When the car is moving forward or backward, the wheels should spin in the appropriate direction.  When the car is stationary, the wheels should not spin.

Your car should stop driving automatically when it hits the edge of the stage.

Your program should accept the following keyboard inputs:

The space bar should stop your car from driving
The up and down arrow keys should make the car drive forward and backward respectively.  Once the car is in motion, it should remain in motion until something stops it (hitting the edge of the stage, or another button press).  You should not have to keep the key held down to keep the car in motion.
The left and right arrow keys should make your car steer left and right (not as a binary decision, but by progressively turning the front wheels)
The 'z' and 'x' keys should make your person's head rotate left and right
Other requirements:
You should make each of the four sides of your car a different color for easy identification
You should have eyes on your person's head so you can tell what direction they are looking
You should have a large enough stage for your car to be able to drive and steer around a bit before hitting the edge.
General Tips:

You may make use of Angel's matrix stack code if you wish. You can find it in Section 8.4.1, or just download it as part of the transformation code that we used in our in-class exercise.
Since we won't talk about viewing until quite late into this project, you'll probably just want to make use of the viewing code from our in class exercise.  You'll want to keep all of your vertex coordinates between, say, -50 and 50 and you should be OK. 
Project 3 will add better camera and viewing controls.
Angel has helpfully provided a LookAt() function to help you get your camera positioned and pointed in the right place.

Grading Criteria:
30 pts: Correct rendering of all car, head parts and stage
35 pts: Correct transformations
10 pts: Correct handling of keyboard input
15 pts: Correct car behavior (stop at edge of stage, roll wheels, etc)
10 pts: Good coding style and design

Things to watch out for:

You must use OpenGL 3.2+ code, including shaders and Buffer Objects. Your shaders will be quite simple for this assignment, but we will be extending this project in the future
Make sure you have your transformation heirarchy set up correctly. Failure to do so will cause incorrect movements
Your heirarchy for your car is

front wheels - spokes
/
car body- head - eyes
\
back wheels - spokes

Thus, anything that happens to the car body also happens to the head, and therefore the eyes, but things that happen to the head should not affect the car body.


Suggested Order for Development

1. Start by drawing the stage with a stationary camera showing the whole stage
2. Next, render the car sitting on the stage with head and wheels
3. Next, implement rotating in place for the car wheels and head
4. Next, implement wheel rolling
5. Next, implement actual driving
6. Finally, implement correct behavior as described above
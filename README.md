# # Snow Fall Particle System 

_Rain, snow flurries_

The objective is to develop a system to generate animation of snow.


![classFlowChart](https://github.com/NCCA/cfgaa24programingassignment-JakeHo0310/assets/160144686/01a674ad-63a0-48ef-bfa6-f81165dcd324)




I have done research on snow movements, how to flow and interact with air resistance. 
I came across Niniane Wang, an American software engineer and technology executive, who used a technique 
to render rain and snow in Microsoft Combat Flight Simulator 3 and Microsoft Flight Simulator 2004: A Century of Flight.
She explained how to map snow textures onto a double cone, orienting around a camera inside so it follows the camera,
showing snow textures on the screen. I created a class for Generating vertices and calculated how to implement the math 
for creating a double cone shape. I then realised her method of rendering snow isn't exactly a particle system, so I 
decided to take a look at perlin noise.

Perlin noise is used in computer graphics and procedural generation to create natural-looking textures. For instance, 
snow. I first made a 3D space which I call the flow field. The flow field is essentially a three-dimensional grid, and
at each point in this grid, Perlin noise is used to determine the flow direction of each snow particle. Perlin noise 
generates values in the range of -1 to 1, this range is achieved through a combination of noise functions and 
interpolation techniques, resulting in smoothly varying values that oscillate around zero.

3D perlin noise within a flow field grid:
![img_1.png](img_1.png)


Examining the effects of my perlin noise, the particles are not flowing as smooth as i expected, so I created a simple
smoothing technique applied to Perlin noise values to reduce abrupt changes and create smoother flow fields. 
By averaging the Perlin noise values over neighboring samples, this smoothing process helps to reduce high-frequency 
noise and create a more coherent and gradual transition between adjacent positions in the flow field.

For the snow texture, I used the gimp software to draw a snowflake texture. I created particle fragment and vertex 
shaders and applied the png image on my particles. I also created different keys to change the colour of the particles.
For example, the default colour is light blue for key 1, 2 for white, 3 for red, 4 for constant changing rainbow 
snowflakes and 5 for black and white background.

Creating a 3D gradient background was also in my initial plan, I created a Background fragment and vertex shaders, I
tried to add a quad in my scene and apply a colour gradient on it, unfortunately I didn't get it to work, I had some 
issues with referencing the shaders.


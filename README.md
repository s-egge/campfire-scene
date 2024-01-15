The Scene
---

The scene is a camping area that cycles between day and night time. When the night arrives, "fireflies" appear and the campfire starts. 


This was made for the final project in an Introduction to Graphics class taught by [Professor Mike Bailey](https://web.engr.oregonstate.edu/~mjb/WebMjb/mjb.html). The `original files upload` commit is starter code that was provided to the class, while the next commit has my changes that made the scene for the final project. There were several more helper files provided to class, for things like loading object files and bmp files to use as textures, that are not included in this repo. The object files and texture files are not included either, but their sources are linked below.


https://github.com/s-egge/campfire-scene/assets/102624422/da5fbf2b-cea8-476e-8701-182eade4c016


__Environment__

The sky is a large sphere that rotates on a 40-second loop. It is textured with two different texture files, a night sky and a day sky, with the meeting edges smoothed. Cutting the sphere in half is a flat square textured with a dirt image. The rest of the scene sits on top of this square. The trees are a single object file that's repeated in a double for-loop.

__Grass__

The grass is made up of many instances of a single Blade, a Vertex Buffer Object of a line with several points to allow it to bend per the shader that it's passed through. The shader spreads the blades of grass throughout the scene, leaving the circle in the middle clear. It also makes the grass "wave" back and forth as the scene loops. 

__Fireflies__

The fireflies are small spheres. The FireFly class holds variables that initialize the firefly in a random space in the environement, and gives them a random variation in their movement and glow cycle. Each firefly is passed through a shader that applies the glow affect based on their glow variation. This keeps the fireflies from looking too uniform. 

__Fire__

The Fire class utilizes a FireParticle struct to create a fire. Each particle has a xyz coordinates, a randomized change in these coordinates, and a lifetime. These particles are held in a vector that is initialized and updated throughout the fire's lifecycle. In order to smooth the "starting" and "extinguishing" of the fire, the initial number of particles starts low and is added to over time until the max number of particles are reached. The color of the particle ranges from red to yellow and is based on the distance from the origin in order to simulate a more natural fire color. 


Sources
---
__Object Files__
* [Tent](https://www.turbosquid.com/3d-models/camping-tent-model-2023671)
* [Campfire](https://free3d.com/3d-model/low-poly-fire-camp-182212.html)
* [Trees](https://free3d.com/3d-model/blender-lowpoly-nature-assets-pack-36502.html)

__Texture Files__
* [Moon and stars](https://www.solarsystemscope.com/textures/)
* [Sky and dirt](https://www.textures.com/)

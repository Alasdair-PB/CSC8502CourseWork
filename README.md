CSC8502 Coursework 2024/25
Alasdair Pilmore-Bedford

Since submission code has been updated to include particles
Video Link: 

Lighting, Tess & Geometry showcase: https://youtu.be/KN5iCqhPOUs 

Particle Render showcase: https://youtu.be/kmshfE3gGJ4

Controls:: Keyboard:1:  Exit Camera track, W:S:A:D: move camera

Key Features
A: Leaves falling from trees as the temperature gets colder and then regrowing when it gets hotter once more. Icicles also hang from the trees in colder weather before melting away. Animation is also included outside of geometry shaders as the runner’s First person hands are visible whenever the camera moves- done using skeletal animation. 

B: Water tessellation, a depth map is used to simulate ice enclosing in from the shallower portions of the lakes. Tessellation is also used to create more accurate water displacements as the water rises as falls by its normal map height. A single plane is tessellated into multiple vertices in this scene. 

C: Deferred rendering is used to support multiple sphere lights that blend together a cool blue and a warm orange hue as the temperature changes.  A Post processing pass is also added to support fog in the distance 

D: A single light support implementation of shadow mapping- visible in every screenshot, but made obvious here as I removed the other buffer/post processing writes for this image. A single shadow map is used for the full terrain. 

E: Particle rendering, snowflakes fall from the sky in instance calls, rendering 500 snowflakes each frame

Material References::

All models were created by myself: FPS hands, rocks, grass, tree along with some of their textures (leaves, grass)

Other textures were sources from a Unity asset pack I’d previously purchased- PBR Material Bundle Vol1 (445) By Cole: https://assetstore.unity.com/packages/2d/textures-materials/pbr-material-bundle-vol1-445-128817 

And all others were provided by this course

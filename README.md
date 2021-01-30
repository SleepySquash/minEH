# minEH
minEH (minimal Engine Holder) is a game engine that contains window and events abstraction (only Cocoa for now), renderer (OpenGL and Vulkan - you can choose one simply at renderer's context creation time: GL::Context or Vk::Context), graphics primitives (currently 2D Sprite, 3D Camera and 3D Mesh), entity component composition system and audio system. Networking system will be supported as well in the future.

## Dependencies:
1. Renderer: OpenGL or Vulkan (or both) required. You can specify which renderer you will use and which one you won't by commenting and uncommenting defines in the Support/Config.hpp file
1. Audio: libsndfile and PortAudio
1. Graphics: stb_image.h and tinyobjloader.h
1. Maths: glm

## Building
Building for now supported only in Xcode (macOS), but you probably might build the project via command line tools on macOS.
Other operating systems will be supported as I get into them.

### macOS
Open the Xcode project, sign it up with your Apple profile. You might have to do some painful stuff with handling the libsndfile, portaudio or vulkan. I'll describe a better step through later.

## Examples
Example of usage is for now embedded into the project - App subfolder (main.cpp and some components) is the example.

## Window and events
Window is an abstract entity which has really intuitive interface. In order to create one, you simply create a Window and then call the open function:
```
Window window;
window.open();
```
Before opening the window you can specify its dimentions and title.
Closing the window is straightforward: `Window.close();`

Events are poll loop based. So you should place the poll loop inside `while (window.isOpen) { }` loop.
```
Event event;
while (window.isOpen)
{
    if (window.pollEvent(event))
    {
        switch (event.type)
        {
            case Event::Type::Closed: window.close(); break;
            case Event::Type::Resized: window.resize(event); break;
            default: break;
        }
    }
}
```
Above is the minimum code needed for the window to properly work.

### Supported OS
1. macOS (via Cocoa)
1. iOS (via Cocoa)
1. ... **definitely** will be supported: Windows, Android, UNIX.

### Known bugs
1. setTitle() leaks memory :(

## Renderer
The window's content is undefined unless you bind a Renderer to it.
Renderer is an abstract class which handles all the calls to your GPU in order to render the window's content.

First you need to create the context object:
```
GL::Context context;
```
```
Vk::Context context;
```
All the calls to the context are abstract except these ones above - you have to specify which Renderer's implementation you want to use: GL for OpenGL or Vk for Vulkan.

Binding is simple: `context.create(&window);`

Now in order for the application to works you need to set up the following code structure in you app:
```
while (window.isOpen)
{
    if (window.pollEvent(event))
    {
        // Event loop
        switch (event.type)
        {
            ...
            case Event::Type::Resized: ... context.resize(); ...
            ...
        }
    }
        
    ... // Update loop might be there
    
    // Draw loop
    uint32_t i = context.beginDraw(); // Aquire the image's number which is going to be rendered (application might have double or triple buffering)
    context.beginRecord(i);
    ... // Record commands
    context.endRecord(i);
    
    ... // Draw commands

    context.endDraw(i); // Push the commands to the GPU
    window.display(); // Update the window's content
}
context.wait(); // Wait for the GPU to stop executing commands
... // Destroy commands
context.destroy();
```
Now the concept behind Record and Draw commands should be explained.
Recording means forming the commands which will be sent to the GPU.
Drawing means not actually drawing but updating the buffers (variables) which are stored on the GPU.
You shouldn't really understand what's what. You should only know that the Draw cycle consists of these two parts and that the primitives that are going to be described next (Sprite and Mesh for example) need to call .record() and .draw() functions they have.

### Backends
1. OpenGL
1. Vulkan
1. ... coming in the future: Metal, DirectX 11/12, WebGL (via Emscripten)

### Supported OS
1. Windows (Vulkan, OpenGL)
1. macOS (Vulkan, OpenGL)
1. iOS (Vulkan, OpenGL?)
1. Android (Vulkan, OpenGL?)
1. UNIX (Vulkan, OpenGL)
1. ... etc since the compatability is defined by the backend which is being used.

## Collectors
Collectors are just static classes that hold specified resources.
For example TextureCollector holds textures, BufferCollectors holds buffers etc.

In order to obtain a resource you call the Collector::get("path or name") function. In order to release a resource you call the Collector::erase("path or name") function.
The usage of every resource is being tracked and once it drops to 0 the resource is freed.

### Collectors present
1. TextureCollector (tc) - textures.
1. BufferCollector (bc) - buffers.
1. ShaderCollector (sc) - not really a collector, but holds OpenGL shaders.
1. DescriptorCollector (dc) - Vulkan descriptors.
1. PipelineCollector (pc) - Vulkan pipelines.

## Graphics
So the engine can render 2D and 3D graphics.
First I should describe the Factory idea I'm using in order to be able to abstract all the Renderer-dependent code. Factory is just a collection of functions like:
```
Sprite* Graphics::GetSprite(context)
Camera* Graphics::GetCamera(context)
Mesh* Graphics::GetMesh(context)
```
that return an abstract graphics object but in fact these functions create Renderer-dependent graphics object and return only the abstraction (since every abstract graphics object is a parent of the renderer-dependent) based on the context that is being parsed inside.
So in order to create a graphics primivite you should call the Factory's method, store the pointer in your object or application and once it became obsolete (the object that used sprite is about to get destroyed, for example) you **have to delete** the pointer (`delete ptr;`).

For now only Sprite on the 2D side is implemented. Sprite is just a single quad with the texture. It has no depth testing. Texture is beind loaded through setTexture("path") method and is released automaticly when the sprite is destroyed.
Sprite on init should always call create() and on destroy - destroy(). Don't forget to `delete sprite;` pointer as well.
On the resize event sprite should call resize(). In the record loop it should call record() and on the draw loop - draw().
```
Sprite sprite;
sprite.setTexture("image.jpg");
sprite.create();
...
while (window.isOpen)
{
    if (window.pollEvent(event))
    {
        // Event loop
        switch (event.type)
        {
            ...
            case Event::Type::Resized: ... sprite.resize(); ...
            ...
        }
    }

    // Draw loop
    uint32_t i = context.beginDraw();
    context.beginRecord(i);
    sprite.record(i);
    context.endRecord(i);
    
    // Draw commands
    sprite.draw();

    context.endDraw(i); // Push the commands to the GPU
    window.display(); // Update the window's content
}
...
sprite.destroy();
delete sprite;
...
```

In order to do some 3D you first need to create the Camera and set it to the context thus making it the active camera:
```
Camera* camera = Graphics::GetCamera(&context);
context.camera = camera;
```

The Camera class also need to be created, updated, drawed (buffers get updated there) and destroyed.
But you might be interested in creating a camera controller object, which will be handling all the movement and the view changes as well as all the specified above actions. In the component system this engine has FreeCamera component presented in the Engine/Components/CameraController.hpp - you can take a look.

Then you can create 3D Meshes which are identical in use to the Sprites but also has setModel("path") method which loads the 3D model (.obj only for now).

### Formats supported
1. Images: .jpg, .png, .bmp and more (stb_image).
1. Models: .obj (tinyobjloader)

## Component system
Component system is the Composition class which contains a list of Entities. An Entity has a list of Components. And the Component is the virtual class that has onInit(), onDestroy(), onUpdate(float), onEvent(Event), onRecord(uint32_t), onDraw() virtual methods that can be overriden.

```
Composition comp;
Entity* entity = comp.addEntity();
auto component = entity->addComponent<YourComponentClass>(your component CTOR's args);
```

For the composition to work in the usual application cycle:
```
Composition composition; // Init
...
while (window.isOpen)
{
    if (window.pollEvent(event))
    {
        switch (event.type)
        {
            ...
        }
        composition.event(event); // Event loop
    }
    
    composition.update(clock.restart()); // Update loop
    
    uint32_t i = context.beginDraw();
    
    context.beginRecord(i);
    composition.record(i); // Record loop
    context.endRecord(i);
    
    composition.draw(); // Draw loop
    
    context.endDraw(i);
    window.display();
}
...
composition.destroy(); // Destroy
...
```

## Audio
For now there's only Audio class which is the audio stream.
First you need to `Audio::Init();` on the start of your application.
And on the termination you should call `Audio::Destroy();`.

Playing audio is straightforward
```
Audio music;
music.open("music.ogg");

if (music.isOpen)
{
    music.setPosition(10.f); // in seconds
    music.setLooped(true);
    music.play();
    
    while (music.isPlaying())
        std::cout << music.getPosition() << "/" << music.duration << "\n";
}
```

Audio is being played in the same thread but async, so performance shouldn't suffer unless you're heavy loading the CPU. I'll be playing audio in the seperate thread in the future.

### Supported formas
1. Depends on the sndfile: WAV, OGG, FLAC and many more.
1. MP3 is not supported due to the licence problems.

### Supported OS
1. All specified in the PortAudio and sndfile libraries.

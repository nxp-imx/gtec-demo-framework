# NativeGraphicsService.GLES3

A implementation of the INativeGraphicsService service interface for OpenGL ES 3.

Method | Functionality
--- | ---
CreateTexture2D | Create a Texture2D instance.
Capture | Capture a screen shot.
CreateBasic2D | A INativeGraphicsBasic2D instance that can be used to draw points and strings.
CreateNativeBatch2D | A INativeBatch2D instance implemented with NativeBatch2D, this can be used to easily draw textures and bitmap fonts.

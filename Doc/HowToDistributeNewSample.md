# How to distribute a new sample

This describes a simple process for how to distribute a new sample that depends on the demo-framework to allow others to compile and run it.

## Develop

Go to github and [download](https://github.com/nxp-imx/gtec-demo-framework/releases) the latest release and then follow the standard process for creating a new app of the desired type. For this you should follow one of the ```Doc/Setup_guide_X``` documents found in the demo-framework repository. For example: [Setup_guide_windows.md](./Setup_guide_windows.md).

Once the sample has been created it's a good idea to follow the [CONTRIBUTING.md](../CONTRIBUTING.md) recommendations as that ensures we provide a higher quality sample and it will also make it easier to integrate the sample in a future version of the framework if it fits.

## Release

To release the new app:

1. Ensure that you follow all the legal requirements for sending code, images and other resources to third parties.
2. Ensure you remove all compiled files to ensure we only send whats needed to compile the sample.
3. Pack the new sample directory.
4. Inform the recipient
    1. Tell the recipient to download the demo-framework release used to develop the sample from github.
    2. Ask them to read the setup guide for their platform.
    3. Send the packed sample and tell the recipient which path to place the packed sample at inside the downloaded demo-framework repository (same location as it was developed in. For example: ```DemoApps/Vulkan/NewSample```.
    4. Ask them to use the "To Compile and install an existing sample application" section of the setup guide to build and install the sample. For example to build and install a wayland build:

        ```bash
        cd DemoApps/Vulkan/NewSample
        FslBuild.py --Variants [WindowSystem=Wayland] -c install --CMakeInstallPrefix $FSL_GRAPHICS_SDK/bin
        ```

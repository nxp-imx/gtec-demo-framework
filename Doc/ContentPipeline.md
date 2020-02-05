# Content Pipeline

*Beware, the content pipeline is currenly in development*

The FslBuild toolchain has a built-in content pipeline. It per default scans the content of `Content.bld` and
recursively tries to process all files found under it.

- The `.Config/FslBuildGen.xml` contains a set of content builders that are invoked based on the file extension and other parameters.
- Each processed file is copied to its corresponding location under `Content`
  - So for a Vulkan app the file at `Content.bld/test/test.frag` is processed and stored at `Content/test/test.frag.spv`
- To allow for more complex content use cases and reuse the `Content.bld/Content.json` file is opened if it exists.
  The file can contain multiple `ContentBuild.Sync` sections that pulls in files from 'external' locations.
  For example this script pulls in two files from `${ProjectRoot}/data`, the files are processed by the tool-chain and
  stored at the corresponding location in `Content`.
    ```json
    {
      "ContentBuilder" : {
        "ContentBuild.Sync": {
          "SourcePath": "${ProjectRoot}/data",
          "Content": [
            "shaders/base/textoverlay.frag",
            "shaders/base/textoverlay.vert"
          ]
        }
      }
    }
    ```
  In this case

    ```
    Content/shaders/base/textoverlay.frag.spv
    Content/shaders/base/textoverlay.vert.spv
    ```

The `ContentBuild.Sync` section can also contain wildcards in its file names like here:

```json
{
  "ContentBuilder" : {
    "ContentBuild.Sync": {
        "SourcePath": "${ProjectRoot}/data",
        "Content": [
        "shaders/base/textoverlay.frag",
        "shaders/base/textoverlay.vert",
        "shaders/bloom/*.vert",
        "shaders/bloom/*.frag"
        ]
    }
  }
}
```

Copy files from a shared resource area:

```json
{
  "ContentBuilder" : {
    "Content.Sync": {
      "SourcePath" : "${ProjectRoot}/Resources",
      "Content": [
        "Models/Dragon/*.*",
        "Textures/Circles/*.*"
      ]
    }
  }
}
```

Copy from multiple source areas using a sync list

```json
{
  "ContentBuilder" : {
    "Version": "1",
    "Content.SyncList": [
      {
        "SourcePath" : "${ProjectRoot}/Resources",
        "Content": [
          "Models/FuturisticCar/Futuristic_Car_C.jpg",
          "Models/FuturisticCar/Futuristic_Car_N.jpg",
          "Models/FuturisticCar/Futuristic_Car_S.jpg",
          "Models/FuturisticCar/FuturisticCar.fsf",
          "Models/FuturisticCar/License.json",
        ]
      },
      {
        "SourcePath" : "${ProjectRoot}/Resources/Source/Bloom",
        "Content": [
          "UITextureAtlas/License.json",
          "UITextureAtlas/MainAtlas.bta",
          "UITextureAtlas/MainAtlas.fbk",
          "UITextureAtlas/MainAtlas.png"
        ]
      }
    ]
  }
}
```


## Valid SourcePath variables

Name            | Location
----------------|----------
$\{Content.bld} | This package's 'Content.bld' directory (This is the default value if SourcePath is not specified)
$\{Content}     | This package's 'Content' directory
$\{ProjectRoot} | The project root of this package.

## Requested features

- Build texture atlases.
- Generate bitmap font files.

# Content Pipeline

*Beware, the content pipeline is currenly in development*

The FslBuild toolchain has a built-in content pipeline. It per default scans the content of `Content.bld` and
recursively tries to process all files found under it.

- The `.Config/FslBuildGen.xml` contains a set of content builders that are invoked based on the file extension and other parameters.
- Each processed file is copied to its corrosponding location under `Content`
  - So for a vulkan app the file at `Content.bld/test/test.frag` is processed and stored at `Content/test/test.frag.spv`
- To allow for more complex content use cases and reuse the `Content.bld/Content.json` file is opened if it exists.
  The file can contain multiple `ContentBuild.Sync` sections that pulls in files from 'external' locations.
  For example this script pulls in two files from `${ProjectRoot}/data`, the files are processed by the tool-chain and
  stored at the corrosponding location in `Content`.
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

## Valid SourcePath variables

Name            | Location
----------------|----------
$\{Content.bld} | This package's 'Content.bld' directory (This is the default value if SourcePath is not specified)
$\{Content}     | This package's 'Content' directory
$\{ProjectRoot} | The project root of this package.

## Requested features

- Build texture atlases.
- Generate bitmap font files.
- Sync files from external directory into Content directory.

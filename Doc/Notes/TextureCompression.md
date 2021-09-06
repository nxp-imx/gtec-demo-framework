# Texture compression notes

Format comparison

FOURCC|Name                         |Description              |Alpha premultiplied?|Compression ratio|Texture Type
------|-----------------------------|-------------------------|--------------------|-----------------|-------------
DXT1  |BC1                          |1-bit Alpha / Opaque     |Yes                 |6:1              |(for 24-bit source image) Simple non-alpha
DXT2  |BC2                          |Explicit alpha           |Yes                 |4:1              |Sharp alpha
DXT3  |BC2                          |Explicit alpha           |No                  |4:1              |Sharp alpha
DXT4  |BC3                          |Interpolated alpha       |Yes                 |4:1              |Gradient alpha
DXT5  |BC3                          |Interpolated alpha       |No                  |4:1              |Gradient alpha
N/A   |BC4                          |Interpolated greyscale   |N/A                 |2:1              |Gradient
N/A   |BC5                          |Interpolated two-channel |N/A                 |2:1              |Gradient
N/A   |ETC1                         |R8G8B8                   |N/A                 |?                |?
N/A   |ETC2                         |R8G8B8                   |N/A                 |?                |?
N/A   |RGBA8_ETC2_EAC               |R8G8B8A8 (bc3 like)      |?                   |?                |?
N/A   |R11_EAC                      |R8G8B8A8 (bc4 like)      |N/A                 |?                |?
N/A   |RG11_EAC                     |R8G8B8A8 (bc5 like)      |N/A                 |?                |?
N/A   |ETC2_PUNCHTHROUGH_ALPHA1_ETC2|1-bit Alpha / Opaque     |No                  |?                |?
N/A   |RGB_PVRTC_4BPPV1_IMG         |?                        |?                   |?                |?
N/A   |RGB_PVRTC_2BPPV1_IMG         |?                        |?                   |?                |?
N/A   |RGBA_PVRTC_4BPPV1_IMG        |?                        |?                   |?                |?
N/A   |RGBA_PVRTC_2BPPV1_IMG        |?                        |?                   |?                |?


## Source

* [BC: S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression)
* [ETC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression)
* [Texture compression techniques](https://sv-journal.org/2014-1/06/en/index.php?lang=en#6-3)
* [Unity texture compression](https://docs.unity3d.com/Manual/class-TextureImporterOverride.html)

import PhotoScan

path = "C:/Users/Ruolan Xu/Desktop/BME 436/testing/res.ply"


def reconstruct():
    doc = PhotoScan.app.document
    chunk = doc.addChunk()
    chunk.addPhotos(["C:/Program Files/Agisoft/PhotoScan Pro/photos/000.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/001.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/002.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/003.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/004.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/005.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/006.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/007.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/008.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/009.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/010.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/011.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/012.jpeg",
                     "C:/Program Files/Agisoft/PhotoScan Pro/photos/013.jpeg"])
    chunk.matchPhotos(accuracy=PhotoScan.MediumAccuracy,
                      preselection=PhotoScan.GenericPreselection)
    chunk.alignCameras()
    chunk.buildDenseCloud(quality=PhotoScan.MediumQuality)
    chunk.buildModel(surface=PhotoScan.Arbitrary,
                     interpolation=PhotoScan.EnabledInterpolation)
    chunk.buildUV(mapping=PhotoScan.GenericMapping)
    chunk.buildTexture(blending=PhotoScan.MosaicBlending, size=4096)

    chunk.exportPoints(path,
                       binary=True,
                       precision=6,
                       normals=True,
                       colors=True,
                       format=PhotoScan.PointsFormatPLY)


if __name__ == "__main__":
    reconstruct()

import PhotoScan

path = "C:/Users/Daniel Hull/Documents/Projects/BFPD/mesh.ply"


def reconstruct():
    doc = PhotoScan.app.document
    chunk = doc.addChunk()
    chunk.addPhotos(["C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img0.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img1.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img2.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img3.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img4.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img5.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img6.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img7.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img8.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img9.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img10.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img11.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img12.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img13.jpg",
                     "C:/Users/Daniel Hull/Documents/Projects/BFPD/software/fetch/img14.jpg"])
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

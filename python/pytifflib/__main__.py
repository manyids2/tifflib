import numpy as np
from dlup.writers import TifffileImageWriter, TiffCompression
from dlup import SlideImage


if __name__ == "__main__":
    slidepath = '../data/aperio/CMU-1.svs'
    slide = SlideImage.from_file_path(slidepath)
    print(slide)

    writer = TifffileImageWriter(
        filename="../data/dlup/slide1.itiff",
        size=(1024, 1024),
        mpp=1.0,
        tile_size=(256, 256),
        pyramid=True,
        compression=TiffCompression.JPEG,
        quality=100,
    )

    tiles = np.ones([16, 256, 256], dtype=np.uint8) * 128

    def iterator():
        for tile in tiles:
            yield tile

    writer.from_tiles_iterator(iterator())

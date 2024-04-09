FFmpeg with GL-Transitions
===========================

This is a fork of the FFmpeg project with the addition of the GL-Transitions.
This fork mantains by @Leask

## About FFmpeg

FFmpeg is a collection of libraries and tools to process multimedia content
such as audio, video, subtitles and related metadata.

## Libraries

* `libavcodec` provides implementation of a wider range of codecs.
* `libavformat` implements streaming protocols, container formats and basic I/O access.
* `libavutil` includes hashers, decompressors and miscellaneous utility functions.
* `libavfilter` provides a mean to alter decoded Audio and Video through chain of filters.
* `libavdevice` provides an abstraction to access capture and playback devices.
* `libswresample` implements audio mixing and resampling routines.
* `libswscale` implements color conversion and scaling routines.

## Tools

* [ffmpeg](https://ffmpeg.org/ffmpeg.html) is a command line toolbox to
  manipulate, convert and stream multimedia content.
* [ffplay](https://ffmpeg.org/ffplay.html) is a minimalistic multimedia player.
* [ffprobe](https://ffmpeg.org/ffprobe.html) is a simple analysis tool to inspect
  multimedia content.
* Additional small tools such as `aviocat`, `ismindex` and `qt-faststart`.

## Documentation

The offline documentation is available in the **doc/** directory.

The online documentation is available in the main [website](https://ffmpeg.org)
and in the [wiki](https://trac.ffmpeg.org).

### Examples

Coding examples are available in the **doc/examples** directory.

## License

FFmpeg codebase is mainly LGPL-licensed with optional components licensed under
GPL. Please refer to the LICENSE file for detailed information.

## Contributing

Patches should be submitted to the ffmpeg-devel mailing list using
`git format-patch` or `git send-email`. Github pull requests should be
avoided because they are not part of our review process and will be ignored.


## Build with gl-transitions instructions

- https://gl-transitions.com/
- https://donglumail.medium.com/3-methods-you-need-to-know-for-ffmpeg-transition-animation-7d2ea8f7ced7
- https://blog.csdn.net/weixin_40948587/article/details/121073081
- https://stackoverflow.com/questions/36548940/yasm-nasm-not-found-or-too-old-use-disable-yasm-for-a-crippled-build
- https://packages.debian.org/search?suite=default&section=all&arch=any&searchon=names&keywords=libxvid
- https://www.linuxquestions.org/questions/slackware-14/upgrading-ffmpeg-4175729823/
- https://gitweb.gentoo.org/repo/gentoo.git/plain/media-video/ffmpeg/files/ffmpeg-6.0-binutils-2.41.patch
- https://stackoverflow.com/questions/27272525/what-does-collect2-error-ld-returned-1-exit-status-mean
- https://github.com/stevenlovegrove/Pangolin/issues/399
- https://ffmpeg.org/pipermail/ffmpeg-devel/2018-November/236589.html
- https://www.linuxquestions.org/questions/slackware-14/rebuild-ffmpeg-fails-with-nvidia-legacy340-driver-installed-4175713073/
- https://www.linuxquestions.org/questions/programming-9/how-to-solve-%27collect2-ld-returned-1-exit-status-%27-4175624159/
- https://askubuntu.com/questions/429734/collect2-error-ld-returned-1-exit-status
- https://superuser.com/questions/1257225/collect2-error-ld-returned-1-exit-status-makefile107-recipe-for-target-bina
- https://blog.csdn.net/Asa_Ho/article/details/116096500

## Package dependencies

- libass-dev
- libfdk-aac-dev
- libgl1
- libgl1-mesa-glx
- libglew-dev
- libglew2.2
- libglib2.0-0
- libmp3lame-dev
- libopus-dev
- libtheora-dev
- libvorbis-dev
- libvpx-dev
- libx264-dev
- libx265-dev
- libxvidcore-dev


## Using gl-transitions
- https://shotstack.io/learn/use-ffmpeg-to-convert-images-to-video/
- https://github.com/transitive-bullshit/ffmpeg-gl-transition/blob/master/concat.sh
- https://superuser.com/questions/778762/crossfade-between-2-videos-using-ffmpeg
- https://superuser.com/questions/258032/is-it-possible-to-use-ffmpeg-to-trim-off-x-seconds-from-the-beginning-of-a-video
- https://video.stackexchange.com/questions/4563/how-can-i-crop-a-video-with-ffmpeg

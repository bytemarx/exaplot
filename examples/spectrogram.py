import math
import pathlib
import time

import librosa
import numpy
from orbital import RunParam, breakpoint, init, msg, plot
from scipy.interpolate import CubicSpline

init(
    [(0,0,0,2),(0,0,3,1),(0,0,5,0)],
    frame_rate=RunParam(12, "Frame Rate (1-60 fps)"),
    file=RunParam("examples/never-gonna-give-you-up.mp3", "MP3 file"),
)

plot[1].title = "Spectrogram"
plot[1].x_axis = "log(Hz)"
plot[1].two_dimen.line.type = "impulse"
plot[1].two_dimen.x_range = 1, 4.5
plot[1].two_dimen.y_range = -150, 0

plot[2].color_map.show()
plot[2].color_map.data_size = 10000, 1
plot[2].color_map.x_range = 1, 4.35
plot[2].color_map.z_range = -50, 0
plot[2].color_map.color.min = "#C63C00"
plot[2].color_map.color.max = "white"

plot[3].title = "Waveform"
plot[3].two_dimen.autorescale_axes = True


def run(frame_rate: int, file: str):
    if frame_rate <= 0 or frame_rate > 60:
        raise ValueError(f"Invalid frame rate: {frame_rate}")
    _file = pathlib.Path(file).resolve()
    if not _file.is_file():
        raise FileNotFoundError(f"Could not find {_file}")

    msg("Loading MP3 file...", append=False)
    audio, sample_rate = librosa.load(_file, sr=None)

    frame_length = int(sample_rate / frame_rate)
    frame_interval_ns = 1_000_000_000 // frame_rate
    sample_length = 1.0 / sample_rate
    n_bins = (sample_rate // (2 * frame_rate)) + 1
    bin_frequencies = [frame_rate * i for i in range(n_bins)]
    bin_frequencies_lg = [0] + [math.log(x, 10) for x in bin_frequencies[1:]]
    bin_frequencies_lg_interp = numpy.linspace(
        plot[2].color_map.x_range.min,
        plot[2].color_map.x_range.max,
        num=plot[2].color_map.data_size.x
    )

    msg("Processing waveform...", append=True)
    waveform_window = 0
    for i in range(0, len(audio), frame_length):
        frame_time_start = time.time_ns()
        breakpoint()

        # get a frame's worth of audio samples
        frame = audio[i : i + frame_length]
        if len(frame) < frame_length:
            break

        waveform_window += 1
        if waveform_window >= 10 * frame_rate:
            plot[3]()
            waveform_window = 0
        plot[3]([t * sample_length for t in range(i, i + frame_length)], frame)
        windowed_frame = frame * numpy.hanning(frame_length)
        spectrogram_lg = 10 * numpy.log(numpy.abs(numpy.fft.rfft(windowed_frame))) - 50
        plot[1]()
        plot[1](bin_frequencies_lg, spectrogram_lg)
        plot[2]([CubicSpline(bin_frequencies_lg, spectrogram_lg)(bin_frequencies_lg_interp)])
        frame_time_remaining = frame_interval_ns - (time.time_ns() - frame_time_start)
        if frame_time_remaining > 0:
            time.sleep(frame_time_remaining * 1e-9)
